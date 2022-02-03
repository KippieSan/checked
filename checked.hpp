#ifndef checked_hpp
#define checked_hpp
#include "flagged.hpp"
#include <iostream>
#include <compare>

namespace checked {
	class Checked {
	private:
		Flagged integer;
	public:
		Checked() : integer(0) {}
		Checked(const Itype& v) : integer(v) {}
		Checked(const Itype& v, const HadOverflowed& s) : integer(v, s) {}
		Checked(const Checked& c) : integer(c.integer) {};

		Itype get_value() const { return this->integer.value; }
		HadOverflowed get_status() const { return this->integer.status; }
        Flagged get_flagged() const { return this->integer; }

		Checked operator= (const Checked& c) { this->integer = c.integer; return (*this); }
		Checked operator+=(const Checked& c) { (*this) = (*this) + c; return (*this); }
		Checked operator-=(const Checked& c) { (*this) = (*this) - c; return (*this); }
		Checked operator*=(const Checked& c) { (*this) = (*this) * c; return (*this); }
		Checked operator/=(const Checked& c) { (*this) = (*this) / c; return (*this); }
		Checked operator%=(const Checked& c) { (*this) = (*this) % c; return (*this); }
		Checked operator++(int) { return (*this) + 1; }
		Checked operator--(int) { return (*this) - 1; }
		Checked operator++() { return (*this) + 1; }
		Checked operator--() { return (*this) - 1; }

	private:
		friend Checked operator+(const Checked&);
		friend Checked operator-(const Checked&);

		friend Checked operator+(const Checked&, const Checked&);
		friend Checked operator-(const Checked&, const Checked&);
		friend Checked operator*(const Checked&, const Checked&);
		friend Checked operator/(const Checked&, const Checked&);
		friend Checked operator%(const Checked&, const Checked&);

		friend std::strong_ordering operator<=>(const Checked&, const Checked&);
		friend bool operator==(const Checked&, const Checked&);

		friend std::ostream& operator<<(std::ostream&, const Checked&);
	};
}
#endif // checked_hpp