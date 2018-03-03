#include <iostream>
#include <iomanip>
#include <fstream>
#include "libvsop2013/include/ascii_data.hpp"

class data_printer : public vsop2013::data_parser_listener
{
public:
    data_printer() : offset(0) {

    }

    void on_start() override {
        offset = 0;
        std::cout << "START" << std::endl;
    }

    void on_header(const vsop2013::header_t& header) override {
        structure_start("HEADER");
        field("version", header.version);
        field("span_start", header.span_start);
        field("span_end", header.span_end);
        field("interval", header.interval);
        field("tables", header.tables);
        field("table_size", header.table_size);

        for (size_t planet = 0; planet < vsop2013::PLANET_COUNT; ++planet) {
            auto& desc = header.structure[planet];
            structure_start("PLANET DATA DESC");
            field("planet", desc.planet);
            field("offset", desc.offset);
            field("coeff_count", desc.coeff_count);
            field("sub_intervals", desc.sub_intervals);
            structure_end();
        }

        structure_end();
    }

    void on_table(const vsop2013::table_t& table) override {
        structure_start("TABLE");
        field("start", table.start);
        field("stop", table.stop);

        for (const auto& planet_data: table.data) {
            structure_start("PLANET_DATA");
            for (const auto& sub: planet_data.sub_intervals) {
                structure_start("SUB_INTERVAL");
                structure_end(); // SUB_INTERVAL
            }
            structure_end(); // PLANET_DATA
        }
        structure_end(); // TABLE
    }

    void on_done() override {
        structure_start("DONE");
        structure_end();
    }

private:
    size_t offset;

    template<typename T>
    void field(const std::string& name, T value) {
        indent();
        std::cout << name << " = " << value << std::endl;
    }

    void structure_start(const std::string& type) {
        indent();
        std::cout << "[" << type << "]" << std::endl;
        push();
    }

    void structure_end() {
        pop();
    }

    void indent() {
        for (size_t iter = 0; iter < offset; ++iter) {
            std::cout << ' ';
        }
    }

    void push() {
        ++offset;
    }

    void pop() {
        --offset;
    }
};

class sql_printer : public vsop2013::data_parser_listener
{
public:
    sql_printer(std::ostream& out)
            : _out(out)
            , _span_counter(0)
            , _tables_total(0)
            , _tables_current(0)
    {

    }

    void on_start() override {
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

    void on_header(const vsop2013::header_t& hdr) override {
        _tables_total = hdr.tables;
    }

    void on_table(const vsop2013::table_t& tbl) override {
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
                     << timepoints[sub_iter+1]
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

    void on_done() override {

    }

private:
    std::ostream& _out;
    uint64_t _span_counter;
    uint64_t _tables_total;
    uint64_t _tables_current;
};

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

    data_printer printer;
    sql_printer sql(std::cout);
    //vsop2013::ascii_data_parser::parse(ifs, printer);
    vsop2013::ascii_data_parser::parse(ifs, sql);

}
