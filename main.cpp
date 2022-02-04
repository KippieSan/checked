#include <iostream>
#include "checked.hpp"
#include "test.hpp"
int main() {
    normal_calc::test();
    assignment_calc::test();
    logical_calc::test();
    overflow::test();
    return 0;
}