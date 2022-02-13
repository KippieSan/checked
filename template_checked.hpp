#ifndef template_checked_hpp
#define template_checked_hpp
#include <iostream>
#include <limits>
#include <compare>
// Constatnt
namespace {
	template <typename T> constexpr T min = std::numeric_limits<T>::min();
	template <typename T> constexpr T max = std::numeric_limits<T>::max();
}
// Flagged definition
namespace checked {
	enum class HadOverflowed : bool {
		Yes = true, No = false
	};
	template <typename T> struct Flagged {
		T value;
		HadOverflowed status;

		constexpr Flagged() noexcept : value(0), status(HadOverflowed::No) {}
		constexpr Flagged(const T& v) noexcept : value(v), status(HadOverflowed::No) {}
		constexpr Flagged(const T& v, const HadOverflowed& s) noexcept : value(v), status(s) {}
		constexpr Flagged(const Flagged& f) noexcept : value(f.value), status(f.status) {}
		constexpr Flagged operator=(const Flagged& f) noexcept { this->value = f.value, this->status = f.status; return (*this); }
	};
}
namespace checked {
	// Preparation for operators
	constexpr HadOverflowed operator||(const HadOverflowed& lhs, const HadOverflowed& rhs) noexcept;
	template <typename T> struct OperandData { T lhs; T rhs; HadOverflowed status; };
	enum class Sign { SamePositive, SameNegative, Different };
	template <typename T> constexpr Sign get_operand_sign(const T& lhs, const T& rhs) noexcept;
	// Checked class
	template <typename T> class Checked {
	private:
		Flagged<T> integer;
		constexpr OperandData<T> get_operand_data(const Checked<T>& c) {
			return { .lhs = this->integer.value, .rhs = c.get_value(), .status = (this->integer.status || c.get_status()) };
		}
	public:
		constexpr Checked() noexcept : integer(0) {}
		constexpr Checked(const T& v) noexcept : integer(v) {}
		constexpr Checked(const T& v, const HadOverflowed& s) noexcept : integer(v, s) {}
		constexpr Checked(const Checked& c) noexcept : integer(c.integer) {};

		constexpr T get_value() const noexcept { return this->integer.value; }
		constexpr HadOverflowed get_status() const noexcept { return this->integer.status; }
		constexpr Flagged<T> get_flagged() const noexcept { return this->integer; }

		constexpr Checked operator= (const Checked& c) noexcept { this->integer = c.integer; return (*this); }
		constexpr Checked operator+=(const Checked& c) noexcept {
			const auto [lhs, rhs, status] = get_operand_data(c);
			switch (get_operand_sign(lhs, rhs)) {
			case Sign::SamePositive:
				if (max<T> - lhs >= rhs) { (*this) = Checked(lhs + rhs, status); return (*this); }
				else { (*this) = Checked(max<T>, HadOverflowed::Yes); return (*this); }
			case Sign::SameNegative:
				if (min<T> - lhs <= rhs) { (*this) = Checked(lhs + rhs, status); return (*this); }
				else { (*this) = Checked(min<T>, HadOverflowed::Yes); return (*this); }
			case Sign::Different:
			default:
				(*this) = Checked(lhs + rhs, status); return (*this);
			}
		}
		constexpr Checked operator-=(const Checked& c) noexcept {
			const auto [lhs, rhs, status] = get_operand_data(c);
			if (rhs == min<T> && lhs < 0) { (*this) = Checked(lhs - rhs, status); return (*this); }
			else if (rhs == min<T> && lhs >= 0) { (*this) = Checked(max<T>, HadOverflowed::Yes); return (*this); }
			else { (*this) += Checked(-rhs, status); return (*this); }
		}
		constexpr Checked operator*=(const Checked& c) noexcept {
			const auto [lhs, rhs, status] = get_operand_data(c);

			if (lhs == 0 || lhs == 1 || rhs == 0 || rhs == 1) { (*this) = Checked(lhs * rhs, status); return(*this); }
			else if (lhs == min<T> || rhs == min<T>) {
                (*this) = get_operand_sign(lhs, rhs) == Sign::Different ? Checked(max<T>, HadOverflowed::Yes) : Checked(min<T>, HadOverflowed::Yes);
                return (*this);
            }

			switch (get_operand_sign(lhs, rhs)) {
			case Sign::SamePositive:
			case Sign::SameNegative:
				if (std::abs(max<T> / lhs) >= std::abs(rhs)) { (*this) = Checked(lhs * rhs, status); return (*this); }
				else { (*this) = Checked(max<T>, HadOverflowed::Yes); return (*this); }
			case Sign::Different:
			default:
				if (lhs == -1) { (*this) = Checked(lhs * rhs, status); return (*this); }
				else if (std::abs(min<T> / lhs) >= std::abs(rhs)) { (*this) = Checked(lhs * rhs, status); return (*this); }
				else { (*this) = Checked(min<T>, HadOverflowed::Yes); return (*this); }
			}
		}
		constexpr Checked operator/=(const Checked& c) noexcept {
			const auto [lhs, rhs, status] = get_operand_data(c);
			if (lhs == min<T> && rhs == -1) { (*this) = Checked(min<T>, HadOverflowed::Yes); return (*this); }
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
		friend std::istream& operator>><>(std::istream&, Checked<T>&) noexcept;
	};
}
// Binary operators
namespace checked {
    template <typename T> constexpr Checked<T> operator+(const Checked<T>& c) noexcept { return c; }
    template <typename T> constexpr Checked<T> operator-(const Checked<T>& c) noexcept { return Checked(-c.get_value(), c.get_status()); }
	template <typename T> constexpr Checked<T> operator+(const Checked<T>& lhs, const Checked<T>& rhs) noexcept { Checked tmp = lhs; return tmp += rhs; }
	template <typename T> constexpr Checked<T> operator-(const Checked<T>& lhs, const Checked<T>& rhs) noexcept { Checked tmp = lhs; return tmp -= rhs; }
	template <typename T> constexpr Checked<T> operator*(const Checked<T>& lhs, const Checked<T>& rhs) noexcept { Checked tmp = lhs; return tmp *= rhs; }
	template <typename T> constexpr Checked<T> operator/(const Checked<T>& lhs, const Checked<T>& rhs) noexcept { Checked tmp = lhs; return tmp /= rhs; }
	template <typename T> constexpr Checked<T> operator%(const Checked<T>& lhs, const Checked<T>& rhs) noexcept { Checked tmp = lhs; return tmp %= rhs; }
}
// Comparison operators
namespace checked {
	template <typename T> constexpr bool operator==(const Checked<T>& lhs, const Checked<T>& rhs) noexcept { return (lhs.get_value() == rhs.get_value()); }
	template <typename T> constexpr std::strong_ordering operator<=>(const Checked<T>& lhs, const Checked<T>& rhs) noexcept {
		if (lhs == rhs) return std::strong_ordering::equal;
		else return (lhs.get_value() <=> rhs.get_value());
	}
}
// IO operators
namespace checked {
	template <typename T> std::ostream& operator<<(std::ostream& out, const Checked<T>& c) noexcept { out << c.get_value(); return out; }
	template <typename T> std::istream& operator>>(std::istream& cin, Checked<T>& c) noexcept { cin >> c.integer.value; return cin; }
}
// impl: Preparation for operators
namespace checked {
	constexpr HadOverflowed operator||(const HadOverflowed& lhs, const HadOverflowed& rhs) noexcept {
		return (static_cast<bool>(lhs) || static_cast<bool>(rhs)) ? HadOverflowed::Yes : HadOverflowed::No;
	}
	template <typename T> constexpr Sign get_operand_sign(const T& lhs, const T& rhs) noexcept {
		if (lhs >= 0 && rhs >= 0) return Sign::SamePositive;
		else if (lhs < 0 && rhs < 0) return Sign::SameNegative;
		else return Sign::Different;
	}
}
#endif // checked_h