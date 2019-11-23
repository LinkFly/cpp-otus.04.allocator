#include "share.h"

#define BOOST_TEST_MODULE allocator_test_module

//#ifdef _MSC_VER
//	#define _ITERATOR_DEBUG_LEVEL 2
//#endif

#include <functional>
#include <boost/test/unit_test.hpp>
#include <ctime>
#include <map>

#include "reserv_allocator.h"
#include "helpers.h"
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
}

BOOST_AUTO_TEST_SUITE_END()
