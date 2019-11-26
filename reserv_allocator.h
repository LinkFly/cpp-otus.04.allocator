#pragma once

#include "logger.h"
#include "block_manager.h"

template<typename _Ty, size_t count = 5>
struct reserv_allocator {
	static_assert(count > 0, "count must be greater zero");
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
	//reserv_allocator(const reserv_allocator& source) noexcept {
	//}

	//template <class _Other, size_t count>
	//constexpr reserv_allocator(const reserv_allocator<_Other, count>& cur_alloc) noexcept {}

	_Ty* allocate(size_t n);
	void deallocate(_Ty* p, size_t n);

	template<typename U, typename ...Args>
	void construct(U* p, Args&&... args);

	template<typename U>
	void destroy(U* p);
private:
	Logger loginfo{ std::cout };
	block_manager<_Ty, count> bmanager;
};

template<typename _Ty, size_t count>
reserv_allocator<_Ty, count>::reserv_allocator() noexcept {
	loginfo(" [reserv_allocator]");
	bmanager.alloc_block();
}

template<typename _Ty, size_t count>
reserv_allocator<_Ty, count>::~reserv_allocator() noexcept {
	loginfo("[~reserv_allocator]");
	bmanager.dealloc_all_blocks();
}

template<typename _Ty, size_t count>
_Ty* reserv_allocator<_Ty, count>::allocate(size_t n) {
	return bmanager.get_ptr(n);
}

template<typename _Ty, size_t count>
void reserv_allocator<_Ty, count>::deallocate(_Ty* p, [[maybe_unused]] size_t n) {
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
