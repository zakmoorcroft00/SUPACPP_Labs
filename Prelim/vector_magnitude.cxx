// vector_magnitude.cxx
// Prelim task 2
#include <iostream>
#include <cmath>
#include <iomanip>

int main() {
    const double x = 7.5;
    const double y = 3.4;

    // Hypot looks like a useful function to use here.
    const double mag = std::hypot(x, y);

    std::cout << std::fixed << std::setprecision(4); // 6 Decimal places
    std::cout << "x = " << x << ", y = " << y << '\n';
    std::cout << "Magnitude = " << mag << '\n';
    return 0;
}
