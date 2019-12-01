#include "share.h"

#define BOOST_TEST_MODULE allocator_test_module

//#ifdef _MSC_VER
//	#define _ITERATOR_DEBUG_LEVEL 2
//#endif

#include <functional>
#include <boost/test/unit_test.hpp>
#include <ctime>
#include <map>
#include <numeric>

#include "reserv_allocator.h"
#include "direct_collector.h"

using std::string;
using std::cout;
using std::endl;
//using std::clock;

bool call_test(string name, std::function<bool(void)> fntest) {
	cout << endl << name << ":\n";
	//test_data test_data = create_test_data();

	auto startTime = clock();
	bool res = fntest();
	auto endTime = clock();

	cout << "TIME: " << endTime - startTime << "ms" << endl;
	return res;
}

bool test_reserv_allocator() {
	cout << "------------------------------\n";
	cout << __PRETTY_FUNCTION__ << endl;
	struct big {
		int i;
		int ar[10000];
		big(int i) : i(i) {}
	};
	/*const int count = 5;*/
	const int count = 10000;
	call_test("Test with std::map<int, big> map_std_alloc:", [&count]() {
		std::map<int, big> map_std_alloc;
		for (int i = 0; i < count; i++) {
			map_std_alloc.emplace(std::piecewise_construct,
				std::forward_as_tuple(i),
				std::forward_as_tuple(1));
		}
		return true;
		});
	/*using alloc = reserv_allocator<big, 5>;*/
	using alloc = reserv_allocator<big, 1000>;
	cout << "\nalloc = reserv_allocator<big, 1000>";
	call_test("test with std::map<int, big, std::less<int>, alloc> map_custom_alloc:", [&count]() {

		// speed x5 !!!
		std::map<int, big, std::less<int>, alloc> map_custom_alloc;
		for (int i = 0; i < count; i++) {
			map_custom_alloc.emplace(std::piecewise_construct,
				std::forward_as_tuple(i),
				std::forward_as_tuple(1));
		}
		return true;
		});
	cout << "------------------------------\n";
	return true;
}

bool test_direct_collector() {
	return call_test(__PRETTY_FUNCTION__, []() {
		const int count = 100;
		direct_collector<int> my;

		// Filling
		for (int i = 1; i <= count; i++) {
			my.emplace(i);
		}

		bool res = true;

		// Check iterator using
		int i = 1;
		for (auto it = my.begin(); it != my.end(); ++it) {
			if (i != *it) {
				res = false;
				break;
			}
			++i;
		}

		// Check for-loop
		int i2 = 1;
		for (auto& elt : my) {
			if (elt != i2) {
				res = false;
				break;
			}
			i2++;
		}
		return res;
	});
}

//// Checking helpers
template<class Collector>
bool cont_checker(Collector& collector, int wait_size, int wait_sum) {
	int all = std::accumulate(collector.begin(), collector.end(), 0);
	return collector.size() == wait_size && all == wait_sum;
}

template<class Collector1, class Collector2>
bool cont_checker(Collector1& collector1, Collector2& collector2, int wait_size1, int wait_size2, int sum1, int sum2) {
	int all1 = std::accumulate(collector1.begin(), collector1.end(), 0);
	int all2 = std::accumulate(collector2.begin(), collector2.end(), 0);
	return (collector1.size() == wait_size1) && (collector2.size() == wait_size2) && all1 == sum1 && all2 == sum2;
}
//// end Checking helpers

bool test_direct_collector_simple_copy() {
	return call_test(__PRETTY_FUNCTION__, []() {
		direct_collector<int> my;
		my.emplace(1);
		my.emplace(2);
		my.emplace(3);

		// Copy
		direct_collector<int> my2(my);
		my2.emplace(4);

		// Check not changed old container
		return cont_checker(my, my2, 3, 4, 6, 10);
		});
}

bool test_direct_collector_simple_move() {
	return call_test(__PRETTY_FUNCTION__, []() {
		auto tmp = direct_collector<int>{};
		tmp.emplace(1);
		tmp.emplace(2);
		// Move
		direct_collector<int> my(std::move(tmp));
		// Change new
		my.emplace(3);
		// Check correct new
		return cont_checker(my, 3, 6);
	});
}

bool test_direct_collector_copy_other() {
	return call_test(__PRETTY_FUNCTION__, []() {
		direct_collector<int, reserv_allocator<int>> my;
		my.emplace(1);
		my.emplace(2);
		my.emplace(3);

		// Copy
		direct_collector<int> my2(my);
		my2.emplace(4);

		// Check not changed old container
		return cont_checker(my, my2, 3, 4, 6, 10);
		});
}

bool test_direct_collector_move_other() {
	return call_test(__PRETTY_FUNCTION__, []() {
		auto tmp = direct_collector<int, reserv_allocator<int>>{};
		tmp.emplace(1);
		tmp.emplace(2);
		// Move
		direct_collector<int> my(std::move(tmp));
		// Change new
		my.emplace(3);
		// Check correct new
		return cont_checker(my, 3, 6);
		});
}

//struct Init {
//	Init(std::function<void()> init_func) {
//		init_func();
//	}
//};
//#define INIT(init_func) struct Init init(init_func);


BOOST_AUTO_TEST_SUITE(allocator_test_suite)
//INIT(init_base_fixtures)

BOOST_AUTO_TEST_CASE(test_of_test_system)
{
	BOOST_CHECK(test_reserv_allocator());
	BOOST_CHECK(test_direct_collector());
	BOOST_CHECK(test_direct_collector_simple_copy());
	BOOST_CHECK(test_direct_collector_simple_move());
	BOOST_CHECK(test_direct_collector_copy_other());
	BOOST_CHECK(test_direct_collector_move_other());
}

BOOST_AUTO_TEST_SUITE_END()
