// Implementing choice of data loading, printing and magnitude

#include "CustomFunctions.h"   // bring in declarations and Point
#include <iostream>            // for std::cout, std::cerr
#include <fstream>             // for std::ifstream
#include <sstream>             // for std::istringstream
#include <string>              // for std::string
#include <vector>              // for std::vector
#include <cmath>               // for std::hypot and std:;round

bool load_data_csv_xy(const std::string& filename, std::vector<Point>& data) {
    std::ifstream in(filename);                                                     // open file for reading
    if (!in) { std::cerr << "Error: could not open file: " << filename << '\n'; return false; }  // guard open

    std::string line;                                                               // buffer for one line
    std::size_t line_no = 0;                                                        // counter for messages

    if (std::getline(in, line)) {                                                   // read first line (header or data)
        ++line_no;                                                                  // bump line counter
        if (!line.empty() && line.back() == '\r') line.pop_back();                  // strip trailing CR if CRLF
        if (line != "x,y" && line != "X,Y") {                                       // if not a header, parse as data
            std::istringstream iss(line);                                           // make a parser
            double x = 0.0, y = 0.0; char comma = 0;                                // holders
            if (iss >> x >> comma >> y && comma == ',') data.push_back({x, y});     // CSV form
            else { iss.clear(); iss.str(line); if (iss >> x >> y) data.push_back({x, y}); // whitespace fallback
                   else std::cerr << "Warning: could not parse line " << line_no << ": " << line << '\n'; }
        }
    }

    while (std::getline(in, line)) {                                                // parse remaining lines
        ++line_no;                                                                  // increment counter
        if (!line.empty() && line.back() == '\r') line.pop_back();                  // strip CR if present
        if (line.empty()) continue;                                                 // skip empty lines
        std::istringstream iss(line);                                               // parser for this line
        double x = 0.0, y = 0.0; char comma = 0;                                    // holders
        if (iss >> x >> comma >> y && comma == ',') data.push_back({x, y});         // CSV form
        else { iss.clear(); iss.str(line); if (iss >> x >> y) data.push_back({x, y}); // whitespace fallback
               else std::cerr << "Warning: could not parse line " << line_no << ": " << line << '\n'; }
    }

    if (!in.eof()) std::cerr << "Warning: input stream ended unexpectedly before EOF.\n"; // soft warning
    return true;                                                                     // report success
}

bool load_errors_from_file(const std::string& filename,
                           std::vector<double>& sigma_y)
{
    std::vector<Point> tmp;                               // temporary storage for (x, sigma_y)

    if (!load_data_csv_xy(filename, tmp)) {               // reuse existing CSV/whitespace loader
        std::cerr << "Error: could not load errors from file: "
                  << filename << '\n';
        return false;                                     // indicate failure
    }

    sigma_y.clear();                                      // make sure output vector is empty
    sigma_y.reserve(tmp.size());                          // reserve space for speed

    for (const auto& p : tmp) {                           // loop through each (x, y) pair
        sigma_y.push_back(p.y);                           // treat the y value as sigma_y
    }

    return true;                                          // indicate success
}

// void print_first_n(const std::vector<Point>& data, std::size_t N) {
//     const std::size_t total = data.size();                                          // total number of rows
//     if (total == 0) { std::cout << "No data points to print.\n"; return; }          // nothing to show
//     if (N == 0)   { std::cout << "Requested N=0, nothing to print.\n"; return; }    // explicit no-op
//     if (N > total) {                                                                
//         std::cerr << "Warning: requested N=" << N << " exceeds total (" << total
//                   << "). Printing only the first 5.\n";                             // required message
//         N = std::min<std::size_t>(5, total);                                        // cap at 5 (or fewer if <5 exist)
//     }
//     std::cout << "Printing " << N << " of " << total << " (x, y) points:\n";        // header
//     for (std::size_t i = 0; i < N; ++i) {                                           // loop N rows
//         const Point& p = data[i];                                                   // alias row i
//         std::cout << i + 1 << ": (" << p.x << ", " << p.y << ")\n";                 // print index and values
//     } // Needs changing to overload!
// }

