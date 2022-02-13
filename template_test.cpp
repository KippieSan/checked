#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <iomanip>
#include <math.h>
#include <chrono>
#include "template_checked.hpp"
#include "test.hpp"
using namespace checked;
using i64 = std::int64_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
constexpr std::streamsize m = 20;
constexpr bool large_test = true;
constexpr u32 unif = 100000000;
std::random_device rd;
std::mt19937 eng32(rd());
std::mt19937_64 eng64(rd());
i32 get_random_32() { return static_cast<i32>(eng32()); }
i64 get_random_64() { return static_cast<i64>(eng64()); }

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
void output_testcase(Checked<i64>& lhs, Checked<i64>& rhs, Checked<i64>& res, const Op& op) {
        if(!large_test) std::cout << std::setw(m) << lhs << op << std::setw(m) << rhs << Op::equ << std::setw(m) << res;
        return;
}
bool is_correct(const bool condition, std::string& flag) {
    if(condition) {
        if(!large_test) std::cout << " : o";
        return true;
    }
    else {
        if(!large_test) std::cout << " : x";
        flag = "No";
        return false;
    }
}
void testcases(const u32 cases) {
    std::cout << " ( Cases: " << cases << " ) => ";
}
namespace normal_calc {
    constexpr u32 cases = unif;
    std::string flag = "Yes";
    void test() {
        std::cout << "Normal Calculation Test" << std::endl;
        if(large_test) testcases(cases);
        for(u32 i = 0; i < cases; ++i) {
            i64 unchecked_l(get_random_32()), unchecked_r(get_random_32());
            Checked checked_l(unchecked_l), checked_r(unchecked_r);
            Op op = get_random_op();

            Checked checked_res = calc_result(checked_l, checked_r, op);
            output_testcase(checked_l, checked_r, checked_res, op);
            if(!is_correct(checked_res.get_value() == calc_result(unchecked_l, unchecked_r, op) && (checked_res.get_status() != HadOverflowed::Yes), flag)) std::cout << checked_l << op << checked_r << Op::equ << checked_res << std::endl;
            if(!large_test) std::cout << std::endl;
        }
        std::cout << "All results are correct: " << flag << std::endl;
        std::cout << std::endl;
        return;
    }
}
namespace assignment_calc {
    constexpr u32 cases = unif;
    std::string flag = "Yes";
    template <typename T> void calc_result(T& assigned, const T& rhs, const Op& op) {
        switch(op) {
            case Op::add: assigned += rhs; break;
            case Op::sub: assigned -= rhs; break;
            case Op::mul: assigned *= rhs; break;
            case Op::div: assigned /= rhs; break;
            case Op::mod: assigned %= rhs; break;
            case Op::equ: break;
        }
        return;
    }
    void test() {
        std::cout << "Assignment Calculation Test" << std::endl;
        if(large_test) testcases(cases);
        i64 unchecked_assigned(get_random_32());
        Checked checked_assigned(unchecked_assigned);
        for(u32 i = 0; i < cases; ++i) {
            i64 unchecked_rhs(get_random_32());
            Checked checked_rhs(unchecked_rhs);
            Op op = get_random_op();

            if(!large_test) std::cout << "Assign " << op << std::setw(m) << checked_rhs.get_value() << " to " << std::setw(m) << checked_assigned.get_value();

            calc_result(checked_assigned, checked_rhs, op);
            calc_result(unchecked_assigned, unchecked_rhs, op);

            if(!large_test) std::cout << " Assigned = " << std::setw(m) << checked_assigned.get_value();

            if(!is_correct(checked_assigned.get_value() == unchecked_assigned && checked_assigned.get_status() != HadOverflowed::Yes, flag)) std::cout << "Assigned: " << checked_assigned << " Op: " << op << " Rhs: " << checked_rhs << std::endl;

            if(!large_test) std::cout << std::endl;

            while(checked_assigned.get_value() > INT32_MAX || INT32_MIN > checked_assigned.get_value()) checked_assigned /= 10;
            while(unchecked_assigned > INT32_MAX || INT32_MIN > unchecked_assigned) unchecked_assigned /= 10;
        }
        std::cout << "All results are correct: " << flag << std::endl;
        std::cout << std::endl;
        return;
    }
}
namespace logical_calc {
    constexpr u32 cases = unif;
    constexpr u32 p = 10, hit = 4;
    std::string flag = "Yes";
    void test() {
        std::cout << "Logical Operation Test" << std::endl;
        if(large_test) testcases(cases);
        for(u32 i = 0; i < cases; ++i) {
            i64 unchecked_l(get_random_32()), unchecked_r(get_random_32());
            unchecked_r = unchecked_l % p == hit ? unchecked_l : unchecked_r;
            Checked checked_l(unchecked_l), checked_r(unchecked_r);

            if(!large_test) std::cout << "l: " << std::setw(m) << checked_l.get_value() << " r: " << std::setw(m) << checked_r.get_value() << " result: ";

            const bool result = ((checked_l <=> checked_r) == (unchecked_l <=> unchecked_r));
            const auto equal = (checked_l <=> checked_r);
            
            if(!large_test) std::cout << std::boolalpha << result;
            if(!is_correct((checked_l <=> checked_r) == (unchecked_l <=> unchecked_r), flag)) std::cout << checked_l << " <=> " << checked_r << std::endl;
            if(!large_test && equal == std::strong_ordering::equal) std::cout << " equivalent";
            if(!large_test) std::cout << std::endl;
        }
        std::cout << "All results are correct: " << flag << std::endl;
        std::cout << std::endl;
        return;
    }
}
namespace overflow {
    constexpr u32 cases = 10000;
    constexpr u32 continuance = 10000;
    std::string flag = "Yes";
    i64 random_modulo() {
        return std::pow(10, eng32() % 10);
    }
    bool is_overflowing(const i64& unchecked_l, const i64& unchecked_r, const i64& unchecked_result, const Op& op) {
        bool less_than_min = unchecked_result < std::min(unchecked_l, unchecked_r);
        bool greater_than_max = unchecked_result > std::max(unchecked_l, unchecked_r);
        bool divide_by_zero = unchecked_r == 0;
        if(unchecked_l >= 0 && unchecked_r >= 0) return op_divided(op, std::vector<bool>{ less_than_min, false, less_than_min, divide_by_zero, divide_by_zero });
        else if(unchecked_l < 0 && unchecked_r < 0) return op_divided(op, std::vector<bool>{ greater_than_max, false, less_than_min, divide_by_zero, divide_by_zero });
        else return op_divided(op, std::vector<bool>{ false, is_overflowing(unchecked_l, -unchecked_r, unchecked_l + (-unchecked_r), Op::add), greater_than_max, divide_by_zero, divide_by_zero });
    }
    void test() {
        std::cout << "Overflow Test" << std::endl;
        if(large_test) testcases(cases * continuance);
        for(u32 i = 0; i < cases; ++i) {
            if(!large_test) std::cout << "test #" << i << std::endl;
            i64 unchecked_result(0);
            Checked<i64> checked_result(0);
            HadOverflowed hof = HadOverflowed::No;
            for(u32 j = 0; j < continuance; ++j) {
                i64 unchecked_l(get_random_64() / random_modulo()), unchecked_r(get_random_64() / random_modulo());
                Checked checked_l(unchecked_l), checked_r(unchecked_r);
                Op op = get_random_op();
                checked_result += calc_result(checked_l, checked_r, op);
                unchecked_result += calc_result(unchecked_l, unchecked_r, op);
                output_testcase(checked_l, checked_r, checked_result, op);
                if(!large_test) std::cout << " status: " << checked_result.get_status();

                const auto error = [&](const bool is_correct) {
                    if(!is_correct) std::cout << std::setw(m) << checked_l << op << std::setw(m) << checked_r << Op::equ << checked_result << " stat: " << checked_result.get_status() << std::endl;
                    return;
                };

                if(is_overflowing(unchecked_l, unchecked_r, unchecked_result, op)) {
                    error(is_correct(checked_result.get_status() == HadOverflowed::Yes, flag));
                    hof = HadOverflowed::Yes;
                }
                else {
                    if(hof == HadOverflowed::Yes) error(is_correct(checked_result.get_status() == HadOverflowed::Yes, flag));
                }
                if(!large_test) std::cout << std::endl;
                checked_result *= 0;
                unchecked_result *= 0;
            }
        }
        std::cout << "All results are correct: " << flag << std::endl;
        std::cout << std::endl;
        return;
    }
}
namespace speed {
    constexpr bool ratio_test = true;
    constexpr u32 operation = 100000;
    constexpr u32 cases = 100;
    constexpr u32 ratio_cases = 100;
    template <typename T> std::vector<T> query(std::mt19937_64& eng) {
        std::vector<T> all;
        for(u32 i = 0; i < operation; ++i) {
            T lhs = static_cast<i64>(eng()), rhs = static_cast<i64>(eng()), res;
            Op op = op_divided(static_cast<Op>(eng() % 5), std::vector<Op>{ Op::add, Op::sub, Op::mul, Op::div, Op::mod });

            res = op_divided(op, std::vector<T>{ lhs + rhs, lhs - rhs, lhs * rhs, lhs / rhs, lhs % rhs });
            all.push_back(res);
        }
        return all;
    }
    template <typename T> void head(const std::vector<T>& all) {
        std::cout << " head: [";
        for(int i = 0; i < 5; ++i) std::cout << " " << all.at(i);
        std::cout << " ]" << std::endl;
        return;
    }
    void test() {
        double total_i(0), total_c(0);
        double total_ratio(0);
        double low(2000.0), high(-1.0);
        std::cout << "Speed Test ( Cases: " << cases << ", Operation: " << operation << ", Ratio cases: " << ratio_cases << " )" << std::endl;
        auto start = std::chrono::system_clock::now();
        for(u32 r = 0; r < ratio_cases; ++r) {
            for(u32 i = 0; i < cases; ++i) {
                const uint_fast64_t seed = get_random_64();
                std::mt19937_64 eng_i(seed);
                auto start_i = std::chrono::system_clock::now();
                auto&& all_i = query<i64>(eng_i);
                auto end_i = std::chrono::system_clock::now();

                std::mt19937_64 eng_c(seed);
                auto start_c = std::chrono::system_clock::now();
                auto&& all_c = query<Checked<i64>>(eng_c);
                auto end_c = std::chrono::system_clock::now();

                total_i += std::chrono::duration_cast<std::chrono::milliseconds>(end_i - start_i).count();
                total_c += std::chrono::duration_cast<std::chrono::milliseconds>(end_c - start_c).count();
            }
            if(ratio_cases == 1 || r == ratio_cases - 1) {
                std::cout << "Average calculation time (int):     " << total_i / static_cast<double>(cases * ratio_cases) << " (ms)" << std::endl;
                std::cout << "Average calculation time (Checked): " << total_c / static_cast<double>(cases * ratio_cases) << " (ms)" << std::endl;
            }
            auto ratio = (total_c / static_cast<double>(cases)) / (total_i / static_cast<double>(cases));
            low = std::min(ratio, low);
            high = std::max(ratio, high);
            total_ratio += ratio;
        }
        auto end = std::chrono::system_clock::now();
        std::cout << "Average Checked : int ratio: " << total_ratio / ratio_cases << " ( lowest: " << low << ", highest: " << high << " )" << std::endl;
        std::cout << "Speed test time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " (s)" << std::endl;
        return;
    }
}