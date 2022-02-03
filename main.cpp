#include <iostream>
#include "checked.hpp"
#include "test.hpp"
int main() {
    normal_calc_test::test();
    assignment_calc_test::test();
    logical_calc_test::test();
    return 0;
}