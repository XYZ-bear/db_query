// json.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <string>
#include <fstream> 
#include <iostream>
#include <unordered_map>
using namespace std;

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
					cout << "key:" << string(begin + key_begin + 1, key_end - key_begin - 1) << endl;
				}
				key_begin = 0;
				is_blob = true;
				int i = blob(next + 1) + 1;
				next += i;
				index += i;
			}
		}
		else if (ch == '[') {
			is_array = true;
			val_begin = index;
		}
		else if (ch == '"') {
			is_blob = false;
			if (0 == val_begin) {
				if ( key_begin != 0) {
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
				if (key_begin == -1)
					key_begin++;
				if (!is_blob) {
					cout << "key:" << string(begin + key_begin + 1, key_end - key_begin - 1) << endl;
					cout << "val:" << string(begin + val_begin + 1, val_end - val_begin - 1) << endl;
				}

				if (is_array)
					val_begin = index;
				else {
					val_begin = 0;
					key_begin = 0;
				}

			}
		}
		else if (ch == '}' || ch == ']') {
			if (is_str == 0) {
				val_end = index;
				if (!is_blob) {
					cout << "key:" << string(begin + key_begin + 1, key_end - key_begin - 1) << endl;
					cout << "val:" << string(begin + val_begin + 1, val_end - val_begin - 1) << endl;
				}
				val_begin = 0;
				key_begin = 0;
			}
			return index;
		}
		next++;
		index++;
	}
}

template<class T>
struct date_imple_t {
	typedef size_t(T::*unserialize_t)(const char*,size_t);
	typedef void(T::*serialize_t)();
	unserialize_t unserialize;
	serialize_t serialize;
};

class colloect
{
public:
	template<class T>
	colloect(T *obj, const char* name, const date_imple_t<T> *field) {
		obj->add_field(name, field);
	};
};

template<class T>
class json_base_t {
public:
	typedef T child_t;
	void add_field(const char* name, const date_imple_t<T>* field) {
		fields_[name] = field;
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
					if (key_begin != 0) {
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
					if (key_begin == -1)
						key_begin++;
					if (!is_blob) {
						string key(begin + key_begin + 1, key_end - key_begin - 1);
						unserialize(key, begin + val_begin + 1, val_end - val_begin - 1);
					}
					if (is_array)
						val_begin = index;
					else {
						val_begin = 0;
						key_begin = 0;
					}
				}
			}
			else if (ch == '}' || ch == ']') {
				if (is_str == 0) {
					val_end = index;
					if (!is_blob) {
						string key(begin + key_begin + 1, key_end - key_begin - 1);
						unserialize(key, begin + val_begin + 1, val_end - val_begin - 1);
					}
					val_begin = 0;
					key_begin = 0;
				}
				return index;
			}
			next++;
			index++;
		}
	}
	size_t unserialize(string &key,const char* val,size_t len) {
		auto iter = fields_.find(key);
		if (iter != fields_.end()) {
			return (((T*)this)->*(iter->second->unserialize))(val,len);
		}
		return 0;
	}
private:
	static std::unordered_map<string, const date_imple_t<T>*> fields_;
protected:
	static T instance_;
};
template<class T>
std::unordered_map<string, const date_imple_t<T>*> json_base_t<T>::fields_;
template<class T>
T json_base_t<T>::instance_;

#include <sstream>
class field_op {
public:
	static size_t us(int *data,const char* begin,size_t len){
		string val(begin,len);
		*data = stoi(val);
		return 0;
	}
	static size_t us(double *data, const char* begin, size_t len) {
		string val(begin, len);
		*data = stod(val);
		return 0;
	}
	static size_t us(string *data, const char* begin, size_t len) {
		data->assign(begin + 1, len -2);
		return 0;
	}
	static size_t us(float *data, const char* begin, size_t len) {
		string val(begin, len);
		*data = stof(val);
		return 0;
	}
	template<class T>
	static size_t us(vector<T> *data, const char* begin, size_t len) {
		T v;
		us(&v, begin, len);
		data->push_back(v);
		return 0;
	}
	template<class T>
	static size_t us(json_base_t<T> *data, const char* begin, size_t len) {
		return data->blob(begin + 1);
	}
};

#define N(name) \
name;\
private:\
	size_t unserialize_##name( const char* begin,size_t len) {\
		return field_op::us(&name,begin,len);\
	}\
	void serialize_##name() {\
	}\
	static constexpr date_imple_t<child_t> const_impl_##name = { &child_t::unserialize_##name,&child_t::serialize_##name };\
	colloect collect__##name{this,#name,&const_impl_##name};\
public:

#define Json(name)\
class name :public json_base_t<name>

Json(ff2) {
public:
	int N(a);
	double N(b);
};

Json(ff) {
public:
	int N(a);
	double N(b);
	string N(c);
	float N(d);
	ff2 N(e);
	vector<int> N(f);
};

Json(Car) {
public:
	string N(name);
	vector<string> N(models);
};

Json(NN) {
public:
	string N(name);
	int N(age);
	Car N(cars);
};

#include <map>



int main()
{
	const char* jo = "{  \"a\":1,\"b\":2,\"e\":{\"a\":1,\"b\":2},\"c\":\"sfsf\",\"d\":3,\"f\":[1,2,3]}";
	blob(jo);

	ff kkl;
	kkl.blob(jo);
	ifstream myfile("hello.txt");
	string jj;
	string temp;
	if (!myfile.is_open())
	{
		cout << "未成功打开文件" << endl;
	}
	while (getline(myfile, temp))
	{
		jj += temp;
	}
	myfile.close();


	const char *json = "{\"employee\":{ \"name\":\"Bill Gates\", \"age\" : 62, \"city\" : \"Seattle\" },\"job\":\"seal\"}";
	blob(jj.data());
	NN nn;
	nn.blob(jj.data());

    std::cout << "Hello World!\n"; 
	getchar();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
