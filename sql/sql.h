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

struct fds {
	typedef function<void(sql::ResultSet *res, size_t index)> ref_type;
	typedef function<void(string &str)> vs_type;
	typedef function <int64_t()> vi_type;
	typedef function <void(sql::PreparedStatement *ps, size_t index)> pv_type;
	ref_type ref;
	vs_type vs;
	vi_type vi;
	pv_type pv;
	char flag;
	void set_flag(char f) {
		flag = flag | f;
	}
	bool check_flag(char f) {
		return flag & f;
	}
};

struct tbl_fld_link {
	const char* tbl;
	const char* fld;
};

template<class T>
class __db_struct_base;

template<class T, class F>
class field_operator
{
private:
	F *fld_ptr = nullptr;
public:
	field_operator(T *obj, F *ptr, const char* name, const std::initializer_list<char> &flag) {
		auto &f = obj->___get_field(name);
		f.ref = [this](sql::ResultSet *res, size_t index) {
			this->get_field_value(res, index);
		};
		f.vs = [this](string& str) {
			this->get_feild_value_str(str);
		};
		f.vi = [this]()-> int64_t {
			return this->get_feild_value_i64();
		};
		for (auto fl : flag) {
			f.set_flag(fl);
		}
	};

	void get_field_value(sql::ResultSet *res, size_t index) {
		get_feild_value(*fld_ptr, res, index);
	}

	void get_feild_value_str(string& str) {
		get_feild_value_str(*fld_ptr, str);
	}

	int64_t get_feild_value_i64() {
		return get_feild_value_i64(*fld_ptr);
	}

	int64_t prepare_value(sql::PreparedStatement *ps, size_t index) {
		return prepare_set(*fld_ptr, ps, index);
	}

protected:
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
protected:
	static void get_feild_value_str(int32_t &val, string& str) {
		str = to_string(val);
	}

	static void get_feild_value_str(int64_t &val, string& str) {
		str = to_string(val);
	}

	static void get_feild_value_str(uint32_t &val, string& str) {
		str = to_string(val);
	}

	static void get_feild_value_str(uint64_t &val, string& str) {
		str = to_string(val);
	}

	static void get_feild_value_str(bool &val, string& str) {
		str = to_string(val);
	}

	static void get_feild_value_str(string &val, string& str) {
		str = "'";
		str += val;
		str += "'";
	}

	static void get_feild_value_str(double &val, string& str) {
		str = to_string(val);
	}

	static void get_feild_value_str(float &val, string& str) {
		str = to_string(val);
	}
protected:
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
	static void prepare_set(int32_t &val, sql::PreparedStatement *ps, size_t index) {
		ps->setInt((uint32_t)index, val);
	}

	static void prepare_set(int64_t &val, sql::PreparedStatement *ps, size_t index) {
		ps->setInt64((uint32_t)index, val);
	}

	static void prepare_set(uint32_t &val, sql::PreparedStatement *ps, size_t index) {
		ps->setUInt((uint32_t)index, val);
	}

	static void prepare_set(uint64_t &val, sql::PreparedStatement *ps, size_t index) {
		ps->setUInt64((uint32_t)index, val);
	}

	static void prepare_set(bool &val, sql::PreparedStatement *ps, size_t index) {
		ps->setBoolean((uint32_t)index, val);
	}

	static void prepare_set(string &val, sql::PreparedStatement *ps, size_t index) {
		ps->setString((uint32_t)index, val.c_str());
	}

	static void prepare_set(double &val, sql::PreparedStatement *ps, size_t index) {
		ps->setDouble((uint32_t)index, val);
	}

	static void prepare_set(float &val, sql::PreparedStatement *ps, size_t index) {
		ps->setDouble((uint32_t)index, val);
	}

	static void prepare_set(__db_struct_base<T> &val, sql::PreparedStatement *ps, size_t index) {
		val.prepare_value(ps, index);
	}
};


