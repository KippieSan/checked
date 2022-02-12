#include "checked.hpp"
#include <iostream>
#include <limits>
#include <compare>
namespace {
	constexpr Itype min = std::numeric_limits<Itype>::min();
	constexpr Itype max = std::numeric_limits<Itype>::max();
}
namespace checked {
	Checked operator+(const Checked& rhs) noexcept {
		return rhs;
	}
	Checked operator-(const Checked& rhs) noexcept {
		const auto&& [value, status] = rhs.get_flagged();
		if (value == min && value < 0) return Checked(min, HadOverflowed::Yes);
		else return Checked(-value, status);
	}
}
namespace checked {
    HadOverflowed operator||(const HadOverflowed& lhs, const HadOverflowed& rhs) noexcept {
        return (static_cast<bool>(lhs) || static_cast<bool>(rhs)) ? HadOverflowed::Yes : HadOverflowed::No;
    }
	struct OperandData {
		Itype lhs;
		Itype rhs;
		HadOverflowed status;
	};
	OperandData get_operand_data(const Checked& lhs, const Checked& rhs) noexcept {
		const auto&& status = lhs.get_status() || rhs.get_status();
		return { .lhs = lhs.get_value(), .rhs = rhs.get_value(), .status = status };
	}
	enum class Sign {
		SamePositive, SameNegative, Different
	};
	Sign get_operand_sign(const Itype& lhs, const Itype& rhs) noexcept {
		if (lhs >= 0 && rhs >= 0) return Sign::SamePositive;
		else if (lhs < 0 && rhs < 0) return Sign::SameNegative;
		else return Sign::Different;
	}
}
namespace checked {
	Checked operator+(const Checked& lhs_, const Checked& rhs_) noexcept {
		const auto&& [lhs, rhs, status] = get_operand_data(lhs_, rhs_);
		switch (get_operand_sign(lhs, rhs)) {
		case Sign::SamePositive:
			if (max - lhs >= rhs) return Checked(lhs + rhs, status);
			else return Checked(min - (max - lhs - rhs + 1), HadOverflowed::Yes);
			break;
		case Sign::SameNegative:
			if (min - lhs <= rhs) return Checked(lhs + rhs, status);
			else return Checked(max - (min - lhs - rhs - 1), HadOverflowed::Yes);
			break;
		case Sign::Different:
		default:
			return Checked(lhs + rhs, status);
			break;
		}
	}
	Checked operator-(const Checked& lhs_, const Checked& rhs_) noexcept {
		const auto&& [lhs, rhs, status] = get_operand_data(lhs_, rhs_);
		if (rhs == min && lhs < 0) return Checked(lhs - rhs, status);
		else if (rhs == min && lhs >= 0) return Checked(min + lhs, HadOverflowed::Yes);
		else return (Checked(lhs, status) + Checked(-rhs));
	}
	Checked operator*(const Checked& lhs_, const Checked& rhs_) noexcept {
		const auto&& [lhs, rhs, status] = get_operand_data(lhs_, rhs_);
		const auto&& wrapped = static_cast<Itype>(static_cast<uItype>(lhs) * static_cast<uItype>(rhs));

		if (lhs == 0 || lhs == 1 || rhs == 0 || rhs == 1) return Checked(lhs * rhs, status);
		else if (lhs == min || rhs == min) return Checked(wrapped, HadOverflowed::Yes);

		switch (get_operand_sign(lhs, rhs)) {
		case Sign::SamePositive:
		case Sign::SameNegative:
			if (std::abs(max / lhs) >= std::abs(rhs)) return Checked(lhs * rhs, status);
			else return Checked(wrapped, HadOverflowed::Yes);
			break;
		case Sign::Different:
		default:
			if (lhs == -1) return Checked(lhs * rhs, status);
			else if (std::abs(min / lhs) >= std::abs(rhs)) return Checked(lhs * rhs, status);
			else return Checked(wrapped, HadOverflowed::Yes);
		}
	}
	Checked operator/(const Checked& lhs_, const Checked& rhs_) noexcept {
		const auto&& [lhs, rhs, status] = get_operand_data(lhs_, rhs_);
		if (lhs == min && rhs == -1) return Checked(min, HadOverflowed::Yes);
		else if (rhs == 0) return Checked(0, HadOverflowed::Yes);
		else return Checked(lhs / rhs, status);
	}
	Checked operator%(const Checked& lhs_, const Checked& rhs_) noexcept {
		const auto&& [lhs, rhs, status] = get_operand_data(lhs_, rhs_);
		if (rhs == 1 || rhs == -1) return Checked(0, status);
		else if (rhs == 0) return Checked(0, HadOverflowed::Yes);
		else return Checked(lhs % rhs, status);
	}
}
namespace checked {
	bool operator==(const Checked& lhs_, const Checked& rhs_) noexcept {
		const auto&& [lhs, rhs, status] = get_operand_data(lhs_, rhs_);
		return (lhs == rhs);
	}
	std::strong_ordering operator<=>(const Checked& lhs_, const Checked& rhs_) noexcept {
		const auto&& [lhs, rhs, status] = get_operand_data(lhs_, rhs_);
        if (lhs == rhs) return std::strong_ordering::equal;
		else return lhs <=> rhs;
	}
	std::ostream& operator<<(std::ostream& out, const Checked& c) noexcept {
		out << c.get_value();
		return out;
	}
    std::istream& operator>>(std::istream& cin, Checked& c) noexcept {
        cin >> c.integer.value;
        return cin;
    }
}