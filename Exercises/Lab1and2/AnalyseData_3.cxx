/* Skeleton of code:
Take task 2 and move reading and printing functionality in two functions
I.e. my main from 2 needs to be two different fucntions
Now need functionality where the user can specify the amount of lines
I already have some error checking
Need to check edges, if N > total, print a message saying you can't have that many or something
*/

// Task 3: Basically Task 2 with some edge functionality and they are able to define how many lines they want.

#include <iostream>      // std::cout, std::cerr
#include <fstream>       // std::ifstream
#include <sstream>       // std::istringstream
#include <string>        // std::string
#include <vector>        // std::vector
#include <limits>        // std::numeric_limits for input clearing

// Simple struct to hold one (x,y) point
struct Point {           // define a tiny record for clarity
    double x;            // x value
    double y;            // y value
};

// Reading function, new to this to task
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

int main(int argc, char* argv[]) {                              // program entry; accepts filename and optional N
    const std::string default_file = "input2D_float.txt";       // default .txt name (with underscore)
    const std::string filename = (argc > 1) ? argv[1]           // if a path is passed, use it
                                            : default_file;     // otherwise use the default

    std::vector<Point> data;                                    // dynamic container for unknown number of rows
    data.reserve(1024);                                         // optional: reduce reallocations

    if (!load_data_csv_xy(filename, data)) {                    // call the new reader function
        return 1;                                               // fatal open error already reported
    }

    // Get N from the user (or from argv[2] if provided)
    std::size_t N = 5;                                          // default to 5 if user just hits Enter
    if (argc > 2) {                                             // if N provided on command line…
        std::istringstream ss(argv[2]);                         // parse argv[2] as an integer
        std::size_t tmp = 0;                                    // temp holder
        if (ss >> tmp) N = tmp;                                 // if parse succeeds, use it
    } else {                                                    // otherwise, interactively ask the user
        std::cout << "How many lines should I print? (default 5): "; // prompt
        if (!(std::cin >> N)) {                                 // read N from stdin
            std::cin.clear();                                   // clear fail state
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard bad input
            N = 5;                                              // fall back to default
        }
    }

    print_first_n(data, N);                                     // call the new printer function
    return 0;                                                   // success
}
 