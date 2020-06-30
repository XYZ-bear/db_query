#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <time.h>
#include <queue>
#include <string>
#include <stdarg.h>
#include <thread>
#include <queue>
#include <mutex>
#include <type_traits>
#include <cstring>
#include <unordered_map>
#include "concurrentqueue.h"
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;

///-------------------------------db struct-----------------------
#define primary_key 0x1
#define auto_increment 0x2

template<class T>
class __db_struct_base;

class field_operator {
public:
	static void get_feild_value(int32_t &val, sql::ResultSet *res, size_t index) {
		val = res->getInt((uint32_t)index);
	}

	static void get_feild_value(int64_t &val, sql::ResultSet *res, size_t index) {
		val = res->getInt64((uint32_t)index);
	}

	static void get_feild_value(uint32_t &val, sql::ResultSet *res, size_t index) {
		val = res->getUInt((uint32_t)index);
	}

	static void get_feild_value(uint64_t &val, sql::ResultSet *res, size_t index) {
		val = res->getUInt64((uint32_t)index);
	}

	static void get_feild_value(bool &val, sql::ResultSet *res, size_t index) {
		val = res->getBoolean((uint32_t)index);
	}

	static void get_feild_value(string &val, sql::ResultSet *res, size_t index) {
		val.append(res->getString((uint32_t)index).c_str());
	}

	static void get_feild_value(double &val, sql::ResultSet *res, size_t index) {
		val = res->getDouble((uint32_t)index);
	}

	static void get_feild_value(float &val, sql::ResultSet *res, size_t index) {
		val = (float)res->getDouble((uint32_t)index);
	}
public:
	static void get_feild_value_str(int32_t &val, string& str) {
		str += to_string(val);
	}

	static void get_feild_value_str(int64_t &val, string& str) {
		str += to_string(val);
	}

	static void get_feild_value_str(uint32_t &val, string& str) {
		str += to_string(val);
	}

	static void get_feild_value_str(uint64_t &val, string& str) {
		str += to_string(val);
	}

	static void get_feild_value_str(bool &val, string& str) {
		str += to_string(val);
	}

	static void get_feild_value_str(string &val, string& str) {
		str += "'";
		str += val;
		str += "'";
	}

	static void get_feild_value_str(double &val, string& str) {
		str += to_string(val);
	}

	static void get_feild_value_str(float &val, string& str) {
		str += to_string(val);
	}
public:
	static int64_t get_feild_value_i64(int32_t &val) {
		return val;
	}

	static int64_t get_feild_value_i64(int64_t &val) {
		return val;
	}

	static int64_t get_feild_value_i64(uint32_t &val) {
		return val;
	}

	static int64_t get_feild_value_i64(uint64_t &val) {
		return val;
	}

	template<class T>
	static int64_t get_feild_value_i64(T &val) {
		return 0;
	}
public:
	static void prepare_set(const int32_t &val, sql::PreparedStatement *ps, size_t &index) {
		ps->setInt((uint32_t)index++, val);
	}

	static void prepare_set(int64_t &val, sql::PreparedStatement *ps, size_t &index) {
		ps->setInt64((uint32_t)index++, val);
	}

	static void prepare_set(uint32_t &val, sql::PreparedStatement *ps, size_t &index) {
		ps->setUInt((uint32_t)index++, val);
	}

	static void prepare_set(uint64_t &val, sql::PreparedStatement *ps, size_t &index) {
		ps->setUInt64((uint32_t)index++, val);
	}

	static void prepare_set(bool &val, sql::PreparedStatement *ps, size_t &index) {
		ps->setBoolean((uint32_t)index++, val);
	}

	static void prepare_set(const string &val, sql::PreparedStatement *ps, size_t &index) {
		ps->setString((uint32_t)index++, val.c_str());
	}

	static void prepare_set(double &val, sql::PreparedStatement *ps, size_t &index) {
		ps->setDouble((uint32_t)index++, val);
	}

	static void prepare_set(float &val, sql::PreparedStatement *ps, size_t &index) {
		ps->setDouble((uint32_t)index++, val);
	}

	template<class T>
	static void prepare_set(const __db_struct_base<T> &val, sql::PreparedStatement *ps, size_t &index) {
		__db_struct_base<T> &rv = const_cast<__db_struct_base<T>&>(val);
		rv.___prepare_value(ps, index);
	}
};

