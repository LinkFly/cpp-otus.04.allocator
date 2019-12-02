#pragma once

#include "share.h"

#include <iostream>
#include <chrono>
#include <string>

using std::ostream;

using std::endl;

// TODO To add build params
//#define LOG_ON 1

struct Logger {
	Logger([[maybe_unused]] ostream& out = std::cout , std::string prefix = "", std::string suffix = "", std::string ending = "") :
		_prefix(prefix), _suffix(suffix), _ending(ending) {
#ifdef LOG_ON
		_out = out;
#endif
	}
	template<typename Arg, typename ...Args>
	void operator()([[maybe_unused]] Arg arg, [[maybe_unused]] Args&&... args) {
#ifdef LOG_ON
		std::string cur_prefix;
		if (_prefix != "") {
			cur_prefix = _prefix;
		}
		else {
			auto now = std::chrono::system_clock::now();
			cur_prefix = "[" + std::to_string(now.time_since_epoch().count()) + "] ";
		}
		if (LOG_ON) {
			/*_out << endl << __PRETTY_FUNCTION__ << endl;*/
			_out << cur_prefix << arg;
			((_out << _suffix << args), ...);
			_out << _ending << endl;
		}
#endif
	}
private:
#ifdef LOG_ON
	ostream& _out;
#endif
	std::string _prefix;
	std::string _suffix;
	std::string _ending;
};
