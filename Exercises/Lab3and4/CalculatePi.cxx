// In the original MakeFile there is a CalculatePi.cxx
// So I used the same name so I don't have to chnage the Makefile.

#include <iostream>   
#include <random>     
#include <cstdlib>    
#include <cmath>     
#include <iomanip>

int main(int argc, char** argv) {
    // Check that we have exactly two command-line arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <radius> <n_random>\n";
        std::cerr << "Example: " << argv[0]
                  << " 1.0 1000000\n";
        return 1;  // return non-zero on error
    }

    // Convert the first argument to a floating-point radius
    double radius = std::strtod(argv[1], nullptr);

    // Convert the second argument to an integer number of samples
    long long n_random = std::strtoll(argv[2], nullptr, 10);

    // Basic checks on the input values
    if (radius <= 0.0) {
        std::cerr << "Error: radius must be positive.\n";
        return 1;
    }
    if (n_random <= 0) {
        std::cerr << "Error: n_random must be positive.\n";
        return 1;
    }

    // Set up a random number generator for Metropolis
    std::random_device rd;      // seed source
    std::mt19937 gen(rd());     // Mersenne Twister engine

    // Uniform distribution on [-radius, radius] for the starting point
    std::uniform_real_distribution<double> uni(-radius, radius);

    // Choose a proposal width for the Metropolis steps
    double stepSigma = radius * 0.5;          // proposal standard deviation
    std::normal_distribution<double> proposal(0.0, stepSigma);

    // Draw an initial point uniformly inside the square
    double x = uni(gen);                     // initial x_0
    double y = uni(gen);                     // initial y_0

    // Decide on a burn-in length for the chain
    long long burnIn = std::min(10000LL, n_random);  // discard first steps

    // Counters for estimating pi
    long long countInside = 0;               // number of samples inside circle
    long long countTotal  = 0;               // number of samples after burn-in

    // Counters for acceptance rate diagnostics
    long long nAccepted   = 0;               // how many proposals accepted
    long long nProposed   = 0;               // how many proposals made

    // Total number of Metropolis steps (burn-in + samples we care about)
    long long totalSteps = burnIn + n_random;

    // Loop over all Metropolis steps
    for (long long step = 0; step < totalSteps; ++step) {
        // Propose a new point by adding a normal step to current state
        double xTry = x + proposal(gen);
        double yTry = y + proposal(gen);

        // Increment proposal counter
        ++nProposed;

        // Check if proposed point lies inside the square [-R, R] x [-R, R]
        bool insideSquare = (std::fabs(xTry) <= radius) &&
                            (std::fabs(yTry) <= radius);

        if (insideSquare) {
            // f(x,y) is constant inside the square, so f(y)/f(x) = 1
            // We accept any move that stays inside the square
            x = xTry;                        // update current x
            y = yTry;                        // update current y
            ++nAccepted;                     // count this as an accepted move
        } else {
            // Proposed point is outside the allowed region, so reject it
            // x and y remain unchanged
        }

        // After burn-in, start counting samples for the pi estimate
        if (step >= burnIn) {
            // Increment number of post–burn-in samples
            ++countTotal;

            // Check if current point lies inside the circle of radius R
            if (x * x + y * y <= radius * radius) {
                ++countInside;               // count contribution if inside circle
            }
        }
    }

    // Fraction of samples that fell inside the circle
    double fraction = static_cast<double>(countInside)
                    / static_cast<double>(countTotal);

    // Monte Carlo estimate of pi from area ratio
    double piEstimate = 4.0 * fraction;

    // Compute acceptance rate of the Metropolis chain
    double acceptRate = static_cast<double>(nAccepted)
                      / static_cast<double>(nProposed);

    // Print results with high precision
    std::cout << std::setprecision(12);
    std::cout << "Radius:               " << radius      << "\n";
    std::cout << "Requested samples:    " << n_random    << "\n";
    std::cout << "Burn-in steps:        " << burnIn      << "\n";
    std::cout << "Used samples:         " << countTotal  << "\n";
    std::cout << "Points inside circle: " << countInside << "\n";
    std::cout << "Estimated pi:         " << piEstimate  << "\n";
    std::cout << "Acceptance rate:      " << acceptRate  << "\n";

    return 0; // success
}
