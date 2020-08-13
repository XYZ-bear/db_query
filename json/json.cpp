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
	////cout << "len: " << strlen(begin) << endl;
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
	//cout << begin << "str:error" << endl;
	return index;
}

//{fsff{ sfsdf{} }}";
int for_blob(const char* begin) {
	////cout << "len: " << strlen(begin) << endl;
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
				//cout << begin << ":error" << endl;
				return index;
			}
			return index;
		}
		else if (ch == '"') {
			if (start == -1) {
				//cout << begin << ":error" << endl;;
				return index;
			}
			index += for_key_value(begin + index);
		}
		else if(ch != ' '){
			if (start == -1) {
				//cout << begin << ":error" << endl;;
				return index;
			}
		}
		index++;
	}
	return index;
}

int for_key_value(const char* begin) {
	////cout <<"len"<< strlen(begin) << endl;
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
					//cout << string(begin + key_start, key_end - key_start + 1) <<"  "<< string(begin + val_start, val_end - val_start + 1)<< endl;
					////cout << string(begin + val_start, val_end - val_start + 1) << endl;
					return index;
				}
			}
		}
		else if (wait_val && ch == '['){
			//cout << string(begin + key_start, key_end - key_start + 1) << "   ";
			return index + for_array(begin + index);
		}
		else if (wait_val && ch == '{') {
			//cout << string(begin + key_start, key_end - key_start + 1) << "   ";
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
			//cout << string(begin + key_start, key_end - key_start + 1) << "  " << string(begin + val_start, val_end - val_start) << endl;
			return index - 1;
		}
		index++;
	}
	if (val_start != -1 && val_end == -1) {
		val_end = index - 1;
		//cout << "k:" << string(begin + key_start, key_end - key_start + 1) << "  v:" << string(begin + val_start, val_end - val_start + 1) << endl;
	}
	else
		//cout << begin << ":error" << endl;
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

//int for_value(const char* begin) {
//	////cout << "len: " << strlen(begin) << endl;
//	int index = 0;
//	bool start = false;
//	while (char ch = *(begin + index)) {
//		if (ch != ' ') {
//			start = true;
//		}
//
//		index++;
//	}
//	//cout << begin << "str:error" << endl;
//	return index;
//}