std::vector<double> compute_magnitudes(const std::vector<Point>& data) {
    std::vector<double> mags;                                                       // output container
    mags.reserve(data.size());                                                      // avoid reallocations
    for (const auto& p : data) mags.push_back(std::hypot(p.x, p.y));                // hypot = sqrt(x^2 + y^2)
    return mags;                                                                    // return aligned vector
}

// void print_magnitudes(const std::vector<double>& mags) {
//     if (mags.empty()) { std::cout << "No magnitudes to print.\n"; return; }         // nothing to show
//     std::cout << "Magnitudes (" << mags.size() << " values):\n";                    // header
//     for (std::size_t i = 0; i < mags.size(); ++i) std::cout << i + 1 << ": " << mags[i] << '\n'; // one per line
// } // Needs changing to overload! Will comment out for now

double compute_chi2(const std::vector<Point>& data,
                    const std::vector<double>& sigma_y,
                    double m, double c)
{
    const std::size_t n = data.size();             // number of data points
    double chi2 = 0.0;                             // accumulator for chi-squared

    for (std::size_t i = 0; i < n; ++i) {          // loop over each point
        const double x     = data[i].x;            // current x_i
        const double y     = data[i].y;            // current y_i (measured)
        const double sigma = sigma_y[i];           // corresponding σ_i

        if (sigma <= 0.0) {                        // guard against bad errors
            std::cerr << "Chi^2 warning: non-positive sigma at point "
                      << i << " (sigma = " << sigma << ")\n";
            continue;                              // skip this point in chi^2
        }

        const double y_fit = m * x + c;            // fitted y at this x
        const double resid = y - y_fit;            // residual y_i - y_fit

        chi2 += (resid * resid) / (sigma * sigma); // add (resid^2 / σ_i^2)
    }

    return chi2;                                   // return total chi^2
}
// Modified least squares fit: now also computes chi^2 / N_dof
bool fit_straight_line_least_squares(const std::string& data_file,
                                     const std::string& out_file)
{
    std::vector<Point> data;                                   // vector to hold (x,y) points

    if (!load_data_csv_xy(data_file, data)) {                  // try to read the data file using existing loader
        std::cerr << "Fit error: could not load data from "    // print a clear message if reading failed
                  << data_file << '\n';
        return false;                                          // indicate failure
    }

    if (data.size() < 2) {                                     // we need at least two points to fit a line
        std::cerr << "Fit error: need at least 2 points, got " // warn if there are not enough points
                  << data.size() << '\n';
        return false;                                          // indicate failure
    }

    // read the errors from error2D_float.txt using our helper
    std::vector<double> sigma_y;                               // stores error for each y_i
    const std::string error_file = "error2D_float.txt";        // name of the error file

    if (!load_errors_from_file(error_file, sigma_y)) {         // use helper to read σ_i
        std::cerr << "Fit error: could not load errors from "
                  << error_file << '\n';
        return false;                                          // indicate failure
    }

    if (sigma_y.size() != data.size()) {                       // make sure we have one σ_i per point
        std::cerr << "Fit error: number of errors ("
                  << sigma_y.size() << ") does not match number of points ("
                  << data.size() << ").\n";
        return false;                                          // indicate failure
    }

    const std::size_t n = data.size();                         // number of data points as a size_t
    double sum_x  = 0.0;                                       // sum of x values
    double sum_y  = 0.0;                                       // sum of y values
    double sum_xx = 0.0;                                       // sum of x^2 values
    double sum_xy = 0.0;                                       // sum of x*y values

    for (const auto& p : data) {                               // loop over every (x, y) point
        sum_x  += p.x;                                         // accumulate x
        sum_y  += p.y;                                         // accumulate y
        sum_xx += p.x * p.x;                                   // accumulate x^2
        sum_xy += p.x * p.y;                                   // accumulate x*y
    }

    const double n_d = static_cast<double>(n);                 // cast n to double for formula
    const double denom = n_d * sum_xx - sum_x * sum_x;         // denominator in least squares formulas

    if (denom == 0.0) {                                        // check for possible divide-by-zero
        std::cerr << "Fit error: denominator is zero. "        // warn that points are degenerate
                  << "Perhaps all x values are the same?\n";
        return false;                                          // indicate failure
    }

    const double m = (n_d * sum_xy - sum_x * sum_y) / denom;   // slope m from least squares
    const double c = (sum_y * sum_xx - sum_x * sum_xy) / denom;// intercept c from least squares

    // --- NEW: compute chi^2 and chi^2 / N_dof ---
    const double chi2   = compute_chi2(data, sigma_y, m, c);   // total chi-squared for the fit
    const int    N_dof  = static_cast<int>(n) - 2;             // degrees of freedom: N - number_of_params (2)

    if (N_dof <= 0) {                                          // sanity check: should be positive
        std::cerr << "Fit error: non-positive degrees of freedom (N_dof = "
                  << N_dof << ").\n";
        return false;                                          // indicate failure
    }

    const double chi2_per_dof = chi2 / static_cast<double>(N_dof); // chi^2 divided by N_dof

    // --- Build a nice string for the fitted line y = m x + c ---
    std::ostringstream ss;                                     // string stream to build a neat output string
    ss.setf(std::ios::fixed);                                  // use fixed-point notation (not scientific)
    ss.precision(4);                                           // print numbers with 4 decimal places
    ss << "y = " << m << " * x + " << c;                       // build the function string "y = m * x + c"

    const std::string result = ss.str();                       // store the final function string

    // --- Print everything using your print_data helper ---
    print_data("Least squares fit result:");                   // uses string overload for a header line
    print_data("  " + result);                                 // uses string overload to print the fitted function

    // also print chi^2 information
    std::ostringstream ss_stats;                               // second stream for stats line
    ss_stats.setf(std::ios::fixed);
    ss_stats.precision(4);
    ss_stats << "  chi^2 = " << chi2
             << ",  N_dof = " << N_dof
             << ",  chi^2/N_dof = " << chi2_per_dof;

    print_data(ss_stats.str());
    
    // print stats line via print_data

    // // --- Save results to a file ---
    // std::ofstream out(out_file);                               // open an output file to save the info
    // if (!out) {                                                // check that the file opened correctly
    //     std::cerr << "Fit warning: could not open output file "
    //               << out_file << " for writing.\n";
    //     return false;                                          // treat this as a failure
    // }

    // // header comment line (optional, helps if you re-use the file later)
    // out << "# Least squares fit for " << data_file << '\n';
    // out << "# y = m * x + c, with chi^2 statistics\n";
    // out << "# m  c  chi2  N_dof  chi2_per_dof\n";

    // // numerical results in one line (same precision as above)
    // out.setf(std::ios::fixed);
    // out.precision(6);
    // out << m << ' ' << c << ' '
    //     << chi2 << ' ' << N_dof << ' '
    //     << chi2_per_dof << '\n';
        // --- Save results to a file using the new save_output function ---
    if (!save_output(out_file, m, c, chi2, N_dof, chi2_per_dof)) {
        return false;                                          // treat failure to write as an error
    }

    return true;                                               // indicate that everything worked
}



