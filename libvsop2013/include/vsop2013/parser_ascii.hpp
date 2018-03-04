#pragma once
#include <istream>

#include <vsop2013/common.hpp>
#include <vsop2013/parser_listener.hpp>

namespace vsop2013
{
    class ascii_data_parser
    {
    public:
        static void parse(std::istream& in, data_parser_listener& listener);

    private:
        static void parse_table(
                std::istream& in,
                const header_t& header,
                table_t& table);

        static void parse_planet_data(
                std::istream& in,
                const table_structure_descriptor_t& desc,
                planet_data_t& planet_data);

        static void parse_sub_interval(
                std::istream& in,
                const table_structure_descriptor_t& desc,
                sub_interval_t& sub);

        static void parse_header(std::istream& in, header_t& header);
    };
}
