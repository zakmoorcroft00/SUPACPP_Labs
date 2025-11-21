/* Skeleton/Idea of code:
Take functions out of here into CustomFunctions.h/cxx
Modify this code so you don't have the fucntions and you call them in
Prompt to user to decide whether they want to do N lines or mag of all points*/

// Task 5+6: Take functions out into .h file and call from there.

/*Now this file is just being used for the rest. Up until this point I did a new
AnalyseData for each separate bit of the assignment. Makes sense to fiddle around with
this as I have a Makefile. */

// Main program: loads (x, y) data and lets the user choose what to do with it.

#include "CustomFunctions.h"               // bring in Point and function declarations
#include <iostream>                        // for std::cout, std::cerr
#include <limits>                          // for std::numeric_limits used in input recovery
#include <string>                          // for std::string
#include <vector>                          // for std::vector

int main(int argc, char* argv[]) {                                            // program entry point
    const std::string default_file = "input2D_float.txt";                     // default input filename
    const std::string filename = (argc > 1) ? argv[1] : default_file;         // use argv[1] if provided, else default

    std::vector<Point> data;                                                  // container to hold all (x, y) rows
    if (!load_data_csv_xy(filename, data)) return 1;                          // load data; exit with error if it fails

    bool keep_running = true;                                                 // control flag for main loop

    while (keep_running) {                                                    // repeat until user chooses to exit
        std::cout << "Choose an option:\n";                                   // show a simple menu
        std::cout << "  1) Print N lines of (x, y)\n";                        // menu option 1
        std::cout << "  2) Compute and print magnitudes for all rows\n";      // menu option 2
        std::cout << "  3) Fit a straight line y = m*x + c\n";                // menu option 3
        std::cout << "  4) Compute x^y (y rounded) for each data point\n";    // menu option 4
        std::cout << "  0) Exit\n";                                           // menu option 0: exit
        std::cout << "Enter 0, 1, 2, 3 or 4: ";                               // prompt user for a choice

        int choice = 0;                                                       // variable to store the chosen option
        if (!(std::cin >> choice)) {                                          // attempt to read the choice
            std::cin.clear();                                                 // clear error flags on failure
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');   // discard bad input
            std::cerr << "Invalid choice.\n";                                 // tell the user input was invalid
            continue;                                                         // restart loop
        }

        switch (choice) {
        case 0:                                                               // user chose to exit
            keep_running = false;
            break;

        case 1: {                                                             // print N lines
            std::size_t N = 5;                                                // default number of lines to print
            std::cout << "How many lines should I print? (default 5): ";      // prompt for N
            if (!(std::cin >> N)) {                                           // try to read N
                std::cin.clear();                                             // clear error flags
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard bad input
                N = 5;                                                        // fall back to default value
            }
            print_data(data, N);                                              // use overloaded print_data for (x, y) + N
            break;
        }

        case 2: {                                                             // magnitudes
            const auto mags = compute_magnitudes(data);                       // compute |(x, y)| for every row
            print_data(mags);                                                 // print magnitudes to terminal

            // save magnitudes to a descriptive file
            if (!save_output(mags, "magnitudes_output.txt",
                             "Magnitudes of (x,y) points")) {
                std::cerr << "Warning: could not save magnitudes_output.txt\n";
            }
            break;
        }

        case 3: {                                                             // straight-line fit
            if (!fit_straight_line_least_squares(filename,                   // input data file name
                                                 "fit_result.txt")) {        // output file name for fit + chi^2
                std::cerr << "Fitting failed.\n";                             // inform the user on failure
            }
            // fit_straight_line_least_squares now calls save_output(...) internally
            break;
        }

        case 4: {                                                             // x^y for each data point
            print_data("Computing x^y (with y rounded) for each data point:");
            compute_powers_recursive(data, 0);                                // recursive print to terminal

            // also save the numerical x^y values to a file
            const auto powers = compute_powers_values(data);
            if (!save_output(powers, "powers_output.txt",
                             "x^y (y rounded) for each (x,y) point")) {
                std::cerr << "Warning: could not save powers_output.txt\n";
            }
            break;
        }

        default:
            std::cerr << "Unknown option.\n";                                 // tell the user the option is invalid
            break;
        }

        if (!keep_running) {
            break;                                                            // user chose 0, exit immediately
        }

        // prompt user to perform another action or exit
        std::cout << "Would you like to perform another action? (y/n): ";
        char again = 'n';
        std::cin >> again;
        if (again != 'y' && again != 'Y') {
            keep_running = false;                                             // exit on anything other than 'y'/'Y'
        }
    }

    return 0;                                                                 // indicate successful completion
}
