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
	Logger(ostream& out, std::string prefix = "", std::string suffix = "", std::string ending = "") :
		_out{ out }, _prefix(prefix), _suffix(suffix), _ending(ending) {}
	template<typename Arg, typename ...Args>
	void operator()(Arg arg, Args&&... args) {
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
	ostream& _out;
	std::string _prefix;
	std::string _suffix;
	std::string _ending;
};