int for_array(const char* begin) {
	int index = 0;

	int val_start = -1;

	while (char ch = *(begin + index)) {
		if (ch == ']') {
			if (val_start != -1)
				//cout << string(begin + val_start, index - val_start) << " ";
			return index;
		}
		else if (ch == '[') {
			index += for_array(begin + index + 1);
			val_start = -1;
		}
		else if (ch == '"') {
			val_start = index;
			int len = for_str(begin + index);
			//cout << string(begin + val_start, len + 1) <<" ";
			index += len;
			val_start = -1;
		}
		else if (val_start != -1 && (ch == ' ' || ch == ',' || ch == ']' || ch == '}')) {
			//cout << string(begin + val_start, index - val_start) << " ";
			val_start = -1;
		}
		else if (ch == '{') {
			val_start = index;
			index += for_blob(begin + index);
			val_start = -1;
		}
		else if (val_start == -1 && (ch != ' ' && ch != '\t' && ch != ',' && ch != '}')) {
			val_start = index;
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
	cout << for_key_value("\"a\":-12321") << endl;
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
	cout << " " << for_array("[123,123,123]") <<endl;
	cout << " " << for_array("[1 , 2, 3 ]") << endl;
	cout << " " << for_array(" [1 , 2, 3 ] ") << endl;
	cout << " " << for_array("[ \"a\" , \"b\",\"c\" ]") << endl;
	cout << " " << for_array("[[1,2],[3,4] , [ 5 , 6 ]]") << endl;
	cout << " " << for_array("[[[1,2]],[[3,4]] , [[ 5 , 6] ]]") << endl;
	cout << " " << for_array("[{\"a\":[1,2]},{\"b\":[5,6]}]") << endl;
	cout << " " << for_blob(R({
		"profile": {
		"firstName": [[1]],
			"lastName" : "John",
			"age" : 30})) << endl;
	//cout << " " << for_array("[\"aa\",\"bb\",\"cc\"]");
	//cout << " " << for_array("[{\"a\":\"sfsdf\"},{\"a\":\"sfsfdsfdf\"]");
	//cout << " " << for_array(R([{ "name":"Porsche", "models" : ["911", "Taycan"] },
	//	{ "name":"BMW", "models" : ["M5", "M3", "X5"] },
	//	{ "name":"Volvo", "models" : ["XC60", "V60"] }])) << endl;
	//cout << " " << for_array("[[1,2]]");

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

Json(HeWeather6){
public:
	Json(Basic) {
	public:
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

Json(Test4)
{
public:
	Json(profile_t) {
	public:
		string N(lastName);
		vector<vector<int>> N(firstName);
		double N(age);
		string N(gender);
		Json(contact_t) {
			string N(type);
			string N(number);
		};
		vector<contact_t> N(contact);
		Json(address_t) {
			string N(postal_code);
			string N(state);
			string N(street);
			string N(city);
		};
		address_t N(address);
		bool N(marital_status);
	};
	profile_t N(profile);
};

void utf_8() {
	ifstream myfile("utf_8.txt");
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

	Test4 ffff;
	ffff.unserialize(jj.data());
	getchar();
}


class kkfs:public nlohmann::json_sax<nlohmann::json>
{
public:
	/*!
	@brief a null value was read
	@return whether parsing should proceed
	*/
	bool null() { return true; }

	/*!
	@brief a boolean value was read
	@param[in] val  boolean value
	@return whether parsing should proceed
	*/
	bool boolean(bool val) { return true; }

	/*!
	@brief an integer number was read
	@param[in] val  integer value
	@return whether parsing should proceed
	*/
	virtual bool number_integer(number_integer_t val) { return true; }

	/*!
	@brief an unsigned integer number was read
	@param[in] val  unsigned integer value
	@return whether parsing should proceed
	*/
	virtual bool number_unsigned(number_unsigned_t val) { return true; }

	/*!
	@brief an floating-point number was read
	@param[in] val  floating-point value
	@param[in] s    raw token value
	@return whether parsing should proceed
	*/
	virtual bool number_float(number_float_t val, const string_t& s) { return true; }

	/*!
	@brief a string was read
	@param[in] val  string value
	@return whether parsing should proceed
	@note It is safe to move the passed string.
	*/
	virtual bool string(string_t& val) { return true; }

	/*!
	@brief a binary string was read
	@param[in] val  binary value
	@return whether parsing should proceed
	@note It is safe to move the passed binary.
	*/
	virtual bool binary(binary_t& val) { return true; }

	/*!
	@brief the beginning of an object was read
	@param[in] elements  number of object elements or -1 if unknown
	@return whether parsing should proceed
	@note binary formats may report the number of elements
	*/
	virtual bool start_object(std::size_t elements) { return true; }

	/*!
	@brief an object key was read
	@param[in] val  object key
	@return whether parsing should proceed
	@note It is safe to move the passed string.
	*/
	virtual bool key(string_t& val) { 
		return true; }

	/*!
	@brief the end of an object was read
	@return whether parsing should proceed
	*/
	virtual bool end_object() { return true; }

	/*!
	@brief the beginning of an array was read
	@param[in] elements  number of array elements or -1 if unknown
	@return whether parsing should proceed
	@note binary formats may report the number of elements
	*/
	virtual bool start_array(std::size_t elements) { return true; }

	/*!
	@brief the end of an array was read
	@return whether parsing should proceed
	*/
	virtual bool end_array() { return true; }

	/*!
	@brief a parse error occurred
	@param[in] position    the position in the input where the error occurs
	@param[in] last_token  the last read token
	@param[in] ex          an exception object describing the error
	@return whether parsing should proceed (must return false)
	*/
	virtual bool parse_error(std::size_t position,
		const std::string& last_token,
		const nlohmann::detail::exception& ex) {
		return true;
	}

	virtual ~kkfs() {};
};

int main()
{
	// callback to set binary_seen to true if a binary value was seen
	bool binary_seen = false;
	auto callback = [&binary_seen](int /*depth*/, nlohmann::json::parse_event_t /*event*/, nlohmann::json & parsed)
	{
		return true;
	};

	nlohmann::json j;
	//auto cbp = nlohmann::detail::json_sax_dom_callback_parser<nlohmann::json>(j, callback, true);
	kkfs cbp;

	//perf_test("table build 1", 1000000, [&cbp]()->void {
	//	nlohmann::json::sax_parse(R({
	//		"profile": {
	//		"firstName": [[1]],
	//			"lastName" : "John",
	//			"age" : -30.3,
	//			"gender" : "Male",
	//			"address" : {
	//			"street": "20th 2nd Street",
	//				"city" : "New York",
	//				"state" : "NY",
	//				"postal_code" : "10003"
	//		},
	//			"contact": [
	//			{
	//				"type": "Home",
	//					"number" : "(735) 754-0100"
	//			},
	//		  {
	//			  "type": "Office",
	//			  "number" : "(725) 854-0750"
	//		  }
	//			],
	//				"marital_status": true
	//	}
	//		}), &cbp, nlohmann::json::input_format_t::json);
	//});

	double aff = 23.003;
	//united_test();
	//utf_8();

	string res;
	int a = 0;
	perf_test("table build 1", 1000, [&res,&a]()->void {
		Test4 ffff;
		ffff.unserialize(R({
			"profile": {
			"firstName": [[1,2]],
				"lastName" : "&#x4F60;&#x597D;",
				"age" : -30.3,
				"gender" : "Male",
				"address" : {
				"street": "20th 2nd Street",
					"city" : "New York",
					"state" : "NY",
					"postal_code" : "10003"
			},
				"contact": [
				{
					"type": "Home",
						"number" : "(735) 754-0100"
				},
		  {
			  "type": "Office",
			  "number" : "(725) 854-0750"
		  }
				],
					"marital_status": true
		}
			}));
		//a+=ffff.profile.lastName.size();
	});

	perf_test("table build 1", 10000, [&a]()->void {
		nlohmann::json patch = R"({
		"profile": {
		"firstName": [[1]],
			"lastName" : "&#x4F60;&#x597D;",
			"age" : 30,
			"gender" : "Male",
			"address" : {
			"street": "20th 2nd Street",
				"city" : "New York",
				"state" : "NY",
				"postal_code" : "10003"
		},
			"contact": [
			{
				"type": "Home",
					"number" : "(735) 754-0100"
			},
	  {
		  "type": "Office",
		  "number" : "(725) 854-0750"
	  }
			],
				"marital_status": true
	}
		}
	)"_json;
		a+=patch["profile"]["lastName"].get<string>().size();
	});
	//cout << res << endl;
	//for_blob(res.data());
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
	f.unserialize(R(
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

	HeWeather6 hw;
	hw.basics;

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
