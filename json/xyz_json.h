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


#define check_result(res)  if (!res) { return false; }

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
	bool parse_str() {
		bool start = false;
		while (ch) {
			if (ch == json_key_symbol::str) {
				if (start)
					return true;
				start = true;
			}
			get_next();
		}
		return true;
	}

	// case 1:"xxx":xxx
	// case 2:"xxx":"xxx"
	// case 3:"xxx":{xxx}
	// case 4:"xxx":[xxx]
	bool parse_key_value() {
		const char* key_start = nullptr;
		const char* key_end = nullptr;
		const char* val_start = nullptr;

		bool wait_val = false;
		while (ch) {
			if (ch == json_key_symbol::str) {
				//ready to pares key
				if (!key_start) {
					key_start = next;
					check_result(parse_str());
					key_end = next;
				}
				else {
					//case 2:"xxx" : "xxx"
					if (wait_val) {
						val_start = next;
						check_result(parse_str());

						string key(key_start + 1, key_end);
						unserialize(key, val_start, next - val_start + 1);
						return true;
					}
				}
			}
			// case 4:"xxx":[xxx]
			else if (wait_val && ch == json_key_symbol::array_begin) {
				get_next();
				string key(key_start + 1, key_end);
				return parse_array(key);
			}
			// case 3:"xxx":{xxx}
			else if (wait_val && ch == json_key_symbol::object_begin) {
				string key(key_start + 1, key_end);
				get_next(unserialize(key, next, end - next));
				return true;
			}
			// ready to pares value
			else if (ch == json_key_symbol::key_value_separator) {
				wait_val = true;
			}
			// get the first index of the value(escape white space and controll char)
			else if (wait_val && !val_start && !is_ctr_or_space_char(ch)) {
				val_start = next;
			}
			// case 1:"xxx" : xxx
			else if (val_start && (is_ctr_or_space_char(ch) || ch == json_key_symbol::next_key_value || ch == json_key_symbol::object_end)) {
				string key(key_start + 1, key_end);
				unserialize(key, val_start, next - val_start);

				return true;
			}
			get_next();
		}
		return false;
	}

	// parse array with 4 cases
	// case 1:["xxx",...]
	// case 2:[xxx,...]
	// case 3:[{xxx},...]
	// case 4:[[xxx],...]
	bool parse_array(string &key){
		const char* val_start =nullptr;

		while (ch) {
			if (ch == json_key_symbol::array_end) {
				if (val_start)
					unserialize(key, val_start, next - val_start);
				return true;
			}
			else if (ch == json_key_symbol::array_begin) {
				get_next();
				check_result(parse_array(key));
				val_start = nullptr;
			}
			//pares string value
			else if (ch == json_key_symbol::str) {
				val_start = next;
				check_result(for_str(next));
				unserialize(key, val_start, next - val_start);
				val_start = nullptr;
			}
			//pares value
			else if (val_start && (is_ctr_or_space_char(ch) || ch == json_key_symbol::next_key_value || ch == json_key_symbol::array_end || ch == json_key_symbol::object_end)) {
				unserialize(key, val_start, next - val_start);
				val_start = nullptr;
			}
			else if (ch == json_key_symbol::object_begin) {
				val_start = next;
				get_next(unserialize(key, next, end - next));
				val_start = nullptr;
			}
			else if (!val_start && (!is_ctr_or_space_char(ch) && ch != json_key_symbol::next_key_value && ch != json_key_symbol::object_end)) {
				val_start = next;
			}
			get_next();
		}
		return false;
	}

	// pares object:
	// case 1:{"xxx":{xxx}}
	// return |      |len|| 
	// return |----len----|
	bool parse_object() {
		// '{' must appear befor '}',set this as flag
		bool start = false;
		while (ch) {
			// obj begin
			if (ch == json_key_symbol::object_begin) {
				if (start) {
					check_result(parse_object());
				}
				else
					start = true;
			}
			else if (ch == json_key_symbol::object_end) {
				if (!start) {
					cout << begin << ":error" << endl;
					return false;
				}
				return true;
			}
			else if (ch == json_key_symbol::str) {
				check_result(parse_key_value());
			}
			else if ( !is_ctr_or_space_char(ch)) {
				if (!start) {
					return false;
				}
			}
			get_next();
		}
		return false;
	}

	void inline get_next(size_t step = 1) {
		next += step;
		if (end && end < next)
			ch = '\0';
		else
			ch = *next;
	}

	size_t unserialize(string &key, const char* val, size_t len) {
		auto iter = fields_.find(key);
		if (iter != fields_.end()) {
			return (((T*)this)->*(iter->second->unserialize))(val, len);
		}
		return 0;
	}
public:
	size_t unserialize(const char* json) {
		begin = json;
		end = nullptr;
		next = json;
		ch = *begin;
		check_result(parse_object());
		return next - begin;
	}
	size_t unserialize(const char* json, size_t size) {
		begin = json;
		end = json + size;
		next = json;
		ch = *begin;
		check_result(parse_object());
		return next - begin;
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
	const char* begin;
	const char* end;
	const char* next;
	char ch;
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