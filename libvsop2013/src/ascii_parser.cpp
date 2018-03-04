#include <cmath>

#include <vsop2013/parser_ascii.hpp>

using namespace vsop2013;

//------------------------------------------------------------------------------
void ascii_data_parser::parse(std::istream& in, data_parser_listener& listener)
{
    listener.on_start();
    header_t header;

    parse_header(in, header);
    listener.on_header(header);

    for (size_t iter = 0; iter < header.tables; ++iter) {
        table_t table;
        parse_table(in, header, table);
        listener.on_table(table);
    }
}

//------------------------------------------------------------------------------
void ascii_data_parser::parse_table(
        std::istream& in,
        const header_t& header,
        table_t& table)
{
    in >> table.start;
    in >> table.stop;

    for (size_t iter = 0; iter < PLANET_COUNT; ++iter) {
        planet_data_t planet_data;
        const table_structure_descriptor_t& desc = header.structure[iter];
        parse_planet_data(in, desc, planet_data);
        table.data.push_back(std::move(planet_data));
    }
}

//------------------------------------------------------------------------------
void ascii_data_parser::parse_planet_data(
        std::istream& in,
        const table_structure_descriptor_t& desc,
        planet_data_t& planet_data)
{
    for (size_t sub = 0; sub < desc.sub_intervals; ++sub) {
        sub_interval_t sub_interval;
        parse_sub_interval(in, desc, sub_interval);
        planet_data.sub_intervals.push_back(std::move(sub_interval));
    }
}

//------------------------------------------------------------------------------
void ascii_data_parser::parse_sub_interval(
        std::istream& in,
        const table_structure_descriptor_t& desc,
        sub_interval_t& sub)
{
    for (size_t var = 0; var < 6; ++var) {
        sub.data[var].resize(desc.coeff_count);
        for (size_t iter = 0; iter < desc.coeff_count; ++iter) {
            double val = 0.0;
            double exp = 0.0;
            in >> val;
            in >> exp;
            sub.data[var][iter] = val * pow(10, exp);
        }
    }
}

//------------------------------------------------------------------------------
void ascii_data_parser::parse_header(std::istream& in, header_t& header)
{
    in >> header.version;
    in >> header.span_start;
    in >> header.span_end;
    in >> header.interval;
    in >> header.tables;
    in >> header.table_size;

    header.structure.resize(PLANET_COUNT);
    for (size_t iter = 0; iter < PLANET_COUNT; ++iter) {
        auto& desc = header.structure[iter];
        desc.planet = iter;
        in >> desc.offset;
        desc.offset--;
    }

    for (size_t iter = 0; iter < PLANET_COUNT; ++iter) {
        in >> header.structure[iter].coeff_count;
    }

    for (size_t iter = 0; iter < PLANET_COUNT; ++iter) {
        in >> header.structure[iter].sub_intervals;
    }
}
