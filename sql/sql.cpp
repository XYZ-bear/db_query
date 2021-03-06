// lua_binder.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "sql.h"

using namespace std;


void perf_test(string name, size_t t, function<void()> f) {
	clock_t startTime, endTime;
	startTime = clock();
	for (size_t i = 0; i < t; i++)
		f();
	endTime = clock();
	cout << name << ":" << endTime - startTime << "ms" << endl;
}

#include "./fast_protobuf/fast_protobuf.h"

MESSAGE(Test) {
public:
	enum(ttten, Hello = -100, Bitch = 100);
	ele(string, str, 1);
	ele(int64_t, int64, 2);
	ele(int32_t, int32, 3);
	ele(uint32_t, uint32, 4);
	ele(int32_t, sint32, 5);
	ele(int64_t, sint64, 6);
	ele(double, doub, 9);
	ele(float, flo, 10);
	ele(bool, boo, 11);
	ele(map<int, int>, mapp, 12);
	ele(vector<int32_t>, vec, 13);
	ele(uint64_t, uint64, 15);
	ele(fixed32, fi32, 16);
	ele(fixed64, fi64, 17);
	ele(enumm<ttten>, ttte, 18);
};

table(city)
{
	sql_field(int, ID, primary_key, auto_increment);
	sql_field(string, Population);
	sql_field(string, Name);
	sql_field(Test, Data);
};

table(country)
{
	sql_field(string, Code);
};
//
//table(home)
//{
//	sql_field(int, idhome, auto_increment);
//	sql_field(string, address);
//	sql_field(string, homecol);
//	sql_field(double, rd);
//};


