// Implementing choice of data loading, printing and magnitude

#include "CustomFunctions.h"   // bring in declarations and Point
#include <iostream>            // for std::cout, std::cerr
#include <fstream>             // for std::ifstream
#include <sstream>             // for std::istringstream
#include <string>              // for std::string
#include <vector>              // for std::vector
#include <cmath>               // for std::hypot

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

void print_first_n(const std::vector<Point>& data, std::size_t N) {
    const std::size_t total = data.size();                                          // total number of rows
    if (total == 0) { std::cout << "No data points to print.\n"; return; }          // nothing to show
    if (N == 0)   { std::cout << "Requested N=0, nothing to print.\n"; return; }    // explicit no-op
    if (N > total) {                                                                
        std::cerr << "Warning: requested N=" << N << " exceeds total (" << total
                  << "). Printing only the first 5.\n";                             // required message
        N = std::min<std::size_t>(5, total);                                        // cap at 5 (or fewer if <5 exist)
    }
    std::cout << "Printing " << N << " of " << total << " (x, y) points:\n";        // header
    for (std::size_t i = 0; i < N; ++i) {                                           // loop N rows
        const Point& p = data[i];                                                   // alias row i
        std::cout << i + 1 << ": (" << p.x << ", " << p.y << ")\n";                 // print index and values
    }
}

std::vector<double> compute_magnitudes(const std::vector<Point>& data) {
    std::vector<double> mags;                                                       // output container
    mags.reserve(data.size());                                                      // avoid reallocations
    for (const auto& p : data) mags.push_back(std::hypot(p.x, p.y));                // hypot = sqrt(x^2 + y^2)
    return mags;                                                                    // return aligned vector
}

void print_magnitudes(const std::vector<double>& mags) {
    if (mags.empty()) { std::cout << "No magnitudes to print.\n"; return; }         // nothing to show
    std::cout << "Magnitudes (" << mags.size() << " values):\n";                    // header
    for (std::size_t i = 0; i < mags.size(); ++i) std::cout << i + 1 << ": " << mags[i] << '\n'; // one per line
}

bool fit_straight_line_least_squares(const std::string& data_file, const std::string& out_file){
    std::vector<Point> data;

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

    std::ostringstream ss;                                     // string stream to build a neat output string
    ss.setf(std::ios::fixed);                                  // use fixed-point notation (not scientific)
    ss.precision(4);                                           // print numbers with 4 decimal places
    ss << "y = " << m << " * x + " << c;                       // build the function string "y = m * x + c"

    const std::string result = ss.str();                       // store the final function string

    std::cout << "Least squares fit result:\n";                // print a label to the terminal
    std::cout << "  " << result << '\n';                       // print the function itself

    std::ofstream out(out_file);                               // open an output file to save the string
    if (!out) {                                                // check that the file opened correctly
        std::cerr << "Fit warning: could not open output file "// warn if we cannot write the file
                  << out_file << " for writing.\n";
        return false;                                          // treat this as a failure
    }

    out << result << '\n';                                     // write the function string to the file
    return true;                                               // indicate that everything worked
}