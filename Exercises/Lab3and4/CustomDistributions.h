#pragma once                          // make sure it's only included once

#include <string>                     // for std::string
#include "FiniteFunctions.h"          // base class declaration

// Normal Gaussian dist
class NormalFunction : public FiniteFunction {
public:
    // Constructor: set range, parameters, and output name
    NormalFunction(double range_min,
                   double range_max,
                   double mu,
                   double sigma,
                   const std::string &outfile);

    // Override the base-class function evaluator
    double callFunction(double x) override;

    // Override printInfo to also print meand and sig
    void printInfo() override;

private:
    double m_mu;                      // mean 
    double m_sigma;                   // standard deviation 
};

// Cuachy-Lorentz dist
class CauchyLorentzFunction : public FiniteFunction {
public:
    CauchyLorentzFunction(double range_min,
                          double range_max,
                          double x0,
                          double gamma,
                          const std::string &outfile);

    double callFunction(double x) override;
    void printInfo() override;

private:
    double m_x0;                      // location parameter x0
    double m_gamma;                   // scale parameter γ (> 0)
};

// Crystal ball dist (never heard of this)
class CrystalBallFunction : public FiniteFunction {
public:
    CrystalBallFunction(double range_min,
                        double range_max,
                        double mean,
                        double sigma,
                        double alpha,
                        double n,
                        const std::string &outfile);

    double callFunction(double x) override;
    void printInfo() override;

private:
    // Shape parameters
    double m_mean;                    // central value ("x̄")
    double m_sigma;                   // width σ
    double m_alpha;                   // tail threshold α (> 0)
    double m_n;                       // tail power n (> 1)

    // Precomputed constants A, B, C, D, N from the assignment
    double m_A;
    double m_B;
    double m_C;
    double m_D;
    double m_N;

    // Helper to recompute A, B, C, D, N whenever parameters change
    void updateConstants();
};
