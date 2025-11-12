// Task 1: Reading the data file to see what it looks like.

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    // Filename has _ within it.
    const std::string default_file = "input2D_float.txt";
    const std::string filename = (argc > 1) ? argv[1] : default_file;

    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Error: could not open file, are you in the right directory?: " << filename << '\n';
        return 1;
    }

    std::string line;
    std::size_t line_no = 0;
    while (std::getline(in, line)) {             
        std::cout << ++line_no << ": " << line << '\n';
    }

    if (!in.eof()) {
        std::cerr << "Warning: stream ended unexpectedly (read error before EOF).\n";
        return 2;
    }
    return 0;
}
