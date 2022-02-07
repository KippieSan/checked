#ifndef checked_hpp
#define checked_hpp
#include "flagged.hpp"
#include <iostream>
#include <compare>

namespace checked {
    class Checked;
    Checked operator+(const Checked&);
    Checked operator-(const Checked&);
    Checked operator+(const Checked&, const Checked&);
	Checked operator-(const Checked&, const Checked&);
	Checked operator*(const Checked&, const Checked&);
	Checked operator/(const Checked&, const Checked&);
	Checked operator%(const Checked&, const Checked&);
    std::strong_ordering operator<=>(const Checked&, const Checked&);
	bool operator==(const Checked&, const Checked&);
	std::ostream& operator<<(std::ostream&, const Checked&);

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
        friend std::istream& operator>>(std::istream&, Checked&);
	};
}
#endif // checked_hpp
