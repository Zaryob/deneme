#include "CDeneme.hpp"

#include <iostream>

void CDeneme::Yazdir() const {
    std::cout << "CDeneme::Yazdir cagrildi." << '\n';
}

int CDeneme::Topla(int a, int b) const {
    return a + b;
}
