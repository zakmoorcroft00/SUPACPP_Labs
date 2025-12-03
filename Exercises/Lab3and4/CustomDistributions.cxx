

#include <iostream>          
#include <fstream>           
#include <vector>            
#include <string>           
#include <limits>           
#include <algorithm>         
#include <cmath>            
#include "CustomDistributions.h"   

// Normal function

// Constructor: pass range + outfile to base class, then store meand and sig
NormalFunction::NormalFunction(double range_min,
                               double range_max,
                               double mu,
                               double sigma,
                               const std::string &outfile)
    : FiniteFunction(range_min, range_max, outfile),
      m_mu(mu),
      m_sigma(sigma)
{}

// Evaluate normal pdf at x
double NormalFunction::callFunction(double x) {
    const double inv_sqrt_2pi = 1.0 / std::sqrt(2.0 * M_PI); 
    double z = (x - m_mu) / m_sigma;                         
    double exponent = -0.5 * z * z;                          
    return (inv_sqrt_2pi / m_sigma) * std::exp(exponent);    }

// Print base info (range, integral, name) + meand and sig
void NormalFunction::printInfo() {
    FiniteFunction::printInfo();                            
    std::cout << "  Normal parameters: mu = " << m_mu
              << ", sigma = " << m_sigma << std::endl;
}

// Cauchy-Lorenntz

CauchyLorentzFunction::CauchyLorentzFunction(double range_min,
                                             double range_max,
                                             double x0,
                                             double gamma,
                                             const std::string &outfile)
    : FiniteFunction(range_min, range_max, outfile),
      m_x0(x0),
      m_gamma(gamma)
{}

// Evaluate Cauchy-Lorentz pdf at x
double CauchyLorentzFunction::callFunction(double x) {
    double t = (x - m_x0) / m_gamma;                        
    double denom = M_PI * m_gamma * (1.0 + t * t);          
    return 1.0 / denom;
}

void CauchyLorentzFunction::printInfo() {
    FiniteFunction::printInfo();
    std::cout << "  Cauchy-Lorentz parameters: x0 = " << m_x0
              << ", gamma = " << m_gamma << std::endl;
}

// Crystal ball

// Helper to precompute A, B, C, D, N based on current parameters
void CrystalBallFunction::updateConstants() {
    double absAlpha = std::fabs(m_alpha);                    
    double a2_over_2 = 0.5 * absAlpha * absAlpha; 

    // From assignment info
    m_A = std::pow(m_n / absAlpha, m_n) * std::exp(-a2_over_2);

    // B 
    m_B = m_n / absAlpha - absAlpha;

    // C 
    m_C = (m_n / absAlpha) * (1.0 / (m_n - 1.0)) * std::exp(-a2_over_2);

    // D 
    m_D = std::sqrt(M_PI / 2.0) * (1.0 + std::erf(absAlpha / std::sqrt(2.0)));

    // N 
    m_N = 1.0 / (m_sigma * (m_C + m_D));
}

// Constructor: pass range + outfile, then store params and compute constants
CrystalBallFunction::CrystalBallFunction(double range_min,
                                         double range_max,
                                         double mean,
                                         double sigma,
                                         double alpha,
                                         double n,
                                         const std::string &outfile)
    : FiniteFunction(range_min, range_max, outfile),
      m_mean(mean),
      m_sigma(sigma),
      m_alpha(alpha),
      m_n(n)
{
    updateConstants();                                       // precompute A,B,C,D,N
}

// // Evaluate Crystal Ball pdf at x (piecewise)
// double CrystalBallFunction::callFunction(double x) {
//     double t = (x - m_mean) / m_sigma;                       // (x - x̄)/σ

//     if (t > -m_alpha) {
//         return m_N * std::exp(-0.5 * t * t);
//     } else {
//         return m_N * m_A * std::pow(m_B - t, -m_n);
//     }
// }

double CrystalBallFunction::callFunction(double x) {
    // t = (x - mean)/sigma as usual
    double t = (x - m_mean) / m_sigma;

    // Use |alpha| for the threshold, but let the sign decide the alpha
    double absAlpha = std::fabs(m_alpha);

    // Effective t: for alpha > 0, use t as-is (left tail);
    // for alpha < 0, mirror t -> -t (right tail).
    double tEff = (m_alpha >= 0.0) ? t : -t;

    if (tEff > -absAlpha) {
        // Gaussian core
        return m_N * std::exp(-0.5 * tEff * tEff);
    } else {
        // Power-law tail
        return m_N * m_A * std::pow(m_B - tEff, -m_n);
    }
}

