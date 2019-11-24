#pragma once

template<typename T>
struct ptrs_manager {
	long long ptrs_count;
	struct ptrs_block {
		/*T* ptrs[ptrs_count];*/
		T** ptrs;
		ptrs_block* next;
	};
	struct cur_ptr_spec {
		ptrs_block* block;
		int idx;
	};
	// Init block (for delete all block in destructor)
	ptrs_block* first_ptrs_block;
	// Current block, idx
	cur_ptr_spec cur_spec;

	ptrs_manager(long long ptrs_count) : ptrs_count{ ptrs_count } {
		init_block();
	}
	~ptrs_manager() {
		delete_all_blocks();
	}

	auto alloc_block() {
		auto new_block = new ptrs_block{};
		T** ptrs = new T * [static_cast<size_t>(ptrs_count)];
		new_block->ptrs = ptrs;
		return new_block;
	}

	void init_block() {
		first_ptrs_block = alloc_block();
		cur_spec.block = first_ptrs_block;
		cur_spec.idx = 0;
	}

	void next_place() {
		if (++cur_spec.idx == ptrs_count) {
			ptrs_block* new_block = alloc_block();
			cur_spec.block->next = new_block;
			cur_spec.block = new_block;
			cur_spec.idx = 0;
		}
	}

	void add_ptr(T* ptr) {
		// TODO avoid static_cast (change algorithm to use size_t for idx)
		cur_spec.block->ptrs[static_cast<size_t>(cur_spec.idx)] = ptr;
		next_place();
	}

	// For first iteration require to call next_ptr_spec
	cur_ptr_spec init_ptr_spec() {
		cur_ptr_spec res;
		res.block = first_ptrs_block;
		res.idx = -1;
		return res;
	}

	// for iterator
	bool next_ptr_spec(cur_ptr_spec& spec) {
		cur_ptr_spec spec_loc = spec;
		if (++spec_loc.idx == ptrs_count) {
			spec_loc.block = spec_loc.block->next;
			if (spec_loc.block == nullptr) {
				return false;
			}
			spec_loc.idx = 0;
		}
		if (spec_loc.block == cur_spec.block) {
			if (spec_loc.idx >= cur_spec.idx) {
				return false;
			}
		}
		spec = spec_loc;
		return true;
	}

	// for iterator
	T* get_ptr(cur_ptr_spec& spec) {
		return spec.block->ptrs[static_cast<size_t>(spec.idx)];
	}

	void delete_all_blocks() {
		auto cur = first_ptrs_block;
		while (cur != nullptr) {
			auto next = cur->next;
			delete cur->ptrs;
			delete cur;
			cur = next;
		}
	}
};
