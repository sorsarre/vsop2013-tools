#pragma once
#include <ostream>
#include <vsop2013/parser_listener.hpp>

namespace printers
{
    //--------------------------------------------------------------------------
    class sqlite_printer : public vsop2013::data_parser_listener
    {
    public:
        sqlite_printer(std::ostream& out);
        void on_start() override;
        void on_header(const vsop2013::header_t& hdr) override;
        void on_table(const vsop2013::table_t& tbl) override;
        void on_done() override;

    private:
        std::ostream& _out;
        uint64_t _span_counter;
        uint64_t _tables_total;
        uint64_t _tables_current;
    };

}
