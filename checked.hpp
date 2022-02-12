#ifndef checked_hpp
#define checked_hpp
#include "flagged.hpp"
#include <iostream>
#include <compare>

namespace checked {
    class Checked;
    Checked operator+(const Checked&) noexcept;
    Checked operator-(const Checked&) noexcept;
    Checked operator+(const Checked&, const Checked&) noexcept;
	Checked operator-(const Checked&, const Checked&) noexcept;
	Checked operator*(const Checked&, const Checked&) noexcept;
	Checked operator/(const Checked&, const Checked&) noexcept;
	Checked operator%(const Checked&, const Checked&) noexcept;
    std::strong_ordering operator<=>(const Checked&, const Checked&) noexcept;
	bool operator==(const Checked&, const Checked&) noexcept;
	std::ostream& operator<<(std::ostream&, const Checked&) noexcept;

	class Checked {
	private:
		Flagged integer;
	public:
		Checked() noexcept : integer(0) {}
		Checked(const Itype& v) noexcept : integer(v) {}
		Checked(const Itype& v, const HadOverflowed& s) noexcept : integer(v, s) {}
		Checked(const Checked& c) noexcept : integer(c.integer) {};

		Itype get_value() const noexcept { return this->integer.value; }
		HadOverflowed get_status() const noexcept { return this->integer.status; }
        Flagged get_flagged() const noexcept { return this->integer; }

		Checked operator= (const Checked& c) noexcept { this->integer = c.integer; return (*this); }
		Checked operator+=(const Checked& c) noexcept { (*this) = (*this) + c; return (*this); }
		Checked operator-=(const Checked& c) noexcept { (*this) = (*this) - c; return (*this); }
		Checked operator*=(const Checked& c) noexcept { (*this) = (*this) * c; return (*this); }
		Checked operator/=(const Checked& c) noexcept { (*this) = (*this) / c; return (*this); }
		Checked operator%=(const Checked& c) noexcept { (*this) = (*this) % c; return (*this); }
		Checked operator++(int) noexcept { (*this) = (*this) + 1; return (*this); }
		Checked operator--(int) noexcept { (*this) = (*this) - 1; return (*this); }
		Checked operator++() noexcept { (*this) = (*this) + 1; return (*this); }
		Checked operator--() noexcept { (*this) = (*this) - 1; return (*this); }
    private:
        friend std::istream& operator>>(std::istream&, Checked&) noexcept;
	};
}
#endif // checked_hpp