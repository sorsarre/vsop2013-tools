#pragma once

#include <cstdint>
#include <cmath>
#include <vector>
#include <memory>

namespace vsop2013
{
    /*
 * Data interpretation:
 * 1. Find table for given JED
 * 2. table_idx = floor( (JED - span_start) / interval )
 * 3. iad -- offset
 * 4. ncf -- coeff_count
 * 5. nsi -- sub_intervals
 * 6. delta2 = interval / sub_intervals
 * 7. ik = sub_interval_index = floor( (JED - table.start) / delta2 )
 * 8. iloc = offset + (6 * coeff_count * sub_interval_index)
 * 9. x = (2.0 * (JED - table.start) / delta2) - 1.0
 * 10. tn[coeff_count] -- sum components
 * 11. tn[0] = 1.0
 * 12. tn[1] = x
 * 13. for (int i : range(2, coeff_count))
 * 14.   tn[i] = (2.0*x*tn[i-1]) - tn[i-2]
 * 15. r[6] -- result
 * 16. for (int i : range(0, 6)) // over rows, each row contains coefficients for a variable
 * 17.   for (int j : range(0, coeff_count)) // over columns, reverse order
 * 18.     jp = coeff_count - j - 1
 * 19.     jt = iloc + (coeff_count * i) + jp // absolute offset in the table, base + row_offset + column_offset
 * 20.     r[i] = r[i] + (tn[jp] * table[jt])
 * 21. return r
 */

    static constexpr const size_t PLANET_COUNT = 9;

    struct table_structure_descriptor_t
    {
        uint32_t planet;
        uint32_t coeff_count;
        uint32_t offset;
        uint32_t sub_intervals;
    };

    struct header_t
    {
        uint32_t version; // usually 2013
        double span_start; // JD
        double span_end; // JD
        double interval; // interval length in days
        uint32_t tables; // number of tables
        uint32_t table_size; // number of coefficients in a table
        std::vector<table_structure_descriptor_t> structure;
    };



    struct sub_interval_t
    {
        std::vector<double> data[6]; // [coeff_count] * 6
    };

    struct planet_data_t
    {
        std::vector<sub_interval_t> sub_intervals;
    };

    struct table_t
    {
        double start;
        double stop;
        std::vector<planet_data_t> data;
    };

    class data_parser_listener
    {
    public:
        virtual void on_start() = 0;
        virtual void on_header(const header_t& hdr) = 0;
        virtual void on_table(const table_t& tbl) = 0;
        virtual void on_done() = 0;
        virtual ~data_parser_listener() = default;
    };

    /*
 * Resulting table structure:
 * - start
 * - stop
 * - planet_0
 *   - sub_interval_0
 *     - variable_0
 *       - coefficients[coeff_count]
 *     - variable_1
 *       - coefficients[coeff_count]
 *     ...
 *     - variable_6
 *       - coefficients[coeff_count]
 *   ...
 *   - sub_interval_${sub_intervals}
 */

    class ascii_data_parser
    {
    public:
        static void parse(std::istream& in, data_parser_listener& listener)
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

    private:
        static void parse_table(std::istream& in, const header_t& header, table_t& table)
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

        static void parse_planet_data(std::istream& in, const table_structure_descriptor_t& desc, planet_data_t& planet_data)
        {
            for (size_t sub = 0; sub < desc.sub_intervals; ++sub) {
                sub_interval_t sub_interval;
                parse_sub_interval(in, desc, sub_interval);
                planet_data.sub_intervals.push_back(std::move(sub_interval));
            }
        }

        static void parse_sub_interval(std::istream& in, const table_structure_descriptor_t& desc, sub_interval_t& sub)
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

        static void parse_header(std::istream& in, header_t& header)
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
    };
}