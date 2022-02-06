#ifndef flagged_hpp
#define flagged_hpp
#include "int_def.hpp"
#include <iostream>
using Itype = std::int64_t;
using uItype = std::uint64_t;
namespace checked {
	enum class HadOverflowed : bool {
		Yes = true, No = false
	};
	struct Flagged {
		Itype value;
		HadOverflowed status;

		Flagged() : value(0), status(HadOverflowed::No) {}
		Flagged(const Itype& v) : value(v), status(HadOverflowed::No) {}
		Flagged(const Itype& v, const HadOverflowed& s) : value(v), status(s) {}
		Flagged(const Flagged& f) : value(f.value), status(f.status) {}
		Flagged operator=(const Flagged& f) { this->value = f.value, this->status = f.status; return (*this); }
	};
}
#endif
