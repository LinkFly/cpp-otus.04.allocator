#pragma once


// TODO Add to build parameters
//#define ASSERT_ALLOC 1

template<typename _Ty, size_t count>
struct block_manager {
	void alloc_block() {
		loginfo("=== malloc(...) ptr + ", count, " ===\n");
		auto before_ptr_alloc = ptr_alloc;
		ptr_alloc = malloc(sizeof(before_ptr_alloc) + count * sizeof(_Ty));
		if (!ptr_alloc)
			// TODO Analize it (maybe delete)
			throw std::bad_alloc();
		*(reinterpret_cast<void**>(ptr_alloc)) = before_ptr_alloc; // for dealloc
		allocate_count = count;
	}

	void dealloc_all_blocks() {
		while (ptr_alloc != nullptr) {
			loginfo("=== dealloc() ptr + ", count, " ===\n");
			void* before_ptr_alloc = *(reinterpret_cast<void**>(ptr_alloc));
			free(ptr_alloc);
			ptr_alloc = before_ptr_alloc;
		}
	}
	_Ty* get_ptr(size_t n) {
#ifdef ASSERT_ALLOC
		assert(n <= count && (allocate_count == 0 || (n <= allocate_count)));
#endif
		loginfo(" [allocate n = ", n, "]");
		if (allocate_count == 0) {
			alloc_block();
		}
		else {
			loginfo("=== not malloc ===\n");
		}
		allocate_count -= n;
		void* p = (char*)ptr_alloc + sizeof(ptr_alloc) + allocate_count * sizeof(_Ty);
		return reinterpret_cast<_Ty*>(p);
	}
private:
	Logger loginfo{ std::cout };
	void* ptr_alloc = nullptr;
	size_t allocate_count = 0;
};