struct tbl_fld_link {
	constexpr tbl_fld_link(const char* t, const char* f):tbl(t),fld(f) {}
	const char* tbl;
	const char* fld;
};

template<class T>
struct tbl_fld_link2:public tbl_fld_link{
	typedef void(T::*ref_type)(sql::ResultSet *res, size_t index);
	typedef void(T::*vs_type)(string &str);
	typedef int64_t(T::*vi_type)();
	typedef void(T::*pv_type)(sql::PreparedStatement *ps, size_t &index);

	constexpr tbl_fld_link2(const char* t, const char* f, ref_type _ref, vs_type _vs, vi_type _vi, pv_type _pv, char _flag):tbl_fld_link(t,f),ref(_ref),vs(_vs),vi(_vi),pv(_pv),flag(_flag){}
	ref_type ref;
	vs_type vs;
	vi_type vi;
	pv_type pv;
	char flag;

	void do_ref(T *obj, sql::ResultSet *res, size_t index) {
		(obj->*ref)(res, index);
	}

	void do_vs(T *obj, string &str) {
		(obj->*vs)(str);
	}

	int64_t do_vi(T *obj) {
		return (obj->*vi)();
	}

	void do_pv(T *obj, sql::PreparedStatement *ps, size_t &index) {
		(obj->*pv)(ps, index);
	}

	bool check_flag(char f) {
		return flag & f;
	}
};

class colloect
{
public:
	template<class T>
	colloect(T *obj, const tbl_fld_link2<T> *field) {
		if (obj->__inited == false) {
			obj->___add_field(field);
		}
	};
};


template<char C1 = 0, char C2 = 0, char C3 = 0, char C4 = 0, char C5 = 0, char C6 = 0, char C7 = 0, char C8 = 0>
constexpr char flagg() {
	return 0 | C1 | C2 | C3 | C4 | C5 | C6 | C7 | C8;
}


#define table(name)\
extern const char name_##name[]=#name;\
class name :public __db_struct_name<name,name_##name>

//static constexpr在当作参数传递时在c17以下的版本存在缺陷，会报undefine错误，不过在c17中得到了修复
//https://stackoverflow.com/questions/8016780/undefined-reference-to-static-constexpr-char

#define sql_field(type,name,...) \
public:\
	type name;\
	void ___ref_##name(sql::ResultSet *res, size_t index){\
		field_operator::get_feild_value(name,res,index);\
	}\
	void ___get_##name##_value_str(string &str){\
		field_operator::get_feild_value_str(name,str);\
	}\
	int64_t ___get_##name##_value_i64(){\
		return field_operator::get_feild_value_i64(name);\
	}\
	void ___prepare_##name##_value(sql::PreparedStatement *ps, size_t &index){\
		field_operator::prepare_set(name,ps,index);\
	}\
	static constexpr tbl_fld_link2<__TBT> _##name= {_NAME,#name,&__TBT::___ref_##name,&__TBT::___get_##name##_value_str,&__TBT::___get_##name##_value_i64,&__TBT::___prepare_##name##_value,flagg<__VA_ARGS__>()};\
private:\
	colloect collect_##name{this,&_##name};

