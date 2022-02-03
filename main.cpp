#include <iostream>
#include "checked.hpp"
int main() {
    checked::Checked a(100), b(200);
    std::cout << a * b << std::endl;
    return 0;
}