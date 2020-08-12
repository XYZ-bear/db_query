#pragma once

#include <iostream>
#include <string>
#include <fstream> 
#include <iostream>
#include <unordered_map>

template<class T>
struct date_imple_t {
	typedef size_t(T::*unserialize_t)(const char*, size_t);
	typedef void(T::*serialize_t)(string &res);
	unserialize_t unserialize;
	serialize_t serialize;
};

class colloect
{
public:
	template<class T>
	colloect(T *obj, const char* name, const date_imple_t<T> *field) {
		if (!obj->inited_)
			obj->__add_field(name, field);
	};
};

template<class T>
class json_base_t {
	enum json_key_symbol
	{
		object_begin = '{',
		object_end = '}',

		array_begin = '[',
		array_end = ']',

		key_value_separator = ':',
		next_key_value = ',',
		str = '"',
		space = ' '
	};
public:
	typedef T child_t;
	void __add_field(const char* name, const date_imple_t<T>* field) {
		if (fields_.find(name) == fields_.end()) {
			fields_[name] = field;
		}
		else {
			inited_ = true;
		}
	}
private:
	//escape the controll char like \t \r \f etc and whitespace
	bool inline is_ctr_or_space_char(char ch) {
		return (ch == ' ' || (ch >= 0x00 && ch <= 0x1F) || ch == 0x7F);
	}

	// parse the string in quotes like "xxx",return the index of the last quote
	// case: "xxx"
	int parse_str(const char* begin, const char* end = nullptr) {
		int index = 0;
		bool start = false;
		while (char ch = *(begin + index)) {
			if (ch == json_key_symbol::str) {
				if (start)
					return index;
				start = true;
			}
			index++;
		}
		cout << begin << "str:error" << endl;
		return index;
	}

	// case 1:"xxx":xxx
	// case 2:"xxx":"xxx"
	// case 3:"xxx":{xxx}
	// case 4:"xxx":[xxx]
	int parse_key_value(const char* begin, const char* end = nullptr) {
		int index = 0;
		int key_start = -1;
		int key_end = -1;
		int val_start = -1;
		int val_end = -1;

		bool wait_val = false;
		while (char ch = *(begin + index)) {
			if (ch == json_key_symbol::str) {
				//ready to pares key
				if (key_start == -1) {
					key_start = index;
					index += parse_str(begin + index);
					key_end = index;
				}
				else {
					//case 2:"xxx" : "xxx"
					if (wait_val) {
						val_start = index;
						index += parse_str(begin + index);
						val_end = index;

						string key(begin + key_start + 1, key_end - key_start - 1);
						unserialize(key, begin + val_start, val_end - val_start + 1);
						return index;
					}
				}
			}
			// case 4:"xxx":[xxx]
			else if (wait_val && ch == json_key_symbol::array_begin) {
				string key(begin + key_start + 1, key_end - key_start - 1);
				return index + parse_array(key, begin + index);
			}
			// case 3:"xxx":{xxx}
			else if (wait_val && ch == json_key_symbol::object_begin) {
				string key(begin + key_start + 1, key_end - key_start - 1);
				return index + unserialize(key, begin + index, 0);
			}
			// ready to pares value
			else if (ch == json_key_symbol::key_value_separator) {
				wait_val = true;
			}
			// get the first index of the value(escape white space and controll char)
			else if (wait_val && val_start == -1 && !is_ctr_or_space_char(ch)) {
				val_start = index;
			}
			// case 1:"xxx" : xxx
			else if (val_start != -1 && (is_ctr_or_space_char(ch) || ch == json_key_symbol::next_key_value || ch == json_key_symbol::object_end)) {
				val_end = index;

				string key(begin + key_start + 1, key_end - key_start - 1);
				unserialize(key, begin + val_start, val_end - val_start);

				return index - 1;
			}
			index++;
		}
		if (val_start != -1 && val_end == -1) {
			val_end = index - 1;
		}
		else
			cout << begin << ":error" << endl;
		return index - 1;
	}

	// parse array with 4 cases
	// case 1:["xxx",...]
	// case 2:[xxx,...]
	// case 3:[{xxx},...]
	// case 4:[[xxx],...]
	int parse_array(string &key, const char* begin, const char* end = nullptr){
		int index = 0;

		int val_start = -1;

		while (char ch = *(begin + index)) {
			if (ch == json_key_symbol::array_end) {
				if (val_start != -1) {
					unserialize(key, begin + val_start, index - val_start);
				}
				return index;
			}
			else if (ch == json_key_symbol::array_begin) {
				index += parse_array(key, begin + index + 1);
				val_start = -1;
			}
			else if (ch == json_key_symbol::str) {
				val_start = index;
				int len = for_str(begin + index);
				unserialize(key, begin + val_start, len + 1);
				index += len;
				val_start = -1;
			}
			else if (val_start != -1 && (is_ctr_or_space_char(ch) || ch == json_key_symbol::next_key_value || ch == json_key_symbol::array_end || ch == json_key_symbol::object_end)) {
				unserialize(key, begin + val_start, index - val_start);
				val_start = -1;
			}
			else if (ch == json_key_symbol::object_begin) {
				val_start = index;
				index += unserialize(key, begin + index, 0);
				val_start = -1;
			}
			else if (val_start == -1 && (!is_ctr_or_space_char(ch) && ch != json_key_symbol::next_key_value && ch != json_key_symbol::object_end)) {
				val_start = index;
			}

			index++;
		}
		return index;
	}

