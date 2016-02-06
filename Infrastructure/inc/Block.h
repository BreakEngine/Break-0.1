#pragma once
#include "Globals.hpp"

namespace Break {
	struct Block {
		byte* ptr;
		size_t size;

		Block() {
			reset();
		}

		void reset() {
			ptr = nullptr;
			size = 0;
		}
	};
}
