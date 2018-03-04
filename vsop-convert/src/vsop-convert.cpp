#include <iostream>
#include <iomanip>
#include <fstream>

#include <vsop2013/parser_ascii.hpp>
#include <vsop2013/parser_listener.hpp>

#include "sqlite_printer.hpp"
#include "text_printer.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: test-binary <vsop2013-file>" << std::endl;
        return 1;
    }

    std::cerr << "Parsing VSOP2013 text file: " << argv[1] << std::endl;
    std::ifstream ifs(argv[1]);

    if (!ifs.is_open()) {
        std::cerr << "Could not open file " << argv[1] << " for reading." << std::endl;
        return 2;
    }

    printers::text_printer printer;
    printers::sqlite_printer sql(std::cout);

    vsop2013::ascii_data_parser::parse(ifs, sql);

}
