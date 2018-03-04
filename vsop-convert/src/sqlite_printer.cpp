#include <iostream>
#include <iomanip>

#include "sqlite_printer.hpp"

using namespace printers;

//------------------------------------------------------------------------------
sqlite_printer::sqlite_printer(std::ostream& out)
        : _out(out)
          , _span_counter(0)
          , _tables_total(0)
          , _tables_current(0) {

}

//------------------------------------------------------------------------------
void sqlite_printer::on_start() {
    _out << std::setprecision(15);
    _span_counter = 0;
    _tables_total = 0;
    _tables_current = 0;
    _out << "CREATE TABLE vsop2013_spans(\n"
         << "    id integer PRIMARY KEY,\n"
         << "    planet INTEGER,\n"
         << "    jde_start REAL,\n"
         << "    jde_end REAL\n"
         << ");\n\n";

    _out << "CREATE TABLE vsop2013_coeff(\n"
         << "    idx INTEGER,\n"
         << "    px REAL,\n"
         << "    py REAL,\n"
         << "    pz REAL,\n"
         << "    vx REAL,\n"
         << "    vy REAL,\n"
         << "    vz REAL,\n"
         << "    span INTEGER,\n"
         << "    FOREIGN KEY(span) REFERENCES vsop2013_spans(id)"
         << ");\n\n";
}

//------------------------------------------------------------------------------
void sqlite_printer::on_header(const vsop2013::header_t& hdr) {
    _tables_total = hdr.tables;
}

//------------------------------------------------------------------------------
void sqlite_printer::on_table(const vsop2013::table_t& tbl) {
    auto start = tbl.start;
    auto end = tbl.stop;
    std::cerr << "\rTables processed: "
              << _tables_current
              << "/"
              << _tables_total;
    ++_tables_current;

    for (size_t planet = 0; planet < vsop2013::PLANET_COUNT; ++planet) {
        const auto& data = tbl.data[planet];

        size_t subs = data.sub_intervals.size();

        // generate timepoints for spans
        std::vector<double> timepoints;
        double delta = (end - start) / subs;
        double current = start;
        for (size_t sub_iter = 0; sub_iter < subs; ++sub_iter) {
            timepoints.push_back(current);
            current += delta;
        }
        timepoints.push_back(end);

        // generate actual data
        for (size_t sub_iter = 0; sub_iter < subs; ++sub_iter) {
            // generate span first
            ++_span_counter;
            _out << "INSERT INTO vsop2013_spans VALUES("
                 << _span_counter
                 << ", "
                 << planet
                 << ", "
                 << timepoints[sub_iter]
                 << ", "
                 << timepoints[sub_iter + 1]
                 << ");\n";

            // now generate coefficients
            const auto& coeffs = data.sub_intervals[sub_iter];
            for (size_t coeff_iter = 0; coeff_iter < coeffs.data[0].size(); ++coeff_iter) {
                _out << "INSERT INTO vsop2013_coeff VALUES("
                     << coeff_iter
                     << ", ";

                for (size_t var_iter = 0; var_iter < 6; ++var_iter) {
                    _out << coeffs.data[var_iter][coeff_iter] << ", ";
                }

                _out << _span_counter << ");\n";
            }

            _out << "\n";
        }
    }
}

//------------------------------------------------------------------------------
void sqlite_printer::on_done() {

}