// bool fit_straight_line_least_squares(const std::string& data_file, const std::string& out_file){
//     std::vector<Point> data;

//     if (!load_data_csv_xy(data_file, data)) {                  // try to read the data file using existing loader
//         std::cerr << "Fit error: could not load data from "    // print a clear message if reading failed
//                   << data_file << '\n';
//         return false;                                          // indicate failure
//     }

//     if (data.size() < 2) {                                     // we need at least two points to fit a line
//         std::cerr << "Fit error: need at least 2 points, got " // warn if there are not enough points
//                   << data.size() << '\n';
//         return false;                                          // indicate failure
//     }

//     const std::size_t n = data.size();                         // number of data points as a size_t
//     double sum_x  = 0.0;                                       // sum of x values
//     double sum_y  = 0.0;                                       // sum of y values
//     double sum_xx = 0.0;                                       // sum of x^2 values
//     double sum_xy = 0.0;                                       // sum of x*y values

//     for (const auto& p : data) {                               // loop over every (x, y) point
//         sum_x  += p.x;                                         // accumulate x
//         sum_y  += p.y;                                         // accumulate y
//         sum_xx += p.x * p.x;                                   // accumulate x^2
//         sum_xy += p.x * p.y;                                   // accumulate x*y
//     }

//     const double n_d = static_cast<double>(n);                 // cast n to double for formula
//     const double denom = n_d * sum_xx - sum_x * sum_x;         // denominator in least squares formulas

