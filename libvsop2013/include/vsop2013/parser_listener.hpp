#pragma once
#include <vsop2013/common.hpp>

namespace vsop2013
{
    class data_parser_listener
    {
    public:
        virtual void on_start() = 0;
        virtual void on_header(const header_t& hdr) = 0;
        virtual void on_table(const table_t& tbl) = 0;
        virtual void on_done() = 0;
        virtual ~data_parser_listener() = default;
    };
}
