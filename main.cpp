#include <iostream>
#include "test.hpp"
int main() {
    normal_calc::test();
    assignment_calc::test();
    logical_calc::test();
    overflow::test();
    speed::test();
    return 0;
}