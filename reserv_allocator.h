#pragma once

#include <iostream>
// TODO check necessary
#include <sstream>  //include this to use string streams
#include <string> 

#include "logger.h"

//#define ASSERT_ALLOC 1

template<typename _Ty, size_t count = 5>
struct reserv_allocator {
	static_assert(count > 0);
	using value_type = _Ty;

	//using size_type = std::size_t;
	//using pointer = _Ty*;
	//using const_pointer = const _Ty*;
	//using reference = _Ty&;
	//using const_reference = const _Ty&;

	template<typename U>
	struct rebind {
		using other = reserv_allocator<U, count>;
	};

	reserv_allocator() noexcept;

	~reserv_allocator() noexcept;

	/*constexpr reserv_allocator(const reserv_allocator&) noexcept = default;*/
	//template <class _Other, size_t count>
	//constexpr reserv_allocator(const reserv_allocator<_Other, count>& cur_alloc) noexcept {}

	_Ty* allocate(size_t n);
	void deallocate(_Ty* p, size_t n);

	template<typename U, typename ...Args>
	void construct(U* p, Args&&... args);

	template<typename U>
	void destroy(U* p);


	void* ptr_alloc = nullptr;
	size_t allocate_count = 0;
private:
	Logger loginfo{ std::cout };

	void alloc();
	void dealloc();
};

//using std::cout;
//using std::map;
//using namespace std;

template<typename _Ty, size_t count>
reserv_allocator<_Ty, count>::reserv_allocator() noexcept {
	loginfo(" [reserv_allocator]");
	alloc();
}

template<typename _Ty, size_t count>
reserv_allocator<_Ty, count>::~reserv_allocator() noexcept {
	loginfo("[~reserv_allocator]");
	dealloc();
}

template<typename _Ty, size_t count>
void reserv_allocator<_Ty, count>::alloc() {
	loginfo("=== malloc(...) ptr + ", count, " ===\n");
	auto before_ptr_alloc = ptr_alloc;
	ptr_alloc = malloc(sizeof(before_ptr_alloc) + count * sizeof(_Ty));
	if (!ptr_alloc)
		throw std::bad_alloc();
	*(reinterpret_cast<void**>(ptr_alloc)) = before_ptr_alloc; // for dealloc
	allocate_count = count;
}

template<typename _Ty, size_t count>
void reserv_allocator<_Ty, count>::dealloc() {
	while (ptr_alloc != nullptr) {
		loginfo("=== dealloc() ptr + ", count, " ===\n");
		void* before_ptr_alloc = *(reinterpret_cast<void**>(ptr_alloc));
		delete ptr_alloc;
		ptr_alloc = before_ptr_alloc;
	}
}

template<typename _Ty, size_t count>
_Ty* reserv_allocator<_Ty, count>::allocate(size_t n) {
#ifdef ASSERT_ALLOC
	assert(n <= count && (allocate_count == 0 || (n <= allocate_count)));
#endif
	loginfo(" [allocate n = ", n, "]");
	if (allocate_count == 0) {
		alloc();
	}
	else {
		loginfo("=== not malloc ===\n");
	}
	allocate_count -= n;
	void* p = (std::byte*)ptr_alloc + sizeof(ptr_alloc) + allocate_count * sizeof(_Ty);
	return reinterpret_cast<_Ty*>(p);
}

template<typename _Ty, size_t count>
void reserv_allocator<_Ty, count>::deallocate(_Ty* p, [[maybe_unused]] size_t n) {
	/*if (LOG_ON) cout << __PRETTY_FUNCTION__ << " [deallocate n = " << n << "]" << "\n\n";
	free(p);*/
	loginfo(" [deallocate n = 0 (disabled)]\n");
}

template<typename _Ty, size_t count>
template<typename U, typename ...Args>
void reserv_allocator<_Ty, count>::construct(U* p, Args&&... args) {
	loginfo("[construct]\n");
	new(p) U(std::forward<Args>(args)...);
}

template<typename _Ty, size_t count>
template<typename U>
void reserv_allocator<_Ty, count>::destroy(U* p) {
	loginfo("[destroy]\n");
	p->~U();
}
