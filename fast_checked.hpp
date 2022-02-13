#ifndef fast_checked_hpp
#define fast_checked_hpp
#include <iostream>
#include <limits>
#include <compare>
// Integer type definition & Constatnt
using Itype = std::int64_t;
using uItype = std::uint64_t;
namespace {
	constexpr Itype min = std::numeric_limits<Itype>::min();
	constexpr Itype max = std::numeric_limits<Itype>::max();
}
// Flagged definition
namespace checked {
	enum class HadOverflowed : bool {
		Yes = true, No = false
	};
	struct Flagged {
		Itype value;
		HadOverflowed status;

		constexpr Flagged() noexcept : value(0), status(HadOverflowed::No) {}
		constexpr Flagged(const Itype& v) noexcept : value(v), status(HadOverflowed::No) {}
		constexpr Flagged(const Itype& v, const HadOverflowed& s) noexcept : value(v), status(s) {}
		constexpr Flagged(const Flagged& f) noexcept : value(f.value), status(f.status) {}
		constexpr Flagged operator=(const Flagged& f) noexcept { this->value = f.value, this->status = f.status; return (*this); }
	};
}
namespace checked {
	// Preparation for operators
	constexpr HadOverflowed operator||(const HadOverflowed& lhs, const HadOverflowed& rhs) noexcept;
	struct OperandData { Itype lhs; Itype rhs; HadOverflowed status; };
	enum class Sign { SamePositive, SameNegative, Different };
	constexpr Sign get_operand_sign(const Itype& lhs, const Itype& rhs) noexcept;
	// Checked class
	class Checked {
	private:
		Flagged integer;
		constexpr OperandData get_operand_data(const Checked& c) {
			return { .lhs = this->integer.value, .rhs = c.get_value(), .status = (this->integer.status || c.get_status()) };
		}
	public:
		constexpr Checked() noexcept : integer(0) {}
		constexpr Checked(const Itype& v) noexcept : integer(v) {}
		constexpr Checked(const Itype& v, const HadOverflowed& s) noexcept : integer(v, s) {}
		constexpr Checked(const Checked& c) noexcept : integer(c.integer) {};

		constexpr Itype get_value() const noexcept { return this->integer.value; }
		constexpr HadOverflowed get_status() const noexcept { return this->integer.status; }
		constexpr Flagged get_flagged() const noexcept { return this->integer; }