template<class T>
class __db_struct_base {
public:
	typedef T __TBT;
	const char* _tbn;
private:
	void build_common_sql() {
		string space;
		__insert_str = "insert into ";
		__insert_str += _tbn;
		__insert_str += '(';

		__update_str = "update ";
		__update_str += _tbn;
		__update_str += " set ";

		for (auto fi : fields_) {
			if (!fi.second->check_flag(auto_increment)) {
				__insert_str += fi.first;
				__insert_str += ',';
				space += "?,";

				__update_str += fi.first;
				__update_str += "=?,";
			}
		}
		__insert_str.pop_back();
		__insert_str += ") values(";
		space.pop_back();
		__insert_str += space + ')';

		__update_str.pop_back();
	}
public:
	__db_struct_base() {
		__instance; //模板类需要显示激活
		fields_;
		//__inited = true;
	}
	~__db_struct_base() {
		//build_common_sql();
		//__inited = true;
	}
	bool ___unpack(sql::ResultSet *res) {
		auto f = res->getMetaData();
		if (res) {
			if (res->getMetaData()->getColumnCount() > 0 && res->getMetaData()->getTableName(1).compare(_tbn) == 0) {
				for (unsigned int i = 1; i <= res->getMetaData()->getColumnCount(); i++) {
					this->___do_ref(res->getMetaData()->getColumnName(i).c_str(), res, i);
				}
				return true;
			}
		}
		return false;
	}
	bool ___unpack(sql::ResultSet *res, int64_t &key) {
		if (res) {
			if (res->getMetaData()->getColumnCount() > 0 && res->getMetaData()->getTableName(1).compare(_tbn) == 0) {
				for (unsigned int i = 1; i <= res->getMetaData()->getColumnCount(); i++) {
					auto iter = fields_.find(res->getMetaData()->getColumnName(i).c_str());
					if (iter != fields_.end()) {
						(((T*)this)->*(iter->second.ref))(res, i);
						if (iter->second.check_flag(primary_key)) {
							key = (((T*)this)->*(iter->second.vi))();
						}
					}
				}
				return true;
			}
		}
		return false;
	}
	bool ___unpack(sql::ResultSet *res, string &key) {
		if (res) {
			if (res->getMetaData()->getColumnCount() > 0 && res->getMetaData()->getTableName(1).compare(_tbn) == 0) {
				for (unsigned int i = 1; i <= res->getMetaData()->getColumnCount(); i++) {
					auto iter = fields_.find(res->getMetaData()->getColumnName(i).c_str());
					if (iter != fields_.end()) {
						(((T*)this)->*(iter->second.ref))(res, i);
						if (iter->second.check_flag(primary_key)) {
							(((T*)this)->*(iter->second.vs))(key);
						}
					}
				}
				return true;
			}
		}
		return false;
	}
	void ___add_field(const tbl_fld_link2<T> *field) {
		if (!__inited) {
			auto iter = fields_.find(field->fld);
			if (iter == fields_.end()) {
				auto &f = fields_[field->fld];
				f = const_cast<tbl_fld_link2<T>*>(field);
			}
			else {
				build_common_sql();
				__inited = true;
			}
		}
		//if (!__inited) {
		//	auto &f = fields_[field->fld];
		//	f = const_cast<tbl_fld_link2<T>*>(field);
		//}
	};
	bool ___do_ref(const char* ele, sql::ResultSet *res, size_t index) {
		auto iter = fields_.find(ele);
		if (iter != fields_.end()) {
			iter->second->do_ref((T*)this, res, index);
			return true;
		}
		return false;
	}
	void ___prepare_value(sql::PreparedStatement *ps, size_t &index) {
		for (auto &field : fields_) {
			if (!field.second->check_flag(auto_increment)) {
				field.second->do_pv((T*)this, ps, index);
			}
		}
	}
	void ___where_key(string &str) {
		str += " where ";
		for (auto &field : fields_) {
			if (field.second->check_flag(primary_key)) {
				str += field.first;
				str += "=";
				field.second->do_vs((T*)this, str);
				return;
			}
		}
	}
private:
	static std::unordered_map<string, tbl_fld_link2<T>*> fields_;
public:
	static string __insert_str;
	static string __update_str;
	static bool __inited;
	static T __instance;
};
template<class T>
T __db_struct_base<T>::__instance;
template<class T>
std::unordered_map<string, tbl_fld_link2<T>*> __db_struct_base<T>::fields_;
template<class T>
bool __db_struct_base<T>::__inited = false;
template<class T>
string __db_struct_base<T>::__insert_str;
template<class T>
string __db_struct_base<T>::__update_str;


template<class T, const char* nae>
class __db_struct_name:public __db_struct_base<T> {
public:
	static constexpr const char* _NAME = nae;
	__db_struct_name(){
		__db_struct_base<T>::_tbn = nae;
	}
};

///-------------------------somthing about sql query------------------


///参数萃取 https://zhuanlan.zhihu.com/p/102240099
namespace detail {
	template<typename R, typename ...As>
	struct __function_traits_base {
		using function_type = std::function<R(As...)>;

		using result_type = R;

		using argument_types = std::tuple<As...>;

		using no_reference_types = std::tuple<typename std::decay<As>::type...>;
	};