	// pares object:
	// case 1:{"xxx":{xxx}}
	int parse_blob(const char* begin, const char* end = nullptr) {
		int index = 0;
		int start = -1;
		while (char ch = *(begin + index)) {
			if (ch == json_key_symbol::object_begin) {
				if (start != -1) {
					index += parse_blob(begin + index);
				}
				else
					start = index;
			}
			else if (ch == json_key_symbol::object_end) {
				if (start == -1) {
					cout << begin << ":error" << endl;
					return index;
				}
				return index;
			}
			else if (ch == json_key_symbol::str) {
				if (start == -1) {
					cout << begin << ":error" << endl;;
					return index;
				}
				index += parse_key_value(begin + index);
			}
			else if ( !is_ctr_or_space_char(ch)) {
				if (start == -1) {
					cout << begin << ":error" << endl;;
					return index;
				}
			}
			index++;
		}
		return index;
	}

	size_t unserialize(string &key, const char* val, size_t len) {
		auto iter = fields_.find(key);
		if (iter != fields_.end()) {
			return (((T*)this)->*(iter->second->unserialize))(val, len);
		}
		return 0;
	}
public:
	size_t unserialize(const char* begin) {
		return parse_blob(begin);
	}
	size_t unserialize(const char* json, size_t size) {
		return parse_blob(begin, json + size);
	}
	void serialize(string &res) {
		res += "{";
		for (auto &filed : fields_) {
			res += "\"" + filed.first + "\":";
			(((T*)this)->*(filed.second->serialize))(res);
			res += ',';
		}
		if (fields_.size() > 0)
			res.pop_back();
		res += "}";
	}
private:
	static std::unordered_map<string, const date_imple_t<T>*> fields_;
protected:
	static T instance_;
public:
	static bool inited_;
};
template<class T>
std::unordered_map<string, const date_imple_t<T>*> json_base_t<T>::fields_;
template<class T>
T json_base_t<T>::instance_;
template<class T>
bool json_base_t<T>::inited_ = false;


class field_op {
private:
	bool is_null(const char* begin, size_t len) {
		if (len == 4 && begin[0] == 'n' && begin[1] == 'u' && begin[2] == 'l' && begin[3] == 'l') {
			return true;
		}
		else
			return false;
	}
	char is_bool(const char* begin, size_t len) {
		if (len == 4 && begin[0] == 't' && begin[1] == 'r' && begin[2] == 'u' && begin[3] == 'e') {
			return true;
		}
		else if (len == 5 && begin[0] == 'f' && begin[1] == 'a' && begin[2] == 'l' && begin[3] == 's' && begin[3] == 'e') {
			return false;
		}
		else
			return -1;
	}
	static bool check_can_convert_num(const char* begin, size_t len) {
		if (len > 0 && begin[0] >= '0' && begin[0] <= '9')
			return true;
		else if (len > 1 && begin[0] == '-' && begin[1] >= '0' && begin[1] <= '9')
			return true;
		else
			return false;
	}
public:
	static size_t unserialize(bool *data, const char* begin, size_t len) {
		string val(begin, len);
		if (val == "true")
			*data = true;
		else if(val == "false")
			*data = false;
		else
			*data = stoi(val);
		return 0;
	}
	static size_t unserialize(int *data, const char* begin, size_t len) {
		if (check_can_convert_num(begin, len)) {
			string val(begin, len);
			*data = stoi(val);
		}
		return 0;
	}
	static size_t unserialize(double *data, const char* begin, size_t len) {
		string val(begin, len);
		*data = stod(val);
		return 0;
	}
	static size_t unserialize(string *data, const char* begin, size_t len) {
		data->assign(begin + 1, len - 2);
		return 0;
	}
	static size_t unserialize(float *data, const char* begin, size_t len) {
		string val(begin, len);
		*data = stof(val);
		return 0;
	}
	template<class T>
	static size_t unserialize(vector<T> *data, const char* begin, size_t len) {
		data->resize(data->size() + 1);
		return unserialize(&(*data)[data->size() - 1], begin, len);
	}
	template<class T>
	static size_t unserialize(json_base_t<T> *data, const char* begin, size_t len) {
		return data->unserialize(begin);
	}

	static void serialize(bool *data, string &res) {
		if (*data) {
			res += "true";
		}
		else
			res += "false";
	}

	static void serialize(int *data, string &res) {
		res += to_string(*data);
	}
	static void serialize(double *data, string &res) {
		res += to_string(*data);
	}
	static void serialize(string *data, string &res) {
		res += "\"" + *data + "\"";
	}
	static void serialize(float *data, string &res) {
		res += to_string(*data);
	}
	template<class T>
	static void serialize(vector<T> *data, string &res) {
		res += '[';
		for (auto &item : *data) {
			serialize(&item, res);
			res += ',';
		}
		if (data->size() > 0)
			res.pop_back();
		res += ']';
	}
	template<class T>
	static void serialize(json_base_t<T> *data, string &res) {
		data->serialize(res);
	}
};

#define N(name) \
name;\
private:\
	size_t unserialize_##name( const char* begin,size_t len) {\
		return field_op::unserialize(&name,begin,len);\
	}\
	void serialize_##name(string &res) {\
		field_op::serialize(&name,res);\
	}\
	static constexpr date_imple_t<child_t> const_impl_##name = { &child_t::unserialize_##name,&child_t::serialize_##name };\
	colloect collect__##name{this,#name,&const_impl_##name};\
public:\

#define Json(name)\
class name :public json_base_t<name>