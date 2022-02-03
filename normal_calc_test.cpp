#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <iomanip>
#include "checked.hpp"
#include "test.hpp"
using namespace checked;
using i64 = std::int64_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
constexpr u32 cases = 10;
constexpr std::streamsize m = 20;
std::random_device rd;
std::mt19937 eng32(rd());
i32 get_random_32() { return static_cast<i32>(eng32()); }

std::ostream& operator<<(std::ostream& out, const HadOverflowed& hof) {
    if(static_cast<bool>(hof)) out << "HadOverflowed::Yes";
    else out << "HadOverflowed::No ";
    return out;
}
enum class Op : std::size_t { add=0, sub=1, mul=2, div=3, mod=4, equ=5 };
std::size_t to_size(const Op& op) { return static_cast<std::size_t>(op); }
template <typename T> T op_divided(const Op& op, std::vector<T>&& list) {
    switch(op) {
        case Op::add: return list.at(to_size(Op::add));
        case Op::sub: return list.at(to_size(Op::sub));
        case Op::mul: return list.at(to_size(Op::mul));
        case Op::div: return list.at(to_size(Op::div));
        case Op::mod: return list.at(to_size(Op::mod));
        case Op::equ: return list.at(to_size(Op::equ));
    }
    std::exit(8);
}
std::ostream& operator<<(std::ostream& out, const Op& op) {
    out << op_divided(op, std::vector{ " + ", " - ", " * ", " / ", " % ", " = " });
    return out;
}
Op get_random_op() {
    return op_divided(static_cast<Op>(eng32() % 5), std::vector<Op>{ Op::add, Op::sub, Op::mul, Op::div, Op::mod });
}
template <typename T> T calc_result(const T& lhs, const T& rhs, const Op& op) {
    return op_divided(op, std::vector{ lhs + rhs, lhs - rhs, lhs * rhs, lhs / rhs, lhs % rhs });
}
void output_testcase(Checked& lhs, Checked& rhs, Checked& res, const Op& op) {
        std::cout << std::setw(m) << lhs << op << std::setw(m) << rhs << Op::equ << std::setw(m) << res;
        return;
}
namespace normal_calc_test {
    std::string flag = "Yes";
    void test() {
        for(u32 i = 0; i < cases; ++i) {
            i64 unchecked_l(get_random_32()), unchecked_r(get_random_32());
            Checked checked_l(unchecked_l), checked_r(unchecked_r);
            Op op = get_random_op();

            Checked checked_res = calc_result(checked_l, checked_r, op);
            output_testcase(checked_l, checked_r, checked_res, op);
            if(checked_res.get_value() == calc_result(unchecked_l, unchecked_r, op)) std::cout << " : o" << std::endl;
            else {
                std::cout << " : x" << std::endl;
                flag = "No";
            }
        }
        std::cout << "All results are correct: " << flag << std::endl;
        return;
    }
}