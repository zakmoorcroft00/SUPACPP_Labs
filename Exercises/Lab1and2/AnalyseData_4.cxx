/* Skeleton of code:
Take task 3 and add the functionality of one of the prelim tasks
Should be able to bake that code from vector_magnitude.cxx into this
Make sure that it is in a dynamic format as porevious
*/

// Task 4: Task 3 + adding calculating the magnitude of the entries

#include <iostream>      // std::cout, std::cerr
#include <fstream>       // std::ifstream
#include <sstream>       // std::istringstream
#include <string>        // std::string
#include <vector>        // std::vector
#include <limits>        // std::numeric_limits
#include <cmath>         // std::hypot

// Simple struct to hold one (x,y) point
struct Point { double x; double y; };   // Structure as before

// Reading function as before
static bool load_data_csv_xy(                     // returns true on success, false on fatal open/read error
    const std::string& filename,                  // path to CSV file
    std::vector<Point>& data                      // output container (will be appended to)
) {
    std::ifstream in(filename);                   // open the file for reading
    if (!in) {                                    // check it opened successfully
        std::cerr << "Error: could not open file: " << filename << '\n';
        return false;                             // fatal: caller should stop
    }

    std::string line;                             // buffer for each line
    std::size_t line_no = 0;                      // 1-based line counter for diagnostics

    // Reusing the detect the header snippet
    if (std::getline(in, line)) {                 // read first line to check for header
        ++line_no;                                // increment line counter
        if (!line.empty() && line.back() == '\r') // strip trailing CR if CRLF line endings
            line.pop_back();                      // remove '\r'
        if (line == "x,y" || line == "X,Y") {    // if it's a header like "x,y"
            // skip it silently
        } else {                                  // not a header; parse it as data
            std::istringstream iss(line);         // set up a parser for this line
            double x = 0.0, y = 0.0;              // placeholders for numbers
            char comma = 0;                       // placeholder to consume the comma
            if (iss >> x >> comma >> y && comma == ',') { // parse "x,y" with a literal comma
                data.push_back(Point{x, y});      // store the parsed point
            } else {                              
                iss.clear(); iss.str(line);       // fallback: try whitespace-separated
                if (iss >> x >> y) {              // parse "x y"
                    data.push_back(Point{x, y});  // store parsed point
                } else {                          // still failed: warn and continue
                    std::cerr << "Warning: could not parse (x, y) on line "
                              << line_no << ": \"" << line << "\"\n";
                }
            }
        }
    }

    // Go through the remaining lines 
    while (std::getline(in, line)) {              // read subsequent lines
        ++line_no;                                 // bump line counter
        if (!line.empty() && line.back() == '\r')  // handle CR at end
            line.pop_back();                       // remove '\r'
        if (line.empty())                          // skip blank lines
            continue;                              // continue to next line

        std::istringstream iss(line);              // parser for this line
        double x = 0.0, y = 0.0;                   // parsed values
        char comma = 0;                            // to hold the comma
        if (iss >> x >> comma >> y && comma == ',') { // parse "x,y"
            data.push_back(Point{x, y});           // append to container
        } else {                                   
            iss.clear(); iss.str(line);            // fallback: whitespace-separated
            if (iss >> x >> y) {                   // parse "x y"
                data.push_back(Point{x, y});       // append
            } else {                               
                std::cerr << "Warning: could not parse (x, y) on line "
                          << line_no << ": \"" << line << "\"\n";
            }
        }
    }

    if (!in.eof()) {                               // if we didn’t end at EOF, note a read issue
        std::cerr << "Warning: input stream ended unexpectedly before EOF.\n";
        // not fatal; keep what we parsed
    }
    return true;                                    // success (even if some lines were skipped)
}

// Printing function (prints only N lines)
static void print_first_n(                         // prints up to N entries from 'data'
    const std::vector<Point>& data,               // input container
    std::size_t N                                 // number of rows the user requested
) {
    const std::size_t total = data.size();         // total number of points loaded

    if (total == 0) {                              // handle empty dataset
        std::cout << "No data points to print.\n";
        return;                                    // nothing to do
    }

    if (N == 0) {                                  // N=0 → nothing requested
        std::cout << "Requested N=0, nothing to print.\n";
        return;                                    // early exit
    }

    if (N > total) {                               // edge case: user asked for more than available
        std::cerr << "Warning: requested N=" << N
                  << " exceeds total rows (" << total << "). Printing only the first 5.\n";
        N = 5;                                     // per spec: cap at 5 when N is too large
        if (N > total) N = total;                  // safety: if fewer than 5 rows exist, print what we have
    }

    std::cout << "Printing " << N << " of " << total << " (x, y) points:\n"; // header line
    for (std::size_t i = 0; i < N; ++i) {          // print exactly N lines
        const Point& p = data[i];                  // const reference to current point
        std::cout << i + 1 << ": (" << p.x << ", " << p.y << ")\n"; // output row number and values
    }
}

// New for this task: Computes magnitudes for the entire dataset; index i matches data[i].
static std::vector<double> compute_magnitudes(const std::vector<Point>& data) {
    std::vector<double> mags;                             // output container for magnitudes
    mags.reserve(data.size());                            // reserve to avoid reallocations
    for (const auto& p : data) mags.push_back(std::hypot(p.x, p.y));   // hypot(x,y) as before in prelim tasl
    return mags;                                          // return aligned vector of magnitudes
}

// Now need to print magnitudes, keep separate so later can swap this to write them
static void print_magnitudes(const std::vector<double>& mags) {
    if (mags.empty()) { std::cout << "No magnitudes to print.\n"; return; }          // nothing to show
    std::cout << "Magnitudes (" << mags.size() << " values):\n";                      // header
    for (std::size_t i = 0; i < mags.size(); ++i) std::cout << i+1 << ": " << mags[i] << '\n'; // one per line
}

int main(int argc, char* argv[]) {                              // program entry; accepts filename and optional N
    const std::string default_file = "input2D_float.txt";       // default .txt name (with underscore)
    const std::string filename = (argc > 1) ? argv[1]           // if a path is passed, use it
                                            : default_file;     // otherwise use the default

    std::vector<Point> data;                                    // dynamic container for unknown number of rows
    if (!load_data_csv_xy(filename, data)) {                    // call the new reader function
        return 1;                                               // fatal open error already reported
    }

    // Get N from the user (or from argv[2] if provided)
    std::size_t N = 5;                                          // default to 5 if user just hits Enter
    if (argc > 2) { std::istringstream ss(argv[2]); std::size_t t=0; if (ss >> t) N = t; } // parse N if provided
    else { std::cout << "How many lines should I print? (default 5): ";
           if (!(std::cin >> N)) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); N = 5; } } // prompt

    print_first_n(data, N);                                // show first N rows as before

    const auto mags = compute_magnitudes(data);            // compute magnitudes for the full dataset
    print_magnitudes(mags);                                // print all magnitudes (aligned with rows)

    return 0;                                                // success
}
 