	template<typename F>
	struct __function_traits;
	template<typename F>
	struct __function_traits<std::reference_wrapper<F>> : public __function_traits<F> {
		using result_type = int;
	};
	template<typename R, typename ...As>
	struct __function_traits<R(*)(As...)> : public __function_traits_base<R, As...> {};
	template<typename R, typename C, typename ...As>
	struct __function_traits<R(C::*)(As...)> : public __function_traits_base<R, As...> {};
	template<typename R, typename C, typename ...As>
	struct __function_traits<R(C::*)(As...) const> : public __function_traits_base<R, As...> {};
	template<typename F>
	struct __function_traits : public __function_traits<decltype(&F::operator())> {};
}

namespace multi_args_and_unpack {
	template<class T>
	static bool unpack(T &ele, sql::ResultSet *query_set) {
		ele.___unpack(query_set);
		return false;
	}

	template<class T>
	static bool unpack(vector<T> &ele, sql::ResultSet *query_set) {
		T val;
		val.___unpack(query_set);
		ele.emplace_back(val);
		return true;
	}

	template<class V>
	static bool unpack(map<int64_t, V> &ele, sql::ResultSet *query_set) {
		int64_t key;
		V val;
		val.unpack(query_set, key);
		ele[key] = val;
		return true;
	}

	template<class V>
	static bool unpack(map<string, V> &ele, sql::ResultSet *query_set) {
		string key;
		V val;
		val.unpack(query_set, key);
		ele[key] = val;
		return true;
	}

	static bool unpack(sql::ResultSet *res, sql::ResultSet *query_set) {
		res = query_set;
		return false;
	}

	static bool unpack(bool res, sql::ResultSet *query_set) {
		return false;
	}

	template<typename Tuple, int N = std::tuple_size<Tuple>::value>
	struct multi_args
	{
		static bool arg(Tuple& t, sql::ResultSet *query_set) {
			multi_args<Tuple, N - 1>::arg(t, query_set);
			using type = typename std::tuple_element<N - 1, Tuple>::type;
			type& v = std::get<N - 1>(t);
			return unpack(v, query_set);
		}
	};

	template<typename Tuple>
	struct multi_args<Tuple, 1>
	{
		static bool arg(Tuple& t, sql::ResultSet *query_set) {
			using type = typename std::tuple_element<0, Tuple>::type;
			type& v = std::get<0>(t);
			return unpack(v, query_set);
		}
	};
};

namespace multi_args_and_prepare {
	class mag {
	public:
		sql::PreparedStatement* pre;
		size_t index = 1;
		template<class ELEMENT, class ...ELEMENTS>
		void args(const ELEMENT& ele, const ELEMENTS&... eles) {
			field_operator::prepare_set(ele, pre, index);
			args(eles...);
		}
		template<class ELEMENT>
		void args(const ELEMENT& ele) {
			field_operator::prepare_set(ele, pre, index);
		}
	};
};

template<class ...T>
class res_set {	
	std::tuple<T...> eles;
public:
	template<class V>
	V& get() {
		return std::get<V>(eles);
	}

	void unpack(sql::ResultSet *query_set) {
		multi_args_and_unpack::multi_args<std::tuple<T...>>::arg(eles, query_set);
	}
};

class asyn_data_base {
public:
	string sql;
	moodycamel::ConcurrentQueue<asyn_data_base*> *res_queue_;
	virtual ~asyn_data_base() {
	}
	virtual void query(sql::Statement *stmt) = 0;
	virtual void result() = 0;
};

class asyn_update_data :public asyn_data_base
{
private:
	function<void(bool result)> func;

	bool r;
public:
	~asyn_update_data() {}
	asyn_update_data(const function<void(bool result)> &f, const char* s, moodycamel::ConcurrentQueue<asyn_data_base*> *queue) {
		func = f;
		sql = s;
		res_queue_ = queue;
	}

	void query(sql::Statement *stmt) {
		r = stmt->executeUpdate(sql.c_str());
		res_queue_->enqueue(this);
	}
	void result() {
		func(r);
	}
};

