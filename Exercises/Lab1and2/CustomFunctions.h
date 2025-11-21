// Include guards prevent double inclusion of this header.
// CustomFunctions.h

#ifndef CUSTOMFUNCTIONS_H
#define CUSTOMFUNCTIONS_H

#include <string>   // for std::string
#include <vector>   // for std::vector

struct Point {      // simple struct to store (x, y)
    double x;       // x component
    double y;       // y component
};

bool load_data_csv_xy(const std::string& filename, std::vector<Point>& data);  // loads data
std::vector<double> compute_magnitudes(const std::vector<Point>& data);        // computes |(x,y)|
bool fit_straight_line_least_squares(const std::string& data_file,
                                     const std::string& out_file);             // fits line and writes string

                                     // Save a vector<double> (e.g. magnitudes or x^y values) to a text file
bool save_output(const std::vector<double>& values,
                 const std::string& filename,
                 const std::string& header);

                 // Save straight-line fit parameters and chi^2 stats to a text file
bool save_output(const std::string& filename,
                 double m, double c, double chi2, int N_dof, double chi2_per_dof);
// Overloaded print functions

void print_data(const std::vector<Point>& data, std::size_t N); // print first N (x, y) points
void print_data(const std::vector<double>& values);             // print list of doubles (e.g. magnitudes)
void print_data(const std::string& message);                    // print a single string nicely

bool load_errors_from_file(const std::string& filename,
                           std::vector<double>& sigma_y);

// Declare the chi-squared helper
double compute_chi2(const std::vector<Point>& data,
                    const std::vector<double>& sigma_y,
                    double m, double c);

// Recursive helper - compute x^n for interger n (with no roudning
double power_recursive(double , int n);

// Function to compute x^y with y rounded to nearest whole number
double power_rounding(const Point& p);

void   compute_powers_recursive(const std::vector<Point>& data, std::size_t index);
bool   compute_powers_for_file(const std::string& data_file);

std::vector<double> compute_powers_values(const std::vector<Point>& data);
#endif // CUSTOMFUNCTIONS_H
