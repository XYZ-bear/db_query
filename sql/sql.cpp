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

table(city)
{
	sql_field(int, ID, primary_key);
	sql_field(string, Population);
	sql_field(string, Name);
};

table(country)
{
	sql_field(string, Code);
};

table(home)
{
	sql_field(int, idhome, auto_increment);
	sql_field(string, address);
	sql_field(string, homecol);
};


void ThreadProc1() {
	int count = 1;
	clock_t startTime;
	startTime = clock();
	auto &db = db_query::query();
	for (int i = 0; i < 100000; i++) {
		db.select()
			.from(city::_table_name)
			.on_asyn_result(
				[&startTime, &count](vector<city> &res)->void
		{
			//cout << res.size() << endl;
			if (count == 100) {
				cout << clock() - startTime << endl;
			}
			count++;
		}
		);
		
		db.select(city::_ID, country::_Code, city::_Name, city::_Population)
			.from(city::_table_name, country::_table_name)
			.where()
			.equal(city::_ID, 1)
			.and_()
			.equal(country::_Code, "ABW")
			.on_asyn_result(
				[&startTime, &count](vector<city> &res, vector<country> &c2)->void
		{
			if (count == 100000) {
				cout << clock() - startTime << endl;
			}
			count++;
			//cout << res.ID << endl;
			//cout << res.Name << endl;
			//cout << res.Population << endl;
			//cout << c2.Code << endl;
		}
		);
	};

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
	for (int i = 0; i < 100000; i++) {
		//db_query::query().select()
		//	.from(city::_table_name)
		//	.on_asyn_result<city>(
		//		[&startTime, &count](vector<city> &res)->void
		//{
		//	if (count == 100) {
		//		cout << clock() - startTime << endl;
		//	}
		//	count++;
		//}
		//);
		db.select(city::_ID(), country::_Code(), city::_Name(), city::_Population())
			.from(city::_table_name, country::_table_name)
			.where()
			.equal(city::_ID().c_str(), 1)
			.and_()
			.equal(country::_Code().c_str(), "AFG")
			.on_asyn_result(
				[&startTime, &count](vector<city> &res, vector<country> &c2)->void
		{
			if (count == 100000) {
				cout << clock() - startTime << endl;
			}
			count++;
			//cout << res.ID << endl;
			//cout << res.Name << endl;
			//cout << res.Population << endl;
			//cout << c2.Code << endl;
		}
		);
	};

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


int main()
{
	res_set<city, country> settt;
	settt.get<city>();
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
	if (!db_query::instance().init(8, "tcp://127.0.0.1:3306", "root", "123456", "world"))
		return 1;
	//db_query::query().select()
	//	.from(city::_table_name)
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

	db_query::query().select()
		.from(city::_table_name)
		.on_asyn_result(
			[](vector<city> &res)->void
	{
		cout << res.size() << endl;
	}
	);

	db_query::query().select()
		.from(city::_table_name)
		.on_asyn_result(
			[](vector<city> &res)->void
	{
		//cout << res.Name << endl;
		cout << res.size() << endl;
	}
	);

	db_query::query().select()
		.from(city::_table_name)
		.on_asyn_result(
			[](map<string, city> &res)->void
	{
		//cout << res.Name << endl;
		cout << res.size() << endl;
	}
	);

	db_query::query().select()
		.from(city::_table_name)
		.on_asyn_result(
			[](vector<res_set<city,country>> &set)->void
	{
		cout << set.size() << endl;
		cout << set[0].get<city>().Name << endl;
		//cout << res.Name << endl;
		//cout << res.size() << endl;
	}
	);

	//db_query::query().select()
	//	.from(city::_table_name)
	//	.on_asyn_result(
	//		[](map<int64_t, res_set<city, country>> &set)->void
	//{
	//	cout << set.size() << endl;
	////	cout << set[0].get<city>().Name << endl;
	//	//cout << res.Name << endl;
	//	//cout << res.size() << endl;
	//}
	//);
	
	db_query::query().select(city::_ID, country::_Code, city::_Name, city::_Population)
		.from(city::_table_name, country::_table_name)
		.where()
		.equal(city::_ID().c_str(), 1)
		.and_()
		.equal(country::_Code().c_str(), "AFG")
		.on_asyn_result(
			[](vector<city> &res, vector<country> &c2)->void
	{
		cout << res.size() << endl;
		cout << c2.size() << endl;
		cout << res[0].ID << endl;
		cout << res[0].Name << endl;
		cout << res[0].Population << endl;
		cout << c2[0].Code << endl;
	}
	);
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
	home hh;
	hh.idhome = 2;
	hh.address = "shanghai";
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
	perf_test("table build 1", 100, [&hh]()->void {
		//db2::instance().update(hh).where().equal(home::__idhome, 2).on_asyn_update([](bool res)->void {
		//	cout << 0;
		//});
		db_query::query().select(city::_ID, country::_Code, city::_Name, city::_Population)
			.from(city::_table_name, country::_table_name)
			.where()
			.equal(city::_ID().c_str(), 1)
			.and_()
			.equal(country::_Code().c_str(), "AFG")
			.on_asyn_result(
				[](vector<city> &res, vector<country> &c2)->void
		{
			//cout << res.ID << endl;
			//cout << res.Name << endl;
			//cout << res.Population << endl;
			//cout << c2.Code << endl;
		}
		);
	});
	perf_test("table build 2", 1000, [&hh]()->void {
		//cout << db_query::query().update(hh).where().equal(home::__idhome, 2).on_update();
		//cout << 1;
	});
	perf_test("table build 2", 1000, [&hh]()->void {
		db_query::query().update(hh).where().equal(home::__idhome, 2).on_asyn_update([](bool res)->void {
			cout << res;
		});
	});

	//perf_test("table build 1", 1, [&hh]()->void {
		//db2::instance().select(city::_ID(), country::_Code(), city::_Name(), city::_Population())
		//	.from(city::_table_name, country::_table_name)
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
		.from(city::_table_name)
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
	perf_test("table build", 1, []()->void {
		db_query::query().select(city::_ID(), country::_Code(), city::_Name(), city::_Population())
			.from(city::_table_name, country::_table_name)
			.where()
			.equal(city::_ID().c_str(), 1)
			.and_()
			.equal(country::_Code().c_str(), "ABW")
			.on_result<city, country>(
				[](city &res, country &c2)->void
		{
			cout << res.ID << endl;
			cout << res.Name << endl;
			cout << res.Population << endl;
			cout << c2.Code << endl;
		}
		);

		db_query::query().select(city::_ID(), city::_Name(), city::_Population())
			.from(city::_table_name)
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
	});

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