template<class F, class P = detail::__function_traits<typename std::decay<F>::type>>
class asyn_prepare_data :public asyn_data_base
{
private:
	function<void(sql::PreparedStatement*)> prepare_func;
	typename P::function_type func;
	typename P::no_reference_types eles;
public:
	~asyn_prepare_data() {}
	asyn_prepare_data(const F &f, function<void(sql::PreparedStatement*)>&pf, const char* s, moodycamel::ConcurrentQueue<asyn_data_base*> *queue) {
		func = f;
		prepare_func = pf;
		sql = s;
		res_queue_ = queue;
	}
	void query(sql::Statement *stmt) {
		sql::Connection *con = stmt->getConnection();
		if (sql::PreparedStatement *ps = con->prepareStatement(sql.c_str())) {
			prepare_func(ps);
			sql::ResultSet *res = ps->executeQuery();
			while (res->next()) {
				if (!multi_args_and_unpack::multi_args<typename P::no_reference_types>::arg(eles, res))
					break;
			}
			res->close();
			delete res;
			res_queue_->enqueue(this);
			ps->close();
			delete ps;
		}
	}

	void result() {
		constexpr auto size = std::tuple_size<typename P::no_reference_types>::value;
		invoke_impl(std::make_index_sequence<size>{});
	}

	template<std::size_t... Index>
	void invoke_impl(std::index_sequence<Index...>){
		func(std::get<Index>(eles)...);
	}

};

template<class F, class P = detail::__function_traits<typename std::decay<F>::type>>
class asyn_query_data3 :public asyn_data_base
{
private:
	typename P::function_type func;
	typename P::no_reference_types eles;
	bool update_res;
public:
	~asyn_query_data3() {}
public:
	asyn_query_data3(const F &f, const char* s, moodycamel::ConcurrentQueue<asyn_data_base*> *queue) {
		func = f;
		sql = s;
		res_queue_ = queue;
	}

	void query(sql::Statement *stmt) {
		if (std::is_same<typename P::no_reference_types, std::tuple<bool>>::value) {
			update_res = stmt->executeUpdate(sql.c_str());
		}
		else {
			sql::ResultSet *res = stmt->executeQuery(sql.c_str());
			while (res->next()) {
				if (!multi_args_and_unpack::multi_args<typename P::no_reference_types>::arg(eles, res))
					break;
			}
			res->close();
			delete res;
		}
		res_queue_->enqueue(this);
	}

	void result() {
		constexpr auto size = std::tuple_size<typename P::no_reference_types>::value;
		invoke_impl(std::make_index_sequence<size>{});
	}

	template<std::size_t... Index>
	void invoke_impl(std::index_sequence<Index...>)
	{
		func(std::get<Index>(eles)...);
	}
};

void push_query(asyn_data_base *base);
bool query(sql::Statement *fs);

class db_query;

class sql_query {
#pragma region sql_split
private:
	char sql_[256];
	char* pos;
private:
	void cpy(const char* src, int len) {
		memcpy(pos, src, len);
		pos += len;
	}
	void appand(int val) {
		sprintf(pos, "%d", val);
		pos += strlen(pos);
		cpy(",", 1);
	}

	void appand(const char* val) {
		cpy(val, (int)strlen(val));
		cpy(",", 1);
	}

	void appand(string& val) {
		cpy(val.c_str(), (int)val.length());
		cpy(",", 1);
	}

	void appand(tbl_fld_link &link) {
		cpy(link.tbl, (int)strlen(link.tbl));
		cpy(".", 1);
		cpy(link.fld, (int)strlen(link.fld));
		cpy(",", 1);
	}
	template<class ELEMENT, class ...ELEMENTS>
	sql_query &args(ELEMENT& ele, ELEMENTS&... eles) {
		appand(ele);
		args(eles...);
		return *this;
	}
	template<class ELEMENT>
	sql_query &args(ELEMENT& ele) {
		appand(ele);
		return *this;
	}
public:
	sql_query() {
		pos = sql_;
	}
	static sql_query & instance() {
		static sql_query db_;
		db_.reset();
		return db_;
	}
	char* end() {
		return sql_;
	}
	void reset() {
		memset(sql_, 0, 256);
		pos = sql_;
	}
	sql_query & update(const char* tbl) {
		cpy("update ", 7);
		cpy(tbl, (int)strlen(tbl));
		cpy(" set ", 5);
		return *this;
	}
	template<class T>
	sql_query & update(T &val) {
		string &str = T::__update_str;
		val.___where_key(str);
		cpy(str.c_str(), (int)str.length());
		return values(val);
	}
	sql_query & insert(const char* fld) {
		cpy("insert into ", 12);
		cpy(fld, (int)strlen(fld));
		return *this;
	}
	template<class T>
	sql_query & insert(T &val) {
		string &str = T::insert_str;;
		cpy(str.c_str(), str.length());
		return values(val);
	}