void CrystalBallFunction::printInfo() {
    FiniteFunction::printInfo();
    std::cout << "  CrystalBall parameters: mean = " << m_mean
              << ", sigma = " << m_sigma
              << ", alpha = " << m_alpha
              << ", n = " << m_n << std::endl;
}

// Now do main (and use logic from assignment one)

// Helper: load data from file into a vector<double> and determine range
static bool load_data_and_range(const std::string &filename,
                                std::vector<double> &data,
                                double &rmin,
                                double &rmax)
{
    std::ifstream infile(filename);                           // open file
    if (!infile.is_open()) {
        std::cerr << "Error: could not open data file: "
                  << filename << std::endl;
        return false;
    }

    data.clear();
    double x = 0.0;
    while (infile >> x) {                                     // read all numbers
        data.push_back(x);
    }
    infile.close();

    if (data.empty()) {
        std::cerr << "Error: no data read from " << filename << std::endl;
        return false;
    }

    // Compute min/max of data
    double xmin = std::numeric_limits<double>::max();
    double xmax = std::numeric_limits<double>::lowest();
    for (double v : data) {
        if (v < xmin) xmin = v;
        if (v > xmax) xmax = v;
    }

    double range = xmax - xmin;
    double padding = 0.1 * range;                             // 10% padding

    rmin = xmin - padding;
    rmax = xmax + padding;

    return true;
}

int main(int argc, char **argv)
{
    // Decide which file to use
    std::string data_file = "Outputs/data/MysteryData23232.txt"; // default
    if (argc > 1) {                                             // optional override from CLI
        data_file = argv[1];
    }

    std::vector<double> data_points;
    double rmin = 0.0;
    double rmax = 0.0;
    if (!load_data_and_range(data_file, data_points, rmin, rmax)) {
        return 1;                                               // bail out if failed
    }

    int Nbins = 50;                                             // histogram bins
    int Nint  = 2000;                                           // integral sampling points

    // Normal example
    {
        double mu    = 0.0;                                     // choose mean
        double sigma = 1.0;                                     // choose sig
        NormalFunction g(rmin, rmax, mu, sigma, "NormalTest");  // ctor

        g.plotFunction();                                      // scan + flag for plotting
        g.plotData(data_points, Nbins, true);                  // add data histogram
        g.integral(Nint);                                      // compute & cache integral
        g.printInfo();                                         // print info + integral

        // When this block ends, destructor is called and the plot
        // Outputs/png/NormalTest.png is written.
    }

    // Cauchy-Lorentz example
    {
        double x0    = 0.0;                                     // centre
        double gamma = 1.0;                                     // width parameter
        CauchyLorentzFunction c(rmin, rmax, x0, gamma, "CauchyTest");

        c.plotFunction();
        c.plotData(data_points, Nbins, true);
        c.integral(Nint);
        c.printInfo();
        // Plot written as Outputs/png/CauchyTest.png when c goes out of scope
    }

    // Crystal 
    // Probably use this? Asymmetrry and tail
    {
        double mean  = 1.0;                                     // central value
        double sigma = 2.1;                                     // width
        double alpha = -1.5;                                    // Can now have negative for RHS
        double n     = 2.0;                                     // tail power

        CrystalBallFunction cb(rmin, rmax, mean, sigma, alpha, n,
                               "CrystalBallTest");

        cb.plotFunction();
        cb.plotData(data_points, Nbins, true);
        cb.integral(Nint);
        cb.printInfo();

        // Metroplois algorithm here
        int Nsamples     = static_cast<int>(data_points.size()); // aim for same number of pseudo-points as real data
        int burnIn       = 2000;                                 // number of initial steps to discard
        int thin         = 10;                                   // only keep 1 in every 'thin' steps
        double propSigma = 0.5;                                  // proposal step size (tune if needed)

        // Run the Metropolis algorithm to generate pseudo-data
        std::vector<double> sampled_points =
            cb.sampleMetropolis(Nsamples, burnIn, thin, propSigma);

        // Add the sampled data to the plot; isdata = false means "sampled" => blue points in the PNG
        cb.plotData(sampled_points, Nbins, false);

        // When leavingthis block, cb's destructor runs and writes Outputs/png/CrystalBallTest.png
    }

    return 0;                                                   // success
}
git add Makefile
git add CustomDistributions.cxx
git add FiniteFunctions.h
git add FiniteFunctions.cxx
git add CalculatePi.cxx
git add README
git add MysteryData23232.txt

git commit -m "***ASSIGNMENT 2 SUBMISSION***"