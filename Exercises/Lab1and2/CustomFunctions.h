// Include guards prevent double inclusion of this header.
#ifndef CUSTOMFUNCTIONS_H
#define CUSTOMFUNCTIONS_H

#include <string>   // for std::string
#include <vector>   // for std::vector

// Small record type to hold one (x, y) point.
struct Point { double x; double y; };

// Reads a CSV file with an optional "x,y" header into 'data'; returns true on success.
bool load_data_csv_xy(const std::string& filename, std::vector<Point>& data);

// Prints the first N rows from 'data', with the required edge-case behaviour.
void print_first_n(const std::vector<Point>& data, std::size_t N);

// Computes |(x,y)| for every point and returns a vector of magnitudes.
std::vector<double> compute_magnitudes(const std::vector<Point>& data);

// Prints every value in 'mags' on its own line.
void print_magnitudes(const std::vector<double>& mags);

// Fits straight line with function declared in CustomFunctions.cxx
bool fit_straight_line_least_squares(const std::string& data_file,
                                     const std::string& out_file);

#endif // CUSTOMFUNCTIONS_H
