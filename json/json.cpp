// json.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <string>
#include <fstream> 
#include <iostream>
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
		if (ch == '{' || ch == '[') {
			if (is_str == 0) {
				is_blob = true;
				int i = blob(next + 1) + 1;
				next += i;
				index += i;
			}
		}
		else if (ch == '"') {
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
				if(!is_blob)
				cout << string(begin + key_begin, val_end - key_begin) << endl;
				val_begin = 0;
				key_begin = 0;
			}
		}
		else if (ch == '}' || ch == ']') {
			if (is_str == 0) {
				val_end = index;
				if (!is_blob)
				cout << string(begin + key_begin, val_end - key_begin) << endl;
				val_begin = 0;
				key_begin = 0;
			}
			return index;
		}
		next++;
		index++;
	}
}




int main()
{
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


    std::cout << "Hello World!\n"; 
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
