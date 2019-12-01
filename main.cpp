#include "share.h"

#include <iostream>
#include <map>
#include <vector>

#include "reserv_allocator.h"
#include "direct_collector.h"

using std::cout;

int main() {
	struct hard {
		int fa;
		int fi;
		hard(int fa, int fi) : fa(fa), fi(fi) {};
		hard(const hard&) = delete;
		hard(hard&&) noexcept = delete;
	};

	/*
• создание экземпляра std::map<int, hard>
• заполнение 10 элементами, где ключ это число от 0 до 9, а значение - соответствующие значению ключа
факториал и число Фибоначчи
• создание экземпляра std::map<int, hard> с новым аллокатором ограниченным 10 элементами
• заполнение 10 элементами, где ключ это число от 0 до 9, а значение - аналогичные первому контейнеру
• вывод на экран всех значений (ключ и значения разделены пробелами) хранящихся в контейнере
• создание экземпляра своего контейнера для хранения hard
• заполнение 10 значениями аналогичными первому контейнеру
• создание экземпляра своего контейнера для хранения hard с новым аллокатором ограниченным 10
элементами
• заполнение 10 значениями аналогичными первому контейнеру
• вывод на экран всех значений хранящихся в контейнере
	*/

	/*создание экземпляра std::map<int, hard>*/
	std::map<int, hard> map;

	//// helpers (using map)
	auto fn_fact = [&map](int i) {
		return i * map.at(i - 1).fa;
	};
	auto fn_fib = [&map](int i) {
		return map.at(i - 2).fi + map.at(i - 1).fi;
	};
	//// end helpers (using map)

	/* заполнение 10 элементами, где ключ это число от 0 до 9, а значение - соответствующие значению ключа
	факториал и число Фибоначчи */
	auto fn_add_to_map = [](auto& map, auto&& key, auto&& fa, auto&& fi) {
		map.emplace(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(fa, fi));
	};
	auto fn_map_fill = [&fn_add_to_map, &fn_fact, &fn_fib](auto& map) {
		using std::forward;
		fn_add_to_map(map, 0, 1, 0);
		fn_add_to_map(map, 1, 1, 1);
		/*map.emplace(0, hard{ 1, 0 });
		map.emplace(1, hard{ 1, 1 });*/
		for (int i = 2; i < 10; i++) {
			int fa = fn_fact(i);
			int fi = fn_fib(i);
			/*map.emplace(i, fa, fi);*/
			fn_add_to_map(map, i, fa, fi);
		}
	};
	fn_map_fill(map);

	/* создание экземпляра std::map<int, hard> с новым аллокатором ограниченным 10 элементами */
	std::map<int, hard, std::less<int>, reserv_allocator<std::pair<const int, hard>, 10>> map2;

	/* заполнение 10 элементами, где ключ это число от 0 до 9, а значение - аналогичные первому контейнеру */
	fn_map_fill(map2);

	/* вывод на экран всех значений (ключ и значения разделены пробелами) хранящихся в контейнере */
	auto fn_print_map = [](auto& map) {
		for (auto& pair : map) {
			cout << pair.first << " " << pair.second.fa << " " << pair.second.fi << endl;
		}
	};
	/*cout << "=== map (with std::allocator): ===\n";
	fn_print_map(map);*/
	cout << "=== map (with reserv_allocator): ===\n";
	fn_print_map(map2);

	/* создание экземпляра своего контейнера для хранения hard */
	direct_collector<hard> dcollector;

	/* заполнение 10 значениями аналогичными первому контейнеру */
	auto fn_collector_fill = [&fn_fact, &fn_fib](auto& collector) {
		collector.emplace(1, 0);
		collector.emplace(1, 1);
		for (int i = 2; i < 10; i++) {
			collector.emplace(fn_fact(i), fn_fib(i));
		}
	};
	fn_collector_fill(dcollector);

	/* создание экземпляра своего контейнера для хранения hard с новым аллокатором ограниченным 10 элементами*/
	direct_collector<hard, reserv_allocator<hard, 10>> dcollector2;

	/* заполнение 10 значениями аналогичными первому контейнеру */
	fn_collector_fill(dcollector2);

	/* вывод на экран всех значений хранящихся в контейнере*/
	auto fn_print_collector = [](auto& collector) {
		int i = 0;
		for (hard& hard : collector) {
			cout << i++ << " " << hard.fa << " " << hard.fi << endl;
		}
	};
	/*cout << "=== direct_collector (with std::allocator): ===\n";
	fn_print_collector(dcollector);*/
	cout << "=== direct_collector (with reserv_allocator): ===\n";
	fn_print_collector(dcollector2);

	//////////////////////////////////////////////
	cout << "\n\n=============== Copy & Move constructors with same & other allocators (tests see in tests.cpp )=================\n";
	{
		// Checking helper
		auto fn = [](auto& mes, auto& collector) {
			int all = 0;
			for (auto& i : collector) {
				all += i;
			}
			bool res = all == 3;
			auto s_res = res ? "true" : "false";
			cout << mes << s_res << endl;
			assert(res);
		};

		{
			///////////// Copy ////////////////
			cout << "\nCopy from collector with same allocator: \n";
			direct_collector<int> tmp;
			tmp.emplace(1);
			tmp.emplace(2);

			direct_collector<int> collector(tmp);
			fn("Checking simple copy: ", collector);
		}

		{
			///////////// Move ////////////////
			cout << "\nMove from collector with same allocator: \n";
			// don't working - compiler optimized it (msvc)
			/*direct_collector<int> my3{};*/

			// this also optimized:
			/*auto fn_get_tmp_collector = []() -> direct_collector<int> { cout << "asdf" << endl; return direct_collector<int>{}; };
			direct_collector<int> my3(fn_get_tmp_collector());*/

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpessimizing-move"
			direct_collector<int> collector(std::move(direct_collector<int>{}));
#pragma clang diagnostic pop
			collector.emplace(1);
			collector.emplace(2);
			fn("Checking simple move: ", collector);
		}

		{
			///////////// Copy with other ////////////////
			auto tmp = direct_collector<int, reserv_allocator<int>>{};
			cout << "\nCopy from collector with other allocator: \n";
			direct_collector<int> collector(tmp);
			collector.emplace(1);
			collector.emplace(2);
			fn("Checking copy other: ", collector);
		}

		{
			///////////// Move with other ////////////////
			cout << "\nMove from collector with other allocator: \n";
			direct_collector<int> collector(direct_collector<int, reserv_allocator<int>>{});
			collector.emplace(1);
			collector.emplace(2);
			fn("Checking move other: ", collector);
		}

		{
			// Check correct new
			cout << "\n-----------------------------";
			cout << "\nMove from collector with other allocator (exp N2): \n";

			auto tmp = direct_collector<int, reserv_allocator<int>>{};
			tmp.emplace(1);
			// Move
			direct_collector<int> collector(std::move(tmp));
			// Change new
			collector.emplace(2);
			fn("Checking move other(2): ", collector);
			cout << "-----------------------------\n\n";
		}
	}

	return 0;
}