#define table(name)\
extern const char name_##name[]=#name;\
class name :public __db_struct_name<name,name_##name>

#define sql_field(type,name,...) \
public:\
	type name;\
    static constexpr tbl_fld_link _##name= {_table_name,#name};\
private:\
	field_operator<___table_type,type> operator_##name{this,&name,#name,{__VA_ARGS__}};

template<class T>
class __db_struct_base {
public:
	typedef T ___table_type;
	const char* _tbn;
public:
	__db_struct_base() {
		//_tbn = _table_name;
	}
	~__db_struct_base() {
	}
	bool unpack(sql::ResultSet *res) {
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
	bool unpack(sql::ResultSet *res, int64_t &key) {
		if (res) {
			if (res->getMetaData()->getColumnCount() > 0 && res->getMetaData()->getTableName(1).compare(_tbn) == 0) {
				for (unsigned int i = 1; i <= res->getMetaData()->getColumnCount(); i++) {
					auto iter = fields_.find(res->getMetaData()->getColumnName(i).c_str());
					if (iter != fields_.end()) {
						iter->second.ref(res, i);
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
	bool unpack(sql::ResultSet *res, string &key) {
		if (res) {
			if (res->getMetaData()->getColumnCount() > 0 && res->getMetaData()->getTableName(1).compare(_tbn) == 0) {
				for (unsigned int i = 1; i <= res->getMetaData()->getColumnCount(); i++) {
					auto iter = fields_.find(res->getMetaData()->getColumnName(i).c_str());
					if (iter != fields_.end()) {
						iter->second.ref(res, i);
						if (iter->second.check_flag(primary_key)) {
							iter->second.vs(key);
						}
					}
				}
				return true;
			}
		}
		return false;
	}
	fds& ___get_field(string s) {
		return fields_[s];;
	}
	bool ___do_ref(const char* ele, sql::ResultSet *res, size_t index) {
		auto iter = fields_.find(ele);
		if (iter != fields_.end()) {
			iter->second.ref(res, index);
			return true;
		}
		return false;
	}
	void ___get_set_str(string& set) {
		string v_str;
		for (auto &field : fields_) {
			if (!field.second.check_flag(auto_increment)) {
				set += field.first;
				set += '=';
				field.vs(v_str);
				set += v_str;
				set += ',';
			}
		}
		set.pop_back();
	}
	void ___get_value_str(string& set) {
		string v_str;
		set += "(";
		for (auto &field : fields_) {
			if (!field.second.check_flag(auto_increment)) {
				set += field.first;
				set += ',';
			}
		}
		set.pop_back();
		set += ") values(";
		for (auto &field : fields_) {
			if (!field.second.check_flag(auto_increment)) {
				field.vs(v_str);
				set += v_str;
				set += ',';
			}
		}
		set.pop_back();
		set += ")";
	}
	void ___get_prepare_str(string& set) {
		string v_str;
		set += "(";
		for (auto &field : fields_) {
			if (!field.second.check_flag(auto_increment)) {
				set += field.first;
				set += ',';
			}
		}
		set.pop_back();
		set += ") values(";
		for (auto &field : fields_) {
			if (!field.second.check_flag(auto_increment)) {
				field.vs(v_str);
				set += '?,';
			}
		}
		set.pop_back();
		set += ")";
	}
	void prepare_value(sql::PreparedStatement *ps, size_t index) {
		for (auto &field : fields_) {
			if (!field.second.check_flag(auto_increment)) {
				field.second.pv(ps, index++);
			}
		}
	}
protected:
	map<string, fds> fields_;
};

template<class T, const char* nae>
class __db_struct_name:public __db_struct_base<T> {
public:
	static constexpr const char* _table_name = nae;
	__db_struct_name(){
		__db_struct_base<T>::_tbn = nae;
	}
};

///-------------------------somthing about sql query------------------


///≤Œ ˝›Õ»° https://zhuanlan.zhihu.com/p/102240099
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
		ele.unpack(query_set);
		return false;
	}

	template<class T>
	static bool unpack(vector<T> &ele, sql::ResultSet *query_set) {
		T val;
		val.unpack(query_set);
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
	template<class ELEMENT, class ...ELEMENTS>
	static void args(sql::PreparedStatement* pre, size_t index, ELEMENT& ele, ELEMENTS&... eles) {
		field_operator<ELEMENT, ELEMENT>::prepare_set(ele, pre, index);
		args(eles..., ++index);
	}
	template<class ELEMENT>
	static void args(sql::PreparedStatement* pre, size_t index, ELEMENT& ele) {
		field_operator<ELEMENT, ELEMENT>::prepare_set(ele, pre, index);
	}
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
	}
	void result() {
		func(r);
	}
};

template<class V,class F, class P = detail::__function_traits<std::decay_t<F>>>
class asyn_prepare_data :public asyn_data_base
{
private:
	function<void(bool result)> func;
	bool r;
public:
	~asyn_prepare_data() {}
	asyn_prepare_data(const function<void(bool result)> &f, const char* s, moodycamel::ConcurrentQueue<asyn_data_base*> *queue) {
		func = f;
		sql = s;
		res_queue_ = queue;
	}

	void query(sql::Statement *stmt) {
		r = stmt->executeUpdate(sql.c_str());
	}
	void result() {
		func(r);
	}
};

template<class F, class P = detail::__function_traits<std::decay_t<F>>>
class asyn_query_data3 :public asyn_data_base
{
private:
	typename P::function_type func;
	typename P::no_reference_types eles;
public:
	~asyn_query_data3() {}
	asyn_query_data3(const F &f, const char* s, moodycamel::ConcurrentQueue<asyn_data_base*> *queue) {
		func = f;
		sql = s;
		res_queue_ = queue;
	}
	void query(sql::Statement *stmt) {
		sql::ResultSet *res = stmt->executeQuery(sql.c_str());
		while (res->next()) {
			if (!multi_args_and_unpack::multi_args<typename P::no_reference_types>::arg(eles, res))
				break;
		}
		res->close();
		delete res;
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
		itoa(val, pos, 10);
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
		cpy("update ", 7);
		cpy(T::_table_name, (int)strlen(T::_table_name));
		cpy(" set ", 5);
		string str;
		val.___get_set_str(str);
		cpy(str.c_str(), (int)str.length());
		return *this;
	}
	sql_query & insert(const char* fld) {
		cpy("insert into ", 12);
		cpy(fld, (int)strlen(fld));
		return *this;
	}
	template<class T>
	sql_query & insert(T &val) {
		cpy("insert into ", 12);
		cpy(T::_table_name, strlen(T::_table_name));
		string str;
		val.___get_value_str(str);
		cpy(str.c_str(), str.length());
		return *this;
	}
	template<class ...ELEMENTS>
	sql_query &values(ELEMENTS... eles) {
		cpy(" values(", 8);
		args(eles...);
		pos -= 1;
		cpy(")", 1);
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
		itoa(val, pos, 10);
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
		itoa(val, pos, 10);
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
		itoa(val, pos, 10);
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
		itoa(val, pos, 10);
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
				resr.unpack(res);
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
			r1.unpack(res);
			r2.unpack(res);
			res->close();
			delete res;
			func(r1, r2);
			reset();
		}
	}

	template<class ...ELEMENTS>
	sql_query &prepare_values(ELEMENTS... eles) {
		if (auto *prepare = con->prepareStatement(end())) {
			multi_args_and_prepare::args(prepare, 0, eles...);
		}
		
		//auto *prepare = con->prepareStatement(end());
		//prepare->set
		//cpy(" values(", 8);
		//args(eles...);
		//pos -= 1;
		//cpy(")", 1);
		return *this;
	}

	bool on_update() {
		bool res = stmt->executeUpdate(end());
		reset();
		return res;
	}

	template<class FUN>
	void on_asyn_result(FUN func) {
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