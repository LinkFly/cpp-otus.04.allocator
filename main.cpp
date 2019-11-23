#include "share.h"

#include <iostream>
#include <map>
#include <vector>

#include "reserv_allocator.h"
//#include "helpers.h"
#include "direct_collector.h"

using std::cout;

void try_my_container() {

}

int main() {
	struct hard {
		int fa;
		int fi;
		hard(int fa, int fi) : fa(fa), fi(fi) {};
	};

	/*
� �������� ���������� std::map<int, hard>
� ���������� 10 ����������, ��� ���� ��� ����� �� 0 �� 9, � �������� - ��������������� �������� �����
��������� � ����� ���������
� �������� ���������� std::map<int, hard> � ����� ����������� ������������ 10 ����������
� ���������� 10 ����������, ��� ���� ��� ����� �� 0 �� 9, � �������� - ����������� ������� ����������
� ����� �� ����� ���� �������� (���� � �������� ��������� ���������) ���������� � ����������
� �������� ���������� ������ ���������� ��� �������� hard
� ���������� 10 ���������� ������������ ������� ����������
� �������� ���������� ������ ���������� ��� �������� hard � ����� ����������� ������������ 10
����������
� ���������� 10 ���������� ������������ ������� ����������
� ����� �� ����� ���� �������� ���������� � ����������
	*/

	/*�������� ���������� std::map<int, hard>*/
	std::map<int, hard> map;

	//// helpers (using map)
	auto fn_fact = [&map](int i) {
		return i * map.at(i - 1).fa;
	};
	auto fn_fib = [&map](int i) {
		return map.at(i - 2).fi + map.at(i - 1).fi;
	};
	//// end helpers (using map)

	/* ���������� 10 ����������, ��� ���� ��� ����� �� 0 �� 9, � �������� - ��������������� �������� �����
	��������� � ����� ��������� */
	auto fn_map_fill = [&fn_fact, &fn_fib](auto& map) {
		map.emplace(0, hard{ 1, 0 });
		map.emplace(1, hard{ 1, 1 });
		for (int i = 2; i < 10; i++) {
			int fa = fn_fact(i);
			int fi = fn_fib(i);
			map.emplace(i, hard{ fa, fi });
		}
	};
	fn_map_fill(map);

	/* �������� ���������� std::map<int, hard> � ����� ����������� ������������ 10 ���������� */
	std::map<int, hard, std::less<int>, reserv_allocator<std::pair<const int, hard>, 10>> map2;

	/* ���������� 10 ����������, ��� ���� ��� ����� �� 0 �� 9, � �������� - ����������� ������� ���������� */
	fn_map_fill(map2);

	/* ����� �� ����� ���� �������� (���� � �������� ��������� ���������) ���������� � ���������� */
	auto fn_print_map = [](auto& map) {
		for (auto& pair : map) {
			cout << pair.first << " " << pair.second.fa << " " << pair.second.fi << endl;
		}
	};
	/*cout << "=== map (with std::allocator): ===\n";
	fn_print_map(map);*/
	cout << "=== map (with reserv_allocator): ===\n";
	fn_print_map(map2);

	/* �������� ���������� ������ ���������� ��� �������� hard */
	direct_collector<hard> dcollector;
	
	/* ���������� 10 ���������� ������������ ������� ���������� */
	auto fn_collector_fill = [&fn_fact, &fn_fib](auto& collector) {
		collector.emplace(1, 0);
		collector.emplace(1, 1);
		for (int i = 2; i < 10; i++) {
			collector.emplace(fn_fact(i), fn_fib(i));
		}
	};
	fn_collector_fill(dcollector);

	/* �������� ���������� ������ ���������� ��� �������� hard � ����� ����������� ������������ 10 ����������*/
	direct_collector<hard, reserv_allocator<hard, 10>> dcollector2;

	/* ���������� 10 ���������� ������������ ������� ���������� */
	fn_collector_fill(dcollector2);

	/* ����� �� ����� ���� �������� ���������� � ����������*/
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

	return 0;
}