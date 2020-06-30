#pragma once
#include <tuple>

//字节长度
#define SIZE_1_BYTE 1
#define SIZE_2_BYTE 2
#define SIZE_4_BYTE 4
#define SIZE_8_BYTE 8

//字段类型定义
enum WireType
{
	WireType_Varint = 0,
	WireType_Fix32 = 1,
	WireType_Fix64 = 2,
	WireType_Length_Limited = 3,
};
#define WIRE_TYPE_LEN 2
typedef uint32_t id_type;

//宏重载
#define PRIVATE_ARGS_GLUE(x, y) x y
#define PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT(_1,_2,_3,_4,_5,_6,_7,_8,_9, N, ...) N
#define PRIVATE_MACRO_VAR_ARGS_IMPL(args)  PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT args
#define COUNT_MACRO_VAR_ARGS(...)  PRIVATE_MACRO_VAR_ARGS_IMPL((__VA_ARGS__,9,8,7,6,5,4,3,2,1,0))

#define PRIVATE_MACRO_CHOOSE_HELPER3(M,count)  M##count
#define PRIVATE_MACRO_CHOOSE_HELPER2(M,count) PRIVATE_MACRO_CHOOSE_HELPER3(M,count)
#define PRIVATE_MACRO_CHOOSE_HELPER1(M,count)  PRIVATE_MACRO_CHOOSE_HELPER2(M,count)
#define PRIVATE_MACRO_CHOOSE_HELPER(M,count)   PRIVATE_MACRO_CHOOSE_HELPER1(M,count)

#define INVOKE_VAR_MACRO(M,...) PRIVATE_ARGS_GLUE(PRIVATE_MACRO_CHOOSE_HELPER(M,COUNT_MACRO_VAR_ARGS(__VA_ARGS__)), (__VA_ARGS__))

//统计参数个数
#define VA_COUNT(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value

//溢出检查
#define OVERFLOW_CHECK(begin,end,size) if ((begin+size) > end) return 0;

//检查相等
#define CHECK_DEFAULT(data,default_value) if(data==default_value) return true;else return false;

//fast_protobuf基类前置声明
template<class T>
class proto_base;

//fixed32,fixed64,enum
template<class VT, class RT = VT>
class operator_base
{
public:
	RT value;

	typedef VT value_type;
	operator_base() {}

	template<class T>
	void operator=(const T &v) {
		value = v;
	}

	template<class T>
	void operator=(T v) {
		value = v;
	}
	template<class T>
	operator const T &()const {
		return (T&)value;
	}
	template<class T>
	bool operator==(T v) {
		return value == v;
	}
	template<class T>
	bool operator!=(T v) {
		return value != v;
	}
};

class fixed32 :public operator_base<fixed32,uint32_t> {
public:
	template<class T>
	void operator=(T v) {
		value = v;
	}
	template<class T>
	fixed32(T v) {
		value = v;
	}
	fixed32() {}
};

class fixed64 :public operator_base<fixed64,uint64_t> {
public:
	template<class T>
	void operator=(T v) {
		value = v;
	}
	template<class T>
	fixed64(T v) {
		value = v;
	}
	fixed64() {}
};

template<class VT>
class enumm :public operator_base<VT,int64_t>
{
public:
	template<class T>
	void operator=(const T v) {
		this->value = (VT)v;
	}
	template<class T>
	enumm(T v) {
		this->value = (VT)v;
	}
	enumm() {}
};