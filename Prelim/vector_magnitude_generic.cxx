// vector_magnitude_generic.cxx
// Prelim question 3: Take a user input of two numbers to compute a magnitude.

#include <iostream>
#include <cmath>
#include <iomanip>
#include <type_traits>

template <typename T, typename U>
double magnitude2d(T x, U y) {
    static_assert(std::is_arithmetic_v<T> && std::is_arithmetic_v<U>, "magnitude2D requires arithmetic arguments");
    return std::hypot(static_cast<double>(x), static_cast<double>(y));
}

int main() {
    std::cout << std::fixed << std::setprecision(4);

    // Need user to input their numbers
    std::cout << "Enter your x component: ";
    double x{};
    if (!(std::cin >> x)) {
        std::cerr << "Invalid input for x. \n";
        return 1;
    }

    std::cout <<"Enter y compnent: ";
    double y{};
    if (!(std::cin >> y)) {
        std::cerr << "Invalid input for y. \n";
        return 1;
    }
    const double mag_user = magnitude2d(x, y);
    std::cout << "Magnitude = " << mag_user << "\n";

    // Now need to verify task 2 using the same fucnction.
    const double x_ref = 7.5;
    const double y_ref = 3.4;
    const double mag_ref = magnitude2d(x_ref, y_ref);

    std::cout << "\nVerification (Task 2 values):\n";
    std::cout << "x = " << x_ref << ", y = " << y_ref
              << " => Magnitude = " << mag_ref << "\n";
    return 0;
}
git add hello_world.cxx vector_magnitude.cxx vector_magnitude_generic.cxx
git commit -m "Prelim tasks 1–3: Hello World, fixed (7.5,3.4) magnitude, generic 2D magnitude with user input + verification"
git push
