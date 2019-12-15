#pragma once
#include <memory>
#include <stack>

#include "ptrs_manager.h"

const size_t ptrs_count_default = 10;

template<typename T, class Alloc = std::allocator<T>>
struct direct_collector {
	class iterator;
	size_t length = 0;
	ptrs_manager<T>* pptrs_mng;
	Alloc* allocator;
	/*std::function<void(class iterator&)> fnDeallocByOldAlloc = nullptr;*/
	class iterator {
		typename ptrs_manager<T>::cur_ptr_spec cur_spec;
		ptrs_manager<T>* pptrs_mng;
		T* cur_ptr;
		friend struct direct_collector;
	public:
		iterator(direct_collector* parent) {
			pptrs_mng = parent->pptrs_mng;
			if (!pptrs_mng) {
				cur_ptr = nullptr;
				return;
			}
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
		bool operator==(iterator other) {
			return this->cur_ptr == other.cur_ptr;
		}
		bool operator!=(iterator other) {
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

	auto begin() {
		return iterator::begin(this);
	}
	auto end() {
		return iterator::end(this);
	}

	template<typename ...Args>
	void emplace(Args&&... args) {
		T* ptr = allocator->allocate(1);
		pptrs_mng->add_ptr(ptr);
		allocator->construct(ptr, std::forward<Args>(args)...);
		++length;
	}

	void copy(const T& t) {
		emplace(t);
	}

	size_t size() { return length; }
	direct_collector(long long ptrs_count = ptrs_count_default) {
		pptrs_mng = new ptrs_manager<T>(ptrs_count);
		allocator = new Alloc();
	}

	auto get_ptrs_count() const {
		return pptrs_mng->ptrs_count;
	}

	template<class U>
	void copy_all_in(U* new_owner) const {
		pptrs_mng->template copy_all_in<U>(new_owner);
	}

	~direct_collector() {
		auto it = begin();
		/*if (fnDeallocByOldAlloc != nullptr) {
			fnDeallocByOldAlloc(it);
		}*/
		for (;it != end(); ++it) {
			allocator->destroy(it.cur_ptr);
			allocator->deallocate(it.cur_ptr, 1);
		}
		delete pptrs_mng;
		delete allocator;
	}

	direct_collector(const direct_collector<T, Alloc>& collector) : direct_collector<T, Alloc>{ collector.get_ptrs_count() } {
		collector.copy_all_in(this);
	}

	direct_collector(direct_collector<T, Alloc> && collector) noexcept {
		pptrs_mng = collector.pptrs_mng;
		allocator = collector.allocator;
		length = collector.length;
		collector.pptrs_mng = nullptr;
		collector.allocator = nullptr;
	}

	// Using same algorithm - no problem copying elements from collector with other allocator
	template<class OtherAlloc>
	direct_collector(const direct_collector<T, OtherAlloc>& collector) : direct_collector<T, Alloc>{ collector.get_ptrs_count() } {
		collector.copy_all_in(this);
	}

	template<class OtherAlloc>
	direct_collector(direct_collector<T, OtherAlloc>&& collector) : direct_collector<T, Alloc>{ collector.get_ptrs_count() } {
		/*allocator = new Alloc();
		pptrs_mng = collector.pptrs_mng;
		size_t eltCount = this->length = collector.length;
		auto moved_alloc = collector.allocator;*/

		for (auto& elt : collector) {
			this->emplace(elt);
		}

		//fnDeallocByOldAlloc = [this, moved_alloc, eltCount](auto& it) {
		//	size_t i = 0;
		//	for (; it != this->end(); ++it) {
		//		if (++i > eltCount) break;
		//		moved_alloc->destroy(it.cur_ptr);
		//		moved_alloc->deallocate(it.cur_ptr, 1);
		//	}
		//	delete moved_alloc;
		//};

		collector.pptrs_mng = nullptr;
		collector.allocator = nullptr;
	}
};
