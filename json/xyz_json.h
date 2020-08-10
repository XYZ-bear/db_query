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
	// parse the string in quotes like "xxx",return the index of the last quote
	// case: "xxx"
	int parse_str(const char* begin) {
		int index = 0;
		bool start = false;
		while (char ch = *(begin + index)) {
			if (ch == '"') {
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
	int parse_key_value(const char* begin) {
		//cout <<"len"<< strlen(begin) << endl;
		int index = 0;
		int key_start = -1;
		int key_end = -1;
		int val_start = -1;
		int val_end = -1;

		bool wait_val = false;
		while (char ch = *(begin + index)) {
			if (ch == '"') {
				if (key_start == -1) {
					key_start = index;
					index += parse_str(begin + index);
					key_end = index;
				}
				else {
					if (wait_val) {
						val_start = index;
						index += parse_str(begin + index);
						val_end = index;

						string key(begin + key_start + 1, key_end - key_start - 1);
						unserialize(key, begin + val_start, val_end - val_start + 1);

						//cout << string(begin + key_start, key_end - key_start + 1) << "  " << string(begin + val_start, val_end - val_start + 1) << endl;
						//cout << string(begin + val_start, val_end - val_start + 1) << endl;
						return index;
					}
				}
			}
			else if (wait_val && ch == '[') {
				string key(begin + key_start + 1, key_end - key_start - 1);
				//unserialize(key, begin + index, 0);

				//cout << string(begin + key_start, key_end - key_start + 1) << "   ";
				return index + parse_array(key, begin + index);
			}
			else if (wait_val && ch == '{') {
				//cout << string(begin + key_start, key_end - key_start + 1) << "   ";

				string key(begin + key_start + 1, key_end - key_start - 1);
				return index + unserialize(key, begin + index, 0);


				//return index + for_blob(begin + index);
			}
			else if (ch == ':') {
				wait_val = true;
			}
			else if (wait_val && val_start == -1 && ch != ' ') {
				val_start = index;
			}
			else if (val_start != -1 && (ch == ' ' || ch == ',' || ch == '}')) {
				val_end = index;

				string key(begin + key_start + 1, key_end - key_start - 1);
				unserialize(key, begin + val_start, val_end - val_start);

				//cout << string(begin + key_start, key_end - key_start + 1) << "  " << string(begin + val_start, val_end - val_start) << endl;
				return index - 1;
			}
			index++;
		}
		if (val_start != -1 && val_end == -1) {
			val_end = index - 1;
			cout << "k:" << string(begin + key_start, key_end - key_start + 1) << "  v:" << string(begin + val_start, val_end - val_start + 1) << endl;
		}
		else
			cout << begin << ":error" << endl;
		return index - 1;
	}

	int parse_array(string &key, const char* begin) {
		int index = 0;
		bool start = false;

		int val_start = -1;

		bool wait_val = true;
		while (char ch = *(begin + index)) {
			if (ch == ']') {
				return index;
			}
			else if (ch == '[') {}
			else if (ch == '"') {
				val_start = index;
				int len = parse_str(begin + index);
				//cout << string(begin + val_start, len + 1) << " ";

				unserialize(key, begin + val_start, len + 1);
				index += len;
				wait_val = false;
				val_start = -1;
			}
			else if (ch == ',') {
				wait_val = true;
			}
			else if (ch == '{') {
				val_start = index;
				index += unserialize(key, begin + index, 0);
				//index += for_blob(begin + index);
				wait_val = false;
				val_start = -1;
			}
			else if (wait_val && val_start == -1 && (ch != ' ' && ch != '\t')) {
				val_start = index;
			}
			else if (val_start != -1 && (ch == ' ' || ch == ',' || ch == ']' || ch == '}')) {
				//cout << string(begin + val_start, index - val_start - 1) << " ";
				unserialize(key, begin + val_start, index - val_start - 1);
				val_start = -1;
			}
			index++;
		}
		return index;
	}

public:
	int for_blob(const char* begin) {
		//cout << "len: " << strlen(begin) << endl;
		int index = 0;
		int start = -1;
		while (char ch = *(begin + index)) {
			if (ch == '{') {
				if (start != -1) {
					index += for_blob(begin + index);
				}
				else
					start = index;
			}
			else if (ch == '}') {
				if (start == -1) {
					cout << begin << ":error" << endl;
					return index;
				}
				return index;
			}
			else if (ch == '"') {
				if (start == -1) {
					cout << begin << ":error" << endl;;
					return index;
				}
				index += parse_key_value(begin + index);
			}
			else if (ch != ' ') {
				if (start == -1) {
					cout << begin << ":error" << endl;;
					return index;
				}
			}
			index++;
		}
		return index;
	}

	int blob(const char* begin) {
		const char* next = begin;
		int key_begin = -1;
		int key_end = 0;
		int val_begin = 0;
		int val_end = 0;
		int index = 0;
		bool is_blob = false;
		bool is_str = false;
		bool is_array = false;
		while (*next) {
			char ch = *next;
			if (ch == '{') {
				if (is_str == 0) {
					if (val_begin != 0) {
						is_blob = true;
						string key(begin + key_begin + 1, key_end - key_begin - 1);
						int i = unserialize(key, begin + val_begin + 1, val_end - val_begin - 1) + 1;
						next += i;
						index += i;
					}
					else {
						is_blob = true;
						int i = blob(next + 1) + 1;
						next += i;
						index += i;
					}
				}
			}
			else if (ch == '[') {
				is_array = true;
				val_begin = index;
			}
			else if (ch == '"') {
				is_blob = false;
				if (0 == val_begin) {
					if (key_begin != -1) {
						key_end = index;
					}
					else
						key_begin = index;
				}
				if (is_str)
					is_str = false;
				else
					is_str = true;
			}
			else if (ch == ':') {
				if (is_str == 0) {
					val_begin = index;
				}
			}
			else if (ch == ',') {
				if (is_str == 0) {
					val_end = index;
					if (!is_blob || is_array) {
						string key(begin + key_begin + 1, key_end - key_begin - 1);
						unserialize(key, begin + val_begin + 1, val_end - val_begin - 1);
					}
					if (is_array)
						val_begin = index;
					else {
						val_begin = 0;
						key_begin = -1;
					}
				}
			}
			else if (ch == ']') {
				if (is_str == 0) {
					val_end = index;
					if (!is_blob) {
						string key(begin + key_begin + 1, key_end - key_begin - 1);
						unserialize(key, begin + val_begin + 1, val_end - val_begin - 1);
					}
					is_array = false;

					val_begin = 0;
					key_begin = -1;
				}
			}
			else if (ch == '}') {
				if (is_str == 0) {
					if (key_begin != -1) {
						val_end = index;
						if (!is_blob) {
							string key(begin + key_begin + 1, key_end - key_begin - 1);
							unserialize(key, begin + val_begin + 1, val_end - val_begin - 1);
						}
						val_begin = 0;
						key_begin = -1;
					}
				}
				return index;
			}
			next++;
			index++;
		}
	}
	size_t unserialize(string &key, const char* val, size_t len) {
		auto iter = fields_.find(key);
		if (iter != fields_.end()) {
			return (((T*)this)->*(iter->second->unserialize))(val, len);
		}
		return 0;
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

#include <sstream>
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
		if (len > 0 && begin[0] >= '0' && begin[0] >= '9')
			return true;
		else
			return false;
	}
public:
	static size_t us(bool *data, const char* begin, size_t len) {
		string val(begin, len);
		if (val == "true")
			*data = true;
		else if(val == "false")
			*data = false;
		else
			*data = stoi(val);
		return 0;
	}
	static size_t us(int *data, const char* begin, size_t len) {
		if (check_can_convert_num(begin, len)) {
			string val(begin, len);
			*data = stoi(val);
		}
		return 0;
	}
	static size_t us(double *data, const char* begin, size_t len) {
		string val(begin, len);
		*data = stod(val);
		return 0;
	}
	static size_t us(string *data, const char* begin, size_t len) {
		data->assign(begin + 1, len - 2);
		return 0;
	}
	static size_t us(float *data, const char* begin, size_t len) {
		string val(begin, len);
		*data = stof(val);
		return 0;
	}
	template<class T>
	static size_t us(vector<T> *data, const char* begin, size_t len) {
		data->resize(data->size() + 1);

		//T v;
		size_t in = us(&(*data)[data->size() - 1], begin, len);
		//data->emplace_back(v);
		return in;
	}
	template<class T>
	static size_t us(json_base_t<T> *data, const char* begin, size_t len) {
		return data->for_blob(begin);
	}

	static void serialize(bool *data, string &res) {
		res += to_string(*data);
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
		return field_op::us(&name,begin,len);\
	}\
	void serialize_##name(string &res) {\
		field_op::serialize(&name,res);\
	}\
	static constexpr date_imple_t<child_t> const_impl_##name = { &child_t::unserialize_##name,&child_t::serialize_##name };\
	colloect collect__##name{this,#name,&const_impl_##name};\
public:\

#define Json(name)\
class name :public json_base_t<name>