		constexpr Checked operator= (const Checked& c) noexcept { this->integer = c.integer; return (*this); }
		constexpr Checked operator+=(const Checked& c) noexcept {
			const auto [lhs, rhs, status] = get_operand_data(c);
			switch (get_operand_sign(lhs, rhs)) {
			case Sign::SamePositive:
				if (max - lhs >= rhs) { (*this) = Checked(lhs + rhs, status); return (*this); }
				else { (*this) = Checked(min - (max - lhs - rhs + 1), HadOverflowed::Yes); return (*this); }
			case Sign::SameNegative:
				if (min - lhs <= rhs) { (*this) = Checked(lhs + rhs, status); return (*this); }
				else { (*this) = Checked(max - (min - lhs - rhs - 1), HadOverflowed::Yes); return (*this); }
			case Sign::Different:
			default:
				(*this) = Checked(lhs + rhs, status); return (*this);
			}
		}
		constexpr Checked operator-=(const Checked& c) noexcept {
			const auto [lhs, rhs, status] = get_operand_data(c);
			if (rhs == min && lhs < 0) { (*this) = Checked(lhs - rhs, status); return (*this); }
			else if (rhs == min && lhs >= 0) { (*this) = Checked(min + lhs, HadOverflowed::Yes); return (*this); }
			else { (*this) += Checked(-rhs, status); return (*this); }
		}
		constexpr Checked operator*=(const Checked& c) noexcept {
			const auto [lhs, rhs, status] = get_operand_data(c);
			const auto wrapped = static_cast<Itype>(static_cast<uItype>(lhs) * static_cast<uItype>(rhs));

			if (lhs == 0 || lhs == 1 || rhs == 0 || rhs == 1) { (*this) = Checked(lhs * rhs, status); return(*this); }
			else if (lhs == min || rhs == min) { (*this) = Checked(wrapped, HadOverflowed::Yes); return (*this); }

			switch (get_operand_sign(lhs, rhs)) {
			case Sign::SamePositive:
			case Sign::SameNegative:
				if (std::abs(max / lhs) >= std::abs(rhs)) { (*this) = Checked(lhs * rhs, status); return (*this); }
				else { (*this) = Checked(wrapped, HadOverflowed::Yes); return (*this); }
			case Sign::Different:
			default:
				if (lhs == -1) { (*this) = Checked(lhs * rhs, status); return (*this); }
				else if (std::abs(min / lhs) >= std::abs(rhs)) { (*this) = Checked(lhs * rhs, status); return (*this); }
				else { (*this) = Checked(wrapped, HadOverflowed::Yes); return (*this); }
			}
		}
		constexpr Checked operator/=(const Checked& c) noexcept {
			const auto [lhs, rhs, status] = get_operand_data(c);
			if (lhs == min && rhs == -1) { (*this) = Checked(min, HadOverflowed::Yes); return (*this); }
			else if (rhs == 0) { (*this) = Checked(0, HadOverflowed::Yes); return (*this); }
			else { (*this) = Checked(lhs / rhs, status); return (*this); }
		}
		constexpr Checked operator%=(const Checked& c) noexcept {
			const auto [lhs, rhs, status] = get_operand_data(c);
			if (rhs == 1 || rhs == -1) { (*this) = Checked(0, status); return (*this); }
			else if (rhs == 0) { (*this) = Checked(0, HadOverflowed::Yes); return (*this); }
			else { (*this) = Checked(lhs % rhs, status); return (*this); }
		}
		constexpr Checked operator++(int) noexcept { (*this) += 1; return (*this); }
		constexpr Checked operator--(int) noexcept { (*this) -= 1; return (*this); }
		constexpr Checked operator++() noexcept { (*this) += 1; return (*this); }
		constexpr Checked operator--() noexcept { (*this) -= 1; return (*this); }
	private:
		friend std::istream& operator>>(std::istream&, Checked&) noexcept;
	};
}
// Binary operators
namespace checked {
    constexpr Checked operator+(const Checked& c) noexcept { return c; }
    constexpr Checked operator-(const Checked& c) noexcept { return Checked(-c.get_value(), c.get_status()); }
	constexpr Checked operator+(const Checked& lhs, const Checked& rhs) noexcept { Checked tmp = lhs; return tmp += rhs; }
	constexpr Checked operator-(const Checked& lhs, const Checked& rhs) noexcept { Checked tmp = lhs; return tmp -= rhs; }
	constexpr Checked operator*(const Checked& lhs, const Checked& rhs) noexcept { Checked tmp = lhs; return tmp *= rhs; }
	constexpr Checked operator/(const Checked& lhs, const Checked& rhs) noexcept { Checked tmp = lhs; return tmp /= rhs; }
	constexpr Checked operator%(const Checked& lhs, const Checked& rhs) noexcept { Checked tmp = lhs; return tmp %= rhs; }
}
// Comparison operators
namespace checked {
	constexpr bool operator==(const Checked& lhs, const Checked& rhs) noexcept { return (lhs.get_value() == rhs.get_value()); }
	constexpr std::strong_ordering operator<=>(const Checked& lhs, const Checked& rhs) noexcept {
		if (lhs == rhs) return std::strong_ordering::equal;
		else return (lhs.get_value() <=> rhs.get_value());
	}
}
// IO operators
namespace checked {
	std::ostream& operator<<(std::ostream& out, const Checked& c) noexcept { out << c.get_value(); return out; }
	std::istream& operator>>(std::istream& cin, Checked& c) noexcept { cin >> c.integer.value; return cin; }
}
// impl: Preparation for operators
namespace checked {
	constexpr HadOverflowed operator||(const HadOverflowed& lhs, const HadOverflowed& rhs) noexcept {
		return (static_cast<bool>(lhs) || static_cast<bool>(rhs)) ? HadOverflowed::Yes : HadOverflowed::No;
	}
	constexpr Sign get_operand_sign(const Itype& lhs, const Itype& rhs) noexcept {
		if (lhs >= 0 && rhs >= 0) return Sign::SamePositive;
		else if (lhs < 0 && rhs < 0) return Sign::SameNegative;
		else return Sign::Different;
	}
}
#endif // checked_h