void ThreadProc1() {
	int count = 1;
	clock_t startTime;
	startTime = clock();
	auto &db = db_query::query();
	//for (int i = 0; i < 100000; i++) {
	//	db.select()
	//		.from(city::_NAME)
	//		.on_asyn_result(
	//			[&startTime, &count](vector<city> &res)->void
	//	{
	//		//cout << res.size() << endl;
	//		if (count == 100) {
	//			cout << clock() - startTime << endl;
	//		}
	//		count++;
	//	}
	//	);
	//	
	//	db.select(city::_ID, country::_Code, city::_Name, city::_Population)
	//		.from(city::_NAME, country::_NAME)
	//		.where()
	//		.equal(city::_ID, 1)
	//		.and_()
	//		.equal(country::_Code, "ABW")
	//		.on_asyn_result(
	//			[&startTime, &count](vector<city> &res, vector<country> &c2)->void
	//	{
	//		if (count == 100000) {
	//			cout << clock() - startTime << endl;
	//		}
	//		count++;
	//		//cout << res.ID << endl;
	//		//cout << res.Name << endl;
	//		//cout << res.Population << endl;
	//		//cout << c2.Code << endl;
	//	}
	//	);
	//};

	while (1) {
		if (!db.upd())
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void ThreadProc2() {
	int count = 1;
	clock_t startTime;
	startTime = clock();
	auto &db = db_query::query();
	//for (int i = 0; i < 100000; i++) {
	//	//db_query::query().select()
	//	//	.from(city::_NAME)
	//	//	.on_asyn_result<city>(
	//	//		[&startTime, &count](vector<city> &res)->void
	//	//{
	//	//	if (count == 100) {
	//	//		cout << clock() - startTime << endl;
	//	//	}
	//	//	count++;
	//	//}
	//	//);
	//	db.select(city::_ID, country::_Code, city::_Name, city::_Population)
	//		.from(city::_NAME, country::_NAME)
	//		.where()
	//		.equal(city::_ID, 1)
	//		.and_()
	//		.equal(country::_Code, "AFG")
	//		.on_asyn_result(
	//			[&startTime, &count](vector<city> &res, vector<country> &c2)->void
	//	{
	//		if (count == 100000) {
	//			cout << clock() - startTime << endl;
	//		}
	//		count++;
	//		//cout << res.ID << endl;
	//		//cout << res.Name << endl;
	//		//cout << res.Population << endl;
	//		//cout << c2.Code << endl;
	//	}
	//	);
	//};

	while (1) {
		if (!db.upd())
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


void thread_test(size_t t) {
	//auto driver = get_driver_instance();
	//auto con = driver->connect("tcp://127.0.0.1:3306", "root", "123456");

	//auto driver1 = get_driver_instance();
	//auto con1 = driver->connect("tcp://127.0.0.1:3306", "root", "123456");
	//thread t11([]()->void {
	//	for (int i = 0; i < 100; i++) {
	//		auto driver = get_driver_instance();
	//		//auto con = driver->connect("tcp://127.0.0.1:3306", "root", "123456");
	//	}
	//});

	//thread t12([]()->void {
	//	for (int i = 0; i < 100; i++) {
	//		auto driver = get_driver_instance();
	//		//auto con = driver->connect("tcp://127.0.0.1:3306", "root", "123456");
	//	}
	//});

	//t11.detach();
	//t12.detach();
	//perf_test("table build 1", 1, []()->void {
	//	for (int i = 0; i < 1000; i++)
	//		std::this_thread::sleep_for(std::chrono::microseconds(1));
	//});
	//perf_test("table build 1", 1, []()->void {
	//	for (int i = 0; i < 1000; i++)
	//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	//});
	//getchar();
	thread t1(ThreadProc1);
	thread t2(ThreadProc2);
	thread t3(ThreadProc1);
	thread t4(ThreadProc1);

	t1.detach();
	t2.detach();
	t3.detach();
	t4.detach();
	getchar();
}

class aa1 {
public:
	~aa1() {
		cout << 1 << endl;
	}
	virtual void a() = 0;
};

class aa2:public aa1 {
public:
	~aa2() {
		cout << 1 << endl;
	}
	int af = 1;
	void a() {
		af++;
	};
};

class sfdf {
public:
	template<int Len,int Len2>
	void sfsd(char const(&s)[Len], char const(&s1)[Len2]){
		const char kk[Len + Len2]="1234";
		cout << kk << endl;
	}
};

#define STRCAT(STRING_L, STRING_R) STRING_L STRING_R;


namespace multi_args_and_unpack2 {
	template<typename Tuple, int N = std::tuple_size<Tuple>::value>
	struct multi_args
	{
		static void arg() {
			multi_args<Tuple, N - 1>::arg();
			using type = typename std::tuple_element<N - 1, Tuple>::type;
			cout << type::_NAME << endl;
		}
	};

	template<typename Tuple>
	struct multi_args<Tuple, 1>
	{
		static void arg() {
			using type = typename std::tuple_element<0, Tuple>::type;
			cout << type::_NAME << endl;
		}
	};
};

template<class ...Args>
void insert() {
	using Tuple = std::tuple<Args...>;
	multi_args_and_unpack2::multi_args<Tuple>::arg();
	//template <std::size_t N>
	//using Nth = typename std::tuple_element<N, Tuple>::type;
	//using First = Nth<0>;
	//using Last = Nth<Size - 1>;
}

#define afffg(...) flagg<__VA_ARGS__>();


//template<char C1 = 0, char C2 = 0, char C3 = 0, char C4 = 0, char C5 = 0, char C6 = 0, char C7 = 0, char C8 = 0>
//constexpr char flagg() {
//	return 0 | C1 | C2 | C3 | C4 | C5 | C6 | C7 | C8;
//}

class ba {
public:
	virtual void ref() = 0;
	virtual void pv() = 0;
};
template<class T>
class field {
public:
	T value;
	template<class O>
	field(O *o,const char* name) {
	}
	void ref() {}
	void pv() {}
};

constexpr int akkk = 10;


class ccc {
public:
	template<class T>
	ccc(T *obj, const char* vv) {
		obj->sf.add(vv, 1);
	}
};

template<class T>
class single {
public:
	static T sf;
};
template<class T>
T single<T>::sf;

template<class T>
class base {
public:
	static map<const char*, int> mm;
	void add(const char* c, int i) {
		mm.find(c);
		//mm.insert({ c,i });
		mm[c] = i;
	}
};
template<class T>
map<const char*, int> base<T>::mm;

class ff :public base<ff>, public single<ff> {
public:
	//static constexpr tbl_fld_link a = { "a","b" };
	static constexpr int b = 1;
	ff() {
		mm;
	}
	~ff() {}
private:
	ccc fcc{ this,"a" };
};

constexpr array<int, 10> vec{};

class C {
public:
	constexpr C(int a) {
		vec;
	}
};

template<int N>
struct mul
{
	static void arg() {
	}
};

template <>
struct mul<1>
{
	static void arg() {
		cout << 1 << endl;
	}
};

template <>
struct mul<2>
{
	static void arg() {
		cout << 2 << endl;
	}
};

#include <fstream>
#include <streambuf>
#include <istream>


template <typename char_type>
struct ostreambuf : std::basic_streambuf<char_type, std::char_traits<char_type>>
{
	ostreambuf(char_type* buffer, std::streamsize bufferLength) {
		// set the "put" pointer the start of the buffer and record it's length.
		setp(buffer, buffer + bufferLength);
	}
};
#include <sstream>
int main()
{
	////perf_test("table build 1", 10, []()->void {
	////	cin.clear();
	////	for (int i = 0; i < 10; i++)
	////		cin >> i;
	////});
	//char mybuffer[512];
	//std::fstream filestr;
	//filestr.rdbuf()->pubsetbuf(mybuffer, 512);
	char data[512] = { "3234343" };
	std::stringstream blob_input_stream;
	char data2[512];
	blob_input_stream.write(data, 512);
	blob_input_stream.read(data2, 512);
	//out.write("abcdef", 3);
	////filebuf f;
	////f.pubsetbuf(iio, 100);
	////istream a(&f);
	////sql::PreparedStatement *sk;
	////sk->setBlob(1, &filestr);
	//int akk = 0;
	//int64_t fkkk = 0;
	//akk = fkkk;


	//mul<2>::arg();
	////cout << A<name_city>::name << endl;
	////A a;
	////tbl_fld_link3 f{"12","34",2};
	////
	////std::tuple<decltype(t2)::_This_type&, field<string>&> t3 = std::tuple_cat(t2, std::tie(a3));
	//
	////perf_test("table build 1", 40000000, []()->void {
	////	ssf df;
	////});
	////perf_test("table build 1", 40000000, []()->void {
	////	city df;
	////});
	//unsigned char data[100];
	//Test df;
	//df.int64 = 32;
	//df.str = "hello";
	//Test kf = df;
	//const proto_base<Test> &fdf = df;
	//proto_base<Test> &fdf2 = const_cast<proto_base<Test>&>(fdf);
	//fdf2.serialize(data, 100);

	//Test df2;
	//df2.unserialize(data, 100);


	//perf_test("table build 1", 10000000, []()->void {
	//	//auto &sf = Test::fields;
	//	Test df;
	//});
	//getchar();
	//city c;
	//cout << city::__insert_str << endl;
	//cout << city::__update_str << endl;
	//unordered_map<const char*, int> sff{ {"123",1},{"12",2} };
	//cout << sff["12"] << endl;
	//cout << sff["123"] << endl;

	//getchar();
	//std::tuple
	//city::_kID;
	//fjfj flll;
	//(flll.*flll.d)();
	//getchar();
	//detail::_function_traits<R(*)(As...)> : public __function_traits_base<R, As...> {};
	
	//auto hf = [](string const& key)->size_t { return key[0]; };

	//map<string const, int, decltype(hf)> m(1, hf);
	//opppp<int, int>(1,2);
	//auto fnc = []()->void {
	//	cout << "1" << endl;

	//struct pushi
	//{
	//	string str;
	//	function<void()> func;
	//	size_t id;
	//};
	//perf_test("table build 1", 100000000, [&fnc]()->void {
	//	function<void()> hh = fnc;
	//	function<void()> hh2 = [hh]() {};
	//	function<void()> hh3 = hh2;
	//	function<void()> hh4= [hh]() {hh(); };
	//});
	//perf_test("table build 1", 100000000, [&fnc]()->void {
	//		asyn_data_base *inf = new asyn_update_data<void()>(fnc, "fds");
	//		delete inf;
	//});
	//getchar();
	//getchar();
	//auto *fnc2 = new function<void()>(fnc);
	//(*fnc2)();
	//quu.push(1);
	//quu.pop();
	//quu.print();
	//quu.push(2);
	//quu.print();
	///uthread_test(1);
	//unlock_queue<int> quu;
	//quu.push(1);
	//quu.push(2);
	//quu.push(3);
	//quu.pop();
	//quu.print();
	if (!db_query::instance().init(8, "tcp://81.68.106.186:3306", "root", "!QAZ2wsx3edc", "world"))
		return 1;
	//db_query::query().select()
	//	.from(city::_NAME)
	//	.on_asyn_result<city, country>(
	//		[](db_set &set)->void
	//{
	//	auto &s = set.get_base<city>();
	//	for (auto i : s) {
	//		city *fs = (city*)i;
	//		std::cout << fs->Name << endl;
	//	}
	//}
	//);
	city dc;
	dc.ID = 4;
	dc.Name = "ck2";
	dc.Population = "nice";
	dc.Data.int64 = 12;
	db_query::query().update(dc).on_asyn_result(
		[](bool res)->void
	{
		cout << res << endl;
		//cout << res.size() << endl;
	});
	//perf_test("table build 1", 100, []()->void {
	//	city cii;
	//	cii.Population = "very";
	//	cii.Name = "china";
	//	db_query::query().insert("city(Population,Name) values(?,?)").prepare_values(cii.Population, cii.Name).on_asyn_result(
	//		[](vector<city> &res)->void
	//	{
	//		//cout << res.size() << endl;
	//	});
	//	//db_query::query().select("insert into city(Population,Name,Data) values('yes','china','\0123')")

	//});
	db_query::query().select()
		.from(city::_NAME).
		where().
		equal("ID",4)
		.on_asyn_result(
			[](vector<city> &res)->void
	{
		for (auto &c : res)
			cout << c.ID << " " << c.Name << " " << c.Population << " " << c.Data.int64 << endl;
		//cout << res.size() << endl;
	}
	);
	//db_query::query().select()
	//	.from(city::_NAME)
	//	.on_asyn_result(
	//		[](vector<city> &res)->void
	//{
	//	//cout << res.Name << endl;
	//	cout << res.size() << endl;
	//}
	//);

	//db_query::query().select()
	//	.from(city::_NAME)
	//	.on_asyn_result(
	//		[](map<string, city> &res)->void
	//{
	//	//cout << res.Name << endl;
	//	cout << res.size() << endl;
	//}
	//);

	//db_query::query().select()
	//	.from(city::_NAME)
	//	.on_asyn_result(
	//		[](vector<res_set<city,country>> &set)->void
	//{
	//	cout << set.size() << endl;
	//	cout << set[0].get<city>().Name << endl;
	//	//cout << res.Name << endl;
	//	//cout << res.size() << endl;
	//}
	//);

	////db_query::query().select()
	////	.from(city::_NAME)
	////	.on_asyn_result(
	////		[](map<int64_t, res_set<city, country>> &set)->void
	////{
	////	cout << set.size() << endl;
	//////	cout << set[0].get<city>().Name << endl;
	////	//cout << res.Name << endl;
	////	//cout << res.size() << endl;
	////}
	////);
	//
	//db_query::query().select(city::_ID, country::_Code, city::_Name, city::_Population)
	//	.from(city::_NAME, country::_NAME)
	//	.where()
	//	.equal(city::_ID, 1)
	//	.and_()
	//	.equal(country::_Code, "AFG")
	//	.on_asyn_result(
	//		[](vector<city> &res, vector<country> &c2)->void
	//{
	//	cout << res.size() << endl;
	//	cout << c2.size() << endl;
	//	cout << res[0].ID << endl;
	//	cout << res[0].Name << endl;
	//	cout << res[0].Population << endl;
	//	cout << c2[0].Code << endl;
	//}
	//);
	while (1) {
		if (!db_query::query().upd())
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	thread_test(1);
	//aa1 *ff = new aa2();
	//perf_test("table build 1", 1000000000, [&ff]()->void {
	//	ff->a();
	//});
	//aa2 aaaf;
	//auto aff = [&aaaf]()->void {
	//	aaaf.a();
	//};
	//perf_test("table build 1", 1000000000, [&aff]()->void {
	//	aff();
	//});
	//getchar();
	//home hh;
	//hh.idhome = 2;
	//hh.address = "shanghai";
	//cout << db2::instance().update(hh).where().equal(home::__idhome, 2).end();
	//sss s;
	//thread t1(&sss::start,s);
	//t1.detach();
	//getchar();

	//thread_test(1);
	//perf_test("table build", 10000000, [&hh]()->void {
	//	db2::instance().insert(hh);
	//});
	//cout << db2::instance().insert(hh).end() << endl;
	//getchar();
	//cout << aa << endl;
	//constexpr const char* s = blog::str("edf");
	//cout << s << endl;
	//cout << s.Hash() << endl;

	//db2::instance().insert(hh).on_asyn_update2([](bool res)->void {
	//	cout << 0;
	//});
	//perf_test("table build 1", 100, [&hh]()->void {
	//	db2::instance().insert(hh).on_asyn_update2([](bool res)->void {
	//		cout << 0;
	//	});
	//});
	//perf_test("table build 1", 100, [&hh]()->void {
	//	//db2::instance().update(hh).where().equal(home::__idhome, 2).on_asyn_update([](bool res)->void {
	//	//	cout << 0;
	//	//});
		db_query::query().select(city::_ID, country::_Code, city::_Name, city::_Population)
			.from(city::_NAME, country::_NAME)
			.where()
			.equal(city::_ID, 1)
			.and_()
			.equal(country::_Code, "AFG")
			.on_asyn_result(
				[](vector<city> &res, vector<country> &c2)->void
		{
			//cout << res.ID << endl;
			//cout << res.Name << endl;
			//cout << res.Population << endl;
			//cout << c2.Code << endl;
		}
		);
	////});
	//perf_test("table build 2", 1000, [&hh]()->void {
	//	//cout << db_query::query().update(hh).where().equal(home::__idhome, 2).on_update();
	//	//cout << 1;
	//});
	//perf_test("table build 2", 1000, [&hh]()->void {
	//	//db_query::query().update(hh).where().equal(home::_idhome, 2).on_asyn_update([](bool res)->void {
	//	//	cout << res;
	//	//}
	//});

	//perf_test("table build 1", 1, [&hh]()->void {
		//db2::instance().select(city::_ID(), country::_Code(), city::_Name(), city::_Population())
		//	.from(city::_NAME, country::_NAME)
		//	.where()
		//	.equal(city::_ID().c_str(), 1)
		//	.and_()
		//	.equal(country::_Code().c_str(), "AFG")
		//	.on_asyn_result<city, country>(
		//		[](city &res, country &c2)->void
		//{
		//	cout << res.ID << endl;
		//	cout << res.Name << endl;
		//	cout << res.Population << endl;
		//	cout << c2.Code << endl;
		//}
		//);
	//	db2::instance().update(hh).where().equal(home::__idhome, 2).on_asyn_update([](bool res)->void {});
	//});

	
	getchar();

	db_query::query().select()
		.from(city::_NAME)
		.on_result<city>(
			[](vector<city> &res)->void
	{
		for (int i = 0; i < 10; i++) {
			cout << res[i].ID << endl;
			cout << res[i].Name << endl;
			cout << res[i].Population << endl;
		}
		cout << res.size() << endl;
	}
	);

	//cout << string(city::__ID()) << endl;
	//perf_test("table build", 1, []()->void {
	//	db_query::query().select(city::_ID, country::_Code, city::_Name, city::_Population)
	//		.from(city::_NAME, country::_NAME)
	//		.where()
	//		.equal(city::_ID, 1)
	//		.and_()
	//		.equal(country::_Code, "ABW")
	//		.on_result<city, country>(
	//			[](city &res, country &c2)->void
	//	{
	//		cout << res.ID << endl;
	//		cout << res.Name << endl;
	//		cout << res.Population << endl;
	//		cout << c2.Code << endl;
	//	}
	//	);

	//	db_query::query().select(city::_ID, city::_Name, city::_Population)
	//		.from(city::_NAME)
	//		.on_result<city>(
	//			[](vector<city> &res)->void
	//	{
	//		for (int i = 0; i < 10; i++) {
	//				cout << res[i].ID << endl;
	//				cout << res[i].Name << endl;
	//				cout << res[i].Population << endl;
	//		}
	//		cout << res.size() << endl;
	//	}
	//	);
	//});

	//db2::instance().select(__(1, 2))
	//	.from("tbl_role")
	//	.where()
	//	.equal("id", 1)
	//	.and_()
	//	.equal("role_name", "xyz")
	//	.or_()
	//	.not_equal("role_id", 1)
	//	.end();

	getchar();
}