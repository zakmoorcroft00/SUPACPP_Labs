/* Skeleton of code:
Open the file as done in AnalyseData_1.cxx
Sort them into a formatting fittinf the file (Matrix?)
Then print this matrix
Have some fail safes and error messages so I know what is going on
Make sure dynamic as will want to generalise later.*/

// Task 2 (CSV-aware): read "input2D float.txt" which is CSV with header "x,y",
// store (x,y) in a dynamic container, then print from the container.

#include <iostream>      // std::cout, std::cerr
#include <fstream>       // std::ifstream
#include <sstream>       // std::istringstream
#include <string>        // std::string
#include <vector>        // std::vector

// Simple struct to hold one (x,y) point
struct Point {           // define a tiny record for clarity
    double x;            // x value
    double y;            // y value
};

int main(int argc, char* argv[]) {                             // program entry; accepts optional filename
    const std::string default_file = "input2D_float.txt";      // default .txt name note the _
    const std::string filename = (argc > 1) ? argv[1]          // if a path is passed, use it
                                            : default_file;    // otherwise use the default

    std::ifstream in(filename);                                // open the file for reading
    if (!in) {                                                 // check it opened successfully
        std::cerr << "Error: could not open file: " << filename << '\n'
                  << "Hint: run from the folder with the file, or pass its path.\n";
        return 1;                                              // non-zero = failure
    }

    std::vector<Point> data;                                   // dynamic container for unknown number of rows
    data.reserve(1024);                                        // optional: pre-allocate to reduce reallocations

    std::string line;                                          // buffer for each line
    std::size_t line_no = 0;                                   // 1-based line counter for diagnostics

    // --- Optional: detect and skip header if present ---
    if (std::getline(in, line)) {                              // read first line to check for header
        ++line_no;                                             // increment line counter
        if (!line.empty() && line.back() == '\r')              // strip trailing CR if CRLF line endings
            line.pop_back();                                   // remove '\r'
        if (line == "x,y" || line == "X,Y") {                  // if it's a header like "x,y"
            // do nothing: we simply skip it and continue
        } else {                                               // not a header; parse it as data
            std::istringstream iss(line);                      // set up a parser for this line
            double x = 0.0, y = 0.0;                           // placeholders for numbers
            char comma = 0;                                     // placeholder to consume the comma
            if (iss >> x >> comma >> y && comma == ',') {      // parse "x,y" with a literal comma
                data.push_back(Point{x, y});                   // store the parsed point
            } else {                                           // if parsing failed, warn and carry on
                std::cerr << "Warning: could not parse (x, y) on line "
                          << line_no << ": \"" << line << "\"\n";
            }
        }
    }

    while (std::getline(in, line)) {                           // read subsequent lines
        ++line_no;                                             // bump line counter
        if (!line.empty() && line.back() == '\r')              // handle possible CR at end
            line.pop_back();                                   // remove '\r'

        if (line.empty())                                      // skip blank lines
            continue;                                          // continue to next line

        // Allow for spaces around the comma by using stream extraction
        std::istringstream iss(line);                          // parser for this line
        double x = 0.0, y = 0.0;                               // parsed values
        char comma = 0;                                        // to hold the comma
        if (iss >> x >> comma >> y && comma == ',') {          // parse "x,y"
            data.push_back(Point{x, y});                       // append to container
        } else {                                               // if not strictly "x,y", try a fallback
            iss.clear();                                       // clear fail state
            iss.str(line);                                     // reset contents
            if (iss >> x >> y) {                               // try whitespace-separated as a fallback
                data.push_back(Point{x, y});                   // store parsed values
            } else {                                           // still failed: warn and skip
                std::cerr << "Warning: could not parse (x, y) on line "
                          << line_no << ": \"" << line << "\"\n";
            }
        }
    }

    if (!in.eof()) {                                           // if we didn’t end at EOF, note a read issue
        std::cerr << "Warning: input stream ended unexpectedly before EOF.\n";
    }

    // Output: Printing from container and not from raw file
    std::cout << "Read " << data.size() << " (x, y) points:\n"; // report count
    for (std::size_t i = 0; i < data.size(); ++i) {            // iterate over stored points
        const Point& p = data[i];                              // const reference to current point
        std::cout << i + 1 << ": (" << p.x << ", " << p.y << ")\n"; // print index and values
    }

    return 0;                                                  // success
}

