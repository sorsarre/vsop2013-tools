#pragma once
#include <memory>

#include <vsop2013/parser_listener.hpp>

namespace printers
{
    //--------------------------------------------------------------------------
    class text_printer : public vsop2013::data_parser_listener
    {
    public:
        text_printer();
        void on_start() override;
        void on_header(const vsop2013::header_t& header) override;
        void on_table(const vsop2013::table_t& table) override;
        void on_done() override;

    private:
        struct impl;

        std::shared_ptr<impl> _impl;
    };
}