	function<void(sql::PreparedStatement*)> prepare_func;
	bool pre;
	template<class ...ELEMENTS>
	sql_query &values(ELEMENTS&... eles) {
		prepare_func = [eles...](sql::PreparedStatement* prepare) {
			multi_args_and_prepare::mag mmgg;
			mmgg.pre = prepare;
			mmgg.args(eles...);
		};
		pre = true;
		return *this;
	}

	template<class ...ELEMENTS>
	sql_query &select(ELEMENTS... eles) {
		cpy("select ", 7);
		args(eles...);
		pos -= 1;
		return *this;
	}

	sql_query &select() {
		cpy("select *", 8);
		return *this;
	}

	template<class ...ELEMENTS>
	sql_query &from(ELEMENTS&... eles) {
		cpy(" from ", 6);
		args(eles...);
		pos -= 1;
		return *this;
	}

	sql_query &select(const char* ele) {
		cpy("select ", 7);
		cpy(ele, (int)strlen(ele));
		return *this;
	}
	sql_query &count(const char* fld) {
		cpy(" count( ", 7);
		cpy(fld, (int)strlen(fld));
		cpy(" )", 2);
		return *this;
	}
	sql_query &where() {
		cpy(" where ", 7);
		return *this;
	}

	sql_query &from(const char* tbl) {
		cpy(" from ", 6);
		cpy(tbl, (int)strlen(tbl));
		return *this;
	}

	sql_query &equal(const tbl_fld_link &link, int val) {
		cpy(link.tbl, (int)strlen(link.tbl));
		cpy(".", 1);
		cpy(link.fld, (int)strlen(link.fld));
		cpy("=", 1);
		sprintf(pos, "%d", val);
		pos += strlen(pos);
		return *this;
	}

	sql_query &equal(const tbl_fld_link &link, const char* val) {
		cpy(link.tbl, (int)strlen(link.tbl));
		cpy(".", 1);
		cpy(link.fld, (int)strlen(link.fld));
		cpy("='", 2);
		cpy(val, (int)strlen(val));
		cpy("'", 1);
		return *this;
	}

	sql_query &equal(const char* key, int val) {
		cpy(key, (int)strlen(key));
		cpy("=", 1);
		sprintf(pos, "%d", val);
		pos += strlen(pos);
		return *this;
	}

	sql_query &equal(const char* key, const char* val) {
		cpy(key, (int)strlen(key));
		cpy("='", 2);
		cpy(val, (int)strlen(val));
		cpy("'", 1);
		return *this;
	}

	sql_query &not_equal(const char* key, int val) {
		cpy(key, (int)strlen(key));
		cpy("!=", 2);
		sprintf(pos, "%d", val);
		pos += strlen(pos);
		return *this;
	}
	sql_query &not_equal(const tbl_fld_link &link, const char* val) {
		cpy(link.tbl, (int)strlen(link.tbl));
		cpy(".", 1);
		cpy(link.fld, (int)strlen(link.fld));
		cpy("!=", 1);
		cpy(val, (int)strlen(val));
		return *this;
	}
	sql_query &not_equal(const tbl_fld_link &link, int val) {
		cpy(link.tbl, (int)strlen(link.tbl));
		cpy(".", 1);
		cpy(link.fld, (int)strlen(link.fld));
		cpy("!=", 2);
		sprintf(pos, "%d", val);
		pos += strlen(pos);
		return *this;
	}
	sql_query &not_equal(const char* key, const char* val) {
		cpy(key, (int)strlen(key));
		cpy("!=", 1);
		cpy(val, (int)strlen(val));
		return *this;
	}
	sql_query& and_() {
		cpy(" and ", 5);
		return *this;
	}
	sql_query& or_() {
		cpy(" or ", 4);
		return *this;
	}
	sql_query &delete_() {
		cpy("delete ", 7);
		return *this;
	}

	template<class ...ELEMENTS>
	sql_query &order_by(ELEMENTS&... eles) {
		cpy("order by ", 9);
		args(eles...);
		pos -= 1;
		return *this;
	}

