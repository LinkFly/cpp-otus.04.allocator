#pragma once

#include <iostream>

#include "logger.h"

Logger loginfo2(std::cout);

struct hard {
	int i;
	double d;
	hard() : i{ 0 }, d{ .0 } {
		loginfo2("hard()");
	}
	hard(int i, double d) : i{ i }, d{ d } {
		loginfo2("hard(int, double)");
	}
	/*hard(const hard& hard) = delete;*/
	hard(const hard& hard) {
		loginfo2("hard(hard&)");
	}
	hard(const hard&& hard) noexcept {
		loginfo2("hard&&");
	}
	~hard() {
		loginfo2("~hard");
	}
};

