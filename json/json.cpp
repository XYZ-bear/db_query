// json.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <string>
#include <fstream> 
#include <iostream>
#include <unordered_map>
using namespace std;

int blob(const char* begin);
int parase(const char* begin);
int for_key_value(const char* begin);
int for_array(const char* begin);

int for_str(const char* begin) {
	//cout << "len: " << strlen(begin) << endl;
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

//{fsff{ sfsdf{} }}";
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
			index += for_key_value(begin + index);
		}
		else if(ch != ' '){
			if (start == -1) {
				cout << begin << ":error" << endl;;
				return index;
			}
		}
		index++;
	}
	return index;
}

int for_key_value(const char* begin) {
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
				index += for_str(begin + index);
				key_end = index;
				
			}
			else {
				if (wait_val) {
					val_start = index;
					index += for_str(begin + index);
					val_end = index;
					cout << string(begin + key_start, key_end - key_start + 1) <<"  "<< string(begin + val_start, val_end - val_start + 1)<< endl;
					//cout << string(begin + val_start, val_end - val_start + 1) << endl;
					return index;
				}
			}
		}
		else if (wait_val && ch == '['){
			cout << string(begin + key_start, key_end - key_start + 1) << "   ";
			return index + for_array(begin + index);
		}
		else if (wait_val && ch == '{') {
			cout << string(begin + key_start, key_end - key_start + 1) << "   ";
			return index + for_blob(begin + index);
		}
		else if (ch == ':') {
			wait_val = true;
		}
		else if (wait_val && val_start == -1 && ch != ' ') {
			val_start = index;
		}
		else if (val_start != -1 && (ch == ' ' || ch == ',' || ch == '}')) {
			val_end = index;
			cout << string(begin + key_start, key_end - key_start + 1) << "  " << string(begin + val_start, val_end - val_start) << endl;
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


int parase(const char* begin) {
	int index = 0;
	while (char ch = *(begin + index)) {
		if (ch == '{') {
			index+= for_blob(begin + index);
		}
		//else if (ch == '"') {
		//	index += for_key_value(begin + index);
		//}
		//else if (ch == '[') {
		//	index += for_array(begin + index);
		//}
		const char* cc = begin + index;
		index++;
	}
	return index;
}


int for_array(const char* begin) {
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
			int len = for_str(begin + index);
			cout << string(begin + val_start, len + 1) <<" ";
			index += len;
			wait_val = false;
			val_start = -1;
		}
		else if (ch == ',') {
			wait_val = true;
		}
		else if (ch == '{') {
			val_start = index;
			index += for_blob(begin + index);
			wait_val = false;
			val_start = -1;
		}
		else if (wait_val && val_start == -1 && (ch != ' ' && ch != '\t')) {
			val_start = index;
		}
		else if (val_start != -1 && (ch == ' ' || ch == ',' || ch == ']' || ch == '}')) {
			cout << string(begin + val_start, index - val_start - 1) << " ";
			val_start = -1;
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
					cout << "key:" << string(begin + key_begin + 1, key_end - key_begin - 1) << endl;
				}
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
				if ( key_begin != -1) {
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
				if (!is_blob) {
					if (is_array) {
						int i = for_array(next + 1);
						//int i = blob(next + 1) + 1;
						next += i;
						index += i;
					}
					else {
						cout << "key:" << string(begin + key_begin + 1, key_end - key_begin - 1) << endl;
						cout << "val:" << string(begin + val_begin + 1, val_end - val_begin - 1) << endl;
					}
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
					cout << "key:" << string(begin + key_begin + 1, key_end - key_begin - 1) << endl;
					cout << "val:" << string(begin + val_begin + 1, val_end - val_begin - 1) << endl;
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
						cout << "key:" << string(begin + key_begin + 1, key_end - key_begin - 1) << endl;
						cout << "val:" << string(begin + val_begin + 1, val_end - val_begin - 1) << endl;
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

#include "xyz_json.h"

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
	vector<Car> N(cars);
};

#include <map>
#include <functional>
#include <time.h>

void perf_test(string name, size_t t, function<void()> f) {
	clock_t startTime, endTime;
	startTime = clock();
	for (size_t i = 0; i < t; i++)
		f();
	endTime = clock();
	cout << name << ":" << endTime - startTime << "ms" << endl;
}
#define R(...) #__VA_ARGS__

void united_test() {
	cout << "----------------for str-----------------" << endl;
	cout << for_str("\"sdfdsf\"") << endl;

	//const char* str = "\"a\":\"sfsdf\"";
	cout << "----------------for key_value-----------------" << endl;
	cout << for_key_value("\"a\":\"sfsdf\"") << endl;
	cout << for_key_value("\"a\":12321") << endl;
	cout << for_key_value(" \"a{}[]  \":  12321") << endl;

	cout << "----------------for blob-----------------" << endl;
	cout << " " << for_blob("{\"a\":\"sfsdf\"}") << endl;
	cout << " " << for_blob("\"a\":12321") << endl;
	cout << " " << for_blob(" \"a{}[]  \":  12321") << endl;
	cout << " " << for_blob("{\"a\":12321}") << endl;
	cout << " " << for_blob(" { \"a{}[]  \":  12321}") << endl;
	cout << " " << for_blob("{\"employee\":{ \"name\":\"Bill Gates\", \"age\" : 62, \"city\" : \"Seattle\" },\"job\":\"seal\"}") << endl;
	cout << " " << for_blob("{\"employee\":{ \"name\":\"Bill Gates\", \"age\" : 62, \"city\" : \"Seattle\" },\"job\":[\"seal\",\"sdf\"]}") << endl;

	cout << "----------------for array-----------------" << endl;
	cout << " " << for_array("[123,123,123]");
	cout << " " << for_array("[\"aa\",\"bb\",\"cc\"]");
	cout << " " << for_array("[{\"a\":\"sfsdf\"},{\"a\":\"sfsfdsfdf\"]");
	cout << " " << for_array(R([{ "name":"Porsche", "models" : ["911", "Taycan"] },
		{ "name":"BMW", "models" : ["M5", "M3", "X5"] },
		{ "name":"Volvo", "models" : ["XC60", "V60"] }])) << endl;


	for_blob(R(
		{
			"employees": [
	{
		"firstName": "Bill",
			"lastName" : "Gates"
	},
		{
			"firstName": "George",
			"lastName" : "Bush"
		},
		{
			"firstName": "Thomas",
			"lastName" : "Carter"
		}
			]
		}
	));
}

void serialize_test() {

}

void complex_test() {

}

Json(HeWeather6)
{
	Json(Basic) {
		std::string N(cid);// : "CN101010100",
		std::string	N(location);// " : "北京",
		std::string	N(parent_city);// : "北京",
		std::string	N(admin_area);// : "北京",
		std::string	N(cnty);// : "中国",
		std::string	N(lat);// : "39.90498734",
		std::string	N(lon);// : "116.40528870",
		std::string	N(tz);// : "8.0"
		int N(ca);
	};
	Basic N(basic);
	vector<Basic> N(basics);
};

#include "json.hpp"

Json(OO1)
{
public:
	string N(firstName);
	string N(lastName);
};

Json(OO)
{
public:
	vector<OO1> N(employees);
};

int main()
{
	string res;
	HeWeather6 ffff;
	ffff.serialize(res);
	cout << res << endl;
	for_blob(res.data());
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

	//perf_test("table build 1", 1, [&jj]()->void {
	//	for (int i = 0; i < 100000; i++)
	//		parase(jj.data());
	//});
	jj += "}";
	nlohmann::json j_patch = R"(		{
			"employees": [
	{
		"firstName": "Bill",
		"lastName" : "Gates"
	},
	{
		"firstName": "George",
		"lastName" : "Bush"
	},
	{
		"firstName": "Thomas",
		"lastName" : "Carter"
	}
			]
		}
	)"_json;
	//cout<<j_patch["employees"][0]["firstName"].get<string>();
	//nlohmann::json fsf;
	//fsf.parse(jj.data(), jj.data() + jj.size());
	//cout << fsf["name"].get<std::string>() << endl;


	//perf_test("table build 1", 1, [&jj]()->void {
	//	for (int i = 0; i < 1000000; i++) {
	//		nlohmann::json j_patch = R"(		{
	//		"employees": [
	//{
	//	"firstName": "Bill",
	//	"lastName" : "Gates"
	//},
	//{
	//	"firstName": "George",
	//	"lastName" : "Bush"
	//},
	//{
	//	"firstName": "Thomas",
	//	"lastName" : "Carter"
	//}
	//		]
	//	}
	//)"_json;
	//		j_patch["employees"][0]["firstName"].get<string>();
	//	}
	//});


	//perf_test("table build 1", 1, [&jj]()->void {
	//	for (int i = 0; i < 1000000; i++) {
	//		OO nn;
	//		nn.for_blob(R({
	//			"employees": [
	//		{
	//			"firstName": "Bill",
	//				"lastName" : "Gates"
	//		},
	//{
	//	"firstName": "George",
	//	"lastName" : "Bush"
	//},
	//{
	//	"firstName": "Thomas",
	//	"lastName" : "Carter"
	//}
	//			]
	//			}));
	//		nn.employees[0].firstName;
	//		
	//	}
	//});
	//NN nn;
	//nn.for_blob(jj.data());


	HeWeather6 f;
	f.for_blob(R(
	{
		"basic": {
			"cid": "CN101010100",
				"location" : "北京",
				"parent_city" : "北京",
				"admin_area" : "北京",
				"cnty" : "中国",
				"lat" : "39.90498734",
				"lon" : "116.40528870",
				"tz" : "8.0"
		}}));
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
