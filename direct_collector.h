#pragma once
#include <memory>
#include <stack>

#include "ptrs_manager.h"

const size_t ptrs_count_default = 10;

template<typename T, class Alloc = std::allocator<T>>
class direct_collector {
public:
	class iterator {
		typename ptrs_manager<T>::cur_ptr_spec cur_spec;
		ptrs_manager<T>* pptrs_mng;
		T* cur_ptr;
		friend class direct_collector;
	public:
		iterator(direct_collector* parent) {
			pptrs_mng = parent->pptrs_mng;
			cur_spec = pptrs_mng->init_ptr_spec();
			this->operator++();
		}
		iterator& operator++() {
			bool next_res = pptrs_mng->next_ptr_spec(cur_spec);
			if (!next_res) {
				cur_ptr = nullptr;
			}
			else cur_ptr = pptrs_mng->get_ptr(cur_spec);
			return *this;
		}
		T& operator*() {
			return *cur_ptr;
		}
		iterator operator++(int) {
			iterator it_new = *this;
			this->operator++();
			return it_new;
		}
		bool operator==(iterator& other) {
			return this->cur_ptr == other.cur_ptr;
		}
		bool operator!=(iterator& other) {
			return this->cur_ptr != other.cur_ptr;
		}

		//// For using in parent
		static iterator begin(direct_collector* parent) {
			return iterator(parent);
		}
		static iterator end(direct_collector* parent) {
			iterator res{ parent };
			res.cur_ptr = nullptr;
			return res;
		}
		//// end For using in parent
	};

	size_t length = 0;

	iterator begin() {
		return iterator::begin(this);
	}
	iterator end() {
		return iterator::end(this);
	}

	template<typename ...Args>
	void emplace(Args... args) {
		T* ptr = allocator.allocate(1);
		pptrs_mng->add_ptr(ptr);
		allocator.construct(ptr, args...);
		++length;
	}

	size_t size() { return length; }

	direct_collector(size_t ptrs_count = ptrs_count_default) {
		pptrs_mng = new ptrs_manager<T>(ptrs_count);
	}

	~direct_collector() {
		for (auto it = begin(); it != end(); ++it) {
			allocator.destroy(it.cur_ptr);
			allocator.deallocate(it.cur_ptr, 1);
		}
		delete pptrs_mng;
	}

private:
	ptrs_manager<T>* pptrs_mng;
	Alloc allocator;
};