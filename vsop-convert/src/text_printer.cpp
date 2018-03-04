#include <iostream>

#include "text_printer.hpp"

using namespace printers;

namespace {
    class output_helper {
    public:
        //----------------------------------------------------------------------
        output_helper()
                : offset(0)
        {

        }

        //----------------------------------------------------------------------
        void reset()
        {
            offset = 0;
        }

        //----------------------------------------------------------------------
        template<typename T>
        void field(const std::string& name, T value) {
            indent();
            std::cout << name << " = " << value << std::endl;
        }

        //----------------------------------------------------------------------
        void structure_start(const std::string& type) {
            indent();
            std::cout << "[" << type << "]" << std::endl;
            push();
        }

        //----------------------------------------------------------------------
        void structure_end() {
            pop();
        }

        //----------------------------------------------------------------------
        void indent() {
            for (size_t iter = 0; iter < offset; ++iter) {
                std::cout << ' ';
            }
        }

        //----------------------------------------------------------------------
        void push() {
            ++offset;
        }

        //----------------------------------------------------------------------
        void pop() {
            --offset;
        }

    private:
        size_t offset;
    };
}

//------------------------------------------------------------------------------
struct text_printer::impl
{
    output_helper helper;
};

//------------------------------------------------------------------------------
text_printer::text_printer()
{
    _impl = std::make_shared<impl>();
}

//------------------------------------------------------------------------------
void text_printer::on_start() {
    _impl->helper.reset();
    std::cout << "START" << std::endl;
}

//------------------------------------------------------------------------------
void text_printer::on_header(const vsop2013::header_t& header) {
    output_helper& helper = _impl->helper;
    helper.structure_start("HEADER");
    helper.field("version", header.version);
    helper.field("span_start", header.span_start);
    helper.field("span_end", header.span_end);
    helper.field("interval", header.interval);
    helper.field("tables", header.tables);
    helper.field("table_size", header.table_size);

    for (size_t planet = 0; planet < vsop2013::PLANET_COUNT; ++planet) {
        auto& desc = header.structure[planet];
        helper.structure_start("PLANET DATA DESC");
        helper.field("planet", desc.planet);
        helper.field("offset", desc.offset);
        helper.field("coeff_count", desc.coeff_count);
        helper.field("sub_intervals", desc.sub_intervals);
        helper.structure_end();
    }

    helper.structure_end();
}

//------------------------------------------------------------------------------
void text_printer::on_table(const vsop2013::table_t& table) {
    output_helper& helper = _impl->helper;
    helper.structure_start("TABLE");
    helper.field("start", table.start);
    helper.field("stop", table.stop);

    for (const auto& planet_data: table.data) {
        helper.structure_start("PLANET_DATA");
        for (const auto& sub: planet_data.sub_intervals) {
            helper.structure_start("SUB_INTERVAL");
            helper.structure_end(); // SUB_INTERVAL
        }
        helper.structure_end(); // PLANET_DATA
    }
    helper.structure_end(); // TABLE
}

//------------------------------------------------------------------------------
void text_printer::on_done() {
    output_helper& helper = _impl->helper;
    helper.structure_start("DONE");
    helper.structure_end();
}