	sql_query &distinct() {
		cpy("distinct ", 9);
		return *this;
	}

#pragma endregion

#pragma region single_query
private:
	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	moodycamel::ConcurrentQueue<asyn_data_base*> res_queue_;
public:
	bool init(const char* host, const char* name, const char* pwd, const char* schema) {
		if (driver = get_driver_instance()) {
			if (con = driver->connect(host, name, pwd)) {
				con->setSchema(schema);
				if (auto stmt = con->createStatement()) {
					auto fc = stmt->getConnection();
					reset();
					thread t1(&sql_query::start, this, stmt);
					t1.detach();
					return true;
				}
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	void start(sql::Statement *fs) {
		stmt = fs;
		while (1) {
			if (!query(fs))
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	void close() {
		if (con) {
			con->close();
			delete con;
		}
	}

	void release() {
		delete stmt;
		delete con;
	}

	template<class RES>
	void on_result(function<void(RES &re)> func) {
		if (sql::ResultSet *res = stmt->executeQuery(end())) {
			RES resr;
			while (res->next()) {
				resr.unpack(res);
			}
			res->close();
			delete res;
			func(resr);
			reset();
		}
	}

	template<class RES>
	void on_result(function<void(vector<RES> &re)> func) {
		if (sql::ResultSet *res = stmt->executeQuery(end())) {
			vector<RES> vresr;
			vresr.resize(res->rowsCount());
			size_t ri = 0;
			while (res->next()) {
				RES &resr = vresr[ri];
				resr.___unpack(res);
			}
			res->close();
			delete res;
			func(vresr);
			reset();
		}
	}

	template<class RES1, class RES2>
	void on_result(function<void(RES1 &r1, RES2 &r2)> func) {
		if (sql::ResultSet *res = stmt->executeQuery(end())) {
			RES1 r1;
			RES2 r2;
			r1.___unpack(res);
			r2.___unpack(res);
			res->close();
			delete res;
			func(r1, r2);
			reset();
		}
	}

	bool on_update() {
		bool res = stmt->executeUpdate(end());
		reset();
		return res;
	}

	template<class FUN>
	void on_asyn_result(FUN func) {
		if (pre) {
			push_query(new asyn_prepare_data<FUN>(func, prepare_func, end(), &res_queue_));
			pre = false;
		}
		else
			push_query(new asyn_query_data3<FUN>(func, end(), &res_queue_));
		reset();
	}

	void on_asyn_update(function<void(bool result)> func) {
		push_query(new asyn_update_data(func, end(), &res_queue_));
		reset();
	}

	bool upd() {
		asyn_data_base* f;
		if (res_queue_.try_dequeue(f)) {
			f->result();
			delete f;
			return true;
		}
		return false;
	}
#pragma endregion
};

class db_query {
private:
	static map<std::thread::id, sql_query*> threads_map_;
	static vector<sql_query*> querys_;
	moodycamel::ConcurrentQueue<asyn_data_base*> query_queue_;
public:
	static sql_query & query() {
		static uint8_t index = 0;
		static std::mutex mutex;
		sql_query *db = nullptr;
		auto tid = std::this_thread::get_id();
		mutex.lock();
		auto iter = threads_map_.find(tid);
		if (iter != threads_map_.end()) {
			db = iter->second;
		}
		else {
			db = querys_[index%querys_.size()];
			threads_map_[tid] = db;
			index++;
		}
		mutex.unlock();
		return *db;
	}
	static db_query & instance() {
		static db_query db;
		return db;
	}

	~db_query() {
		for (auto &tds : threads_map_)
			delete tds.second;
	}

	bool init(uint8_t threads, const char* host, const char* name, const char* pwd, const char* schema) {
		for (uint8_t i = 0; i < threads; i++) {
			sql_query *query = new sql_query;
			if (!query->init(host, name, pwd, schema))
				return false;
			querys_.push_back(query);
		}
		return true;
	}

	void push_query(asyn_data_base *base) {
		query_queue_.enqueue(base);
	}

	bool do_query(sql::Statement *fs) {
		bool res = true;
		asyn_data_base* base;
		if (query_queue_.try_dequeue(base))
			base->query(fs);
		else
			res = false;
		return res;
	}
};
map<std::thread::id, sql_query*> db_query::threads_map_;
vector<sql_query*> db_query::querys_;

void push_query(asyn_data_base *base) {
	db_query::instance().push_query(base);
}

bool query(sql::Statement *fs) {
	return db_query::instance().do_query(fs);
}