//     if (denom == 0.0) {                                        // check for possible divide-by-zero
//         std::cerr << "Fit error: denominator is zero. "        // warn that points are degenerate
//                   << "Perhaps all x values are the same?\n";
//         return false;                                          // indicate failure
//     }

//     const double m = (n_d * sum_xy - sum_x * sum_y) / denom;   // slope m from least squares
//     const double c = (sum_y * sum_xx - sum_x * sum_xy) / denom;// intercept c from least squares

//     std::ostringstream ss;                                     // string stream to build a neat output string
//     ss.setf(std::ios::fixed);                                  // use fixed-point notation (not scientific)
//     ss.precision(4);                                           // print numbers with 4 decimal places
//     ss << "y = " << m << " * x + " << c;                       // build the function string "y = m * x + c"

//     const std::string result = ss.str();                       // store the final function string

//     // std::cout << "Least squares fit result:\n";                // print a label to the terminal
//     // std::cout << "  " << result << '\n';                       // print the function itself

//     print_data("Least squares fit result:");                      // uses string overload for a header line
//     print_data("  " + result);                                    // uses string overload to print the fitted function


//     std::ofstream out(out_file);                               // open an output file to save the string
//     if (!out) {                                                // check that the file opened correctly
//         std::cerr << "Fit warning: could not open output file "// warn if we cannot write the file
//                   << out_file << " for writing.\n";
//         return false;                                          // treat this as a failure
//     }

//     out << result << '\n';                                     // write the function string to the file
//     return true;                                               // indicate that everything worked
// }



// Overload 1: Printing first n entries

void print_data(const std::vector<Point>& data, std::size_t N) {
    const std::size_t total = data.size();                      // total number of points available

    if (total == 0) {                                           // check if there is any data
        std::cout << "No data points to print.\n";              // tell user there is nothing to print
        return;                                                 // exit the function early
    }

    if (N == 0) {                                               // check if user requested 0 lines
        std::cout << "Requested N=0, nothing to print.\n";      // explain that nothing will be shown
        return;                                                 // exit early
    }

    if (N > total) {                                            // handle case where N is too large
        std::cerr << "Warning: requested N=" << N               // warn that N exceeds available data
                  << " exceeds total (" << total
                  << "). Printing only the first 5.\n";
        N = std::min<std::size_t>(5, total);                    // follow the spec: cap at 5 (or fewer)
    }

    std::cout << "Printing " << N << " of " << total
              << " (x, y) points:\n";                           // header line to explain what follows

    for (std::size_t i = 0; i < N; ++i) {                       // loop over first N points
        const Point& p = data[i];                               // reference to current point
        std::cout << i + 1 << ": (" << p.x                      // print 1-based index and x value
                  << ", " << p.y << ")\n";                      // print y value and newline
    }
}

// Overload 2: print a list of doubles (e.g. magnitudes)
void print_data(const std::vector<double>& values) {
    if (values.empty()) {                                       // check if there is anything to print
        std::cout << "No values to print.\n";                   // message for empty vector
        return;                                                 // exit early
    }

    std::cout << "Printing " << values.size()
              << " values:\n";                                  // header showing how many values

    for (std::size_t i = 0; i < values.size(); ++i) {           // loop over all values
        std::cout << i + 1 << ": " << values[i] << '\n';        // print 1-based index and the value
    }
}

// Overload 3: print a single message string (e.g. fitted function)
void print_data(const std::string& message) {
    std::cout << message << '\n';                               // just print the message and newline
}

// Recursive helper that assumes n is an integer component
double power_recursive(double x, int n) {
    if (n == 0) {
        return 1.0; // Base case: x^0 = 1
    } 
    
    if (n < 0) {
        return 1.0 / power_recursive(x, -n);                    // Handles negative exponents
    }

    if(n % 2 == 0) {
        double half = power_recursive(x, n / 2);         // Recursive call for half the exponent
        return half * half;                         // Square the result for even n
    } else {
        return x * power_recursive(x, n - 1);                   // Multiply by x for odd n
    }
}
        
 // Wrapper that rounds y and calls the recursive helper
double power_rounding(const Point& p)
{
    int n = static_cast<int>(std::round(p.y));   // round y
    return power_recursive(p.x, n);              // compute x^n
}

void compute_powers_recursive(const std::vector<Point>& data,
                              std::size_t index)
{
    if (index >= data.size()) {                 // base case: past the end
        return;
    }

    const Point& p = data[index];               // current data point (x, y)

    double value = power_rounding(p);  // compute x^y with y rounded

    // format a nice line: e.g. "x, y -> x^y = value"
    std::ostringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(4);
    ss << "Point " << index
       << ": x = " << p.x
       << ", y = " << p.y
       << ", x^y (y rounded) = " << value;

    print_data(ss.str());                       // your existing print_data(string)

    // recursive step: process the next element
    compute_powers_recursive(data, index + 1);
}

bool compute_powers_for_file(const std::string& data_file)
{
    std::vector<Point> data;                            // hold (x, y) points

    if (!load_data_csv_xy(data_file, data)) {          // reuse your loader
        std::cerr << "Error: could not load data from "
                  << data_file << '\n';
        return false;
    }

    if (data.empty()) {                                // nothing to do
        std::cerr << "Error: no data points in "
                  << data_file << '\n';
        return false;
    }

    print_data("Computing x^y (with y rounded) for each data point:");

    compute_powers_recursive(data, 0);                 // start recursion at index 0

    return true;                                       // everything worked
}

// --- New generic "save_output" overload: save a vector<double> to a file ---
bool save_output(const std::vector<double>& values,
                 const std::string& filename,
                 const std::string& header)
{
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Error: could not open output file " << filename << " for writing.\n";
        return false;
    }

    if (!header.empty()) {                           // optional header line
        out << "# " << header << '\n';
    }

    for (std::size_t i = 0; i < values.size(); ++i) {
        out << i << ' ' << values[i] << '\n';        // index and value
    }
    return true;
}

// --- New "save_output" overload: save straight-line fit + chi^2 stats ---
bool save_output(const std::string& filename,
                 double m, double c, double chi2, int N_dof, double chi2_per_dof)
{
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Fit error: could not open output file " << filename << " for writing.\n";
        return false;
    }

    out << "# Least squares fit with chi^2 statistics\n";
    out << "# m  c  chi2  N_dof  chi^2_per_dof\n";
    out.setf(std::ios::fixed);
    out.precision(6);
    out << m << ' ' << c << ' '
        << chi2 << ' ' << N_dof << ' '
        << chi2_per_dof << '\n';

    return true;
}

// --- Helper to compute x^y (y rounded) for all points and return as vector<double> ---
std::vector<double> compute_powers_values(const std::vector<Point>& data)
{
    std::vector<double> values;
    values.reserve(data.size());
    for (const auto& p : data) {
        values.push_back(power_rounding(p));   // uses recursive power_recursive internally
    }
    return values;
}
