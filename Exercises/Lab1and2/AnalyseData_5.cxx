/* Skeleton/Idea of code:
Take functions out of here into CustomFunctions.h/cxx
Modify this code so you don't have the fucntions and you call them in
Prompt to user to decide whether they want to do N lines or mag of all points*/

// Task 5+6: Take functions out into .h file and call from there.

/*Now this file is just being used for the rest. Up until this point I did a new
AnalyseData for each separate bit of the assignment. Makes sense to fiddle around with
this as I have a Makefile. */

#include "CustomFunctions.h"               // bring in Point and function declarations
#include <iostream>                        // for I/O
#include <limits>                          // for input recovery
#include <sstream>                         // for parsing argv values (optional)
#include <string>                          // for std::string
#include <vector>                          // for std::vector

int main(int argc, char* argv[]) {
    const std::string default_file = "input2D_float.txt";      // default filename (change if your file uses an underscore)
    const std::string filename = (argc > 1) ? argv[1] : default_file;  // allow override via argv

    std::vector<Point> data;                                   // container for (x,y) rows
    if (!load_data_csv_xy(filename, data)) return 1;           // load the file; exit on fatal open error

    std::cout << "Choose an option:\n";                        // show a menu
    std::cout << "  1) Print N lines of (x, y)\n";             // option 1 description
    std::cout << "  2) Compute and print magnitudes for all rows\n"; // option 2 description
    std::cout << "  3) Fit a straight line y = mx + c\n";      // show option 3 of for least squares straight line fit
    std::cout << "Enter 1, 2 or 3: ";                             // prompt
    int choice = 0;                                            // holder for user choice
    if (!(std::cin >> choice)) {                               // read choice
        std::cin.clear();                                      // clear fail state
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // flush bad input
        std::cerr << "Invalid choice.\n";                      // notify
        return 2;                                              // exit with error
    }

    if (choice == 1) {                                         // branch: print first N lines
        std::size_t N = 5;                                     // default N
        std::cout << "How many lines should I print? (default 5): "; // prompt for N
        if (!(std::cin >> N)) {                                // try to read N
            std::cin.clear();                                  // clear fail state
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // flush
            N = 5;                                             // fall back to default
        }
        print_first_n(data, N);                                // call printer
    } else if (choice == 3) {                                  // branch: least-squares line fit
        if (!fit_straight_line_least_squares(                  // call fitting function with input file
                filename,                                      // path to the data file
                "fit_result.txt")) {                           // name of output file for fitted line
            std::cerr << "Fitting failed.\n";                  // tell the user if something went wrong
            // you can choose to `return 4;` here if you want a special error code
        }

    } else {                                                   // branch: unsupported option
        std::cerr << "Unknown option.\n";                      // tell the user the choice was invalid
        return 3;                                              // exit with a non-zero error code
    }

    return 0;}     