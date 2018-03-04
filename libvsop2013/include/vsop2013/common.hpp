#pragma once

#include <cstdint>
#include <vector>

namespace vsop2013
{
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

    //--------------------------------------------------------------------------
    static constexpr const size_t PLANET_COUNT = 9;

    //--------------------------------------------------------------------------
    struct table_structure_descriptor_t
    {
        uint32_t planet;
        uint32_t coeff_count;
        uint32_t offset;
        uint32_t sub_intervals;
    };

    //--------------------------------------------------------------------------
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

    //--------------------------------------------------------------------------
    struct sub_interval_t
    {
        std::vector<double> data[6]; // [coeff_count] * 6
    };

    //--------------------------------------------------------------------------
    struct planet_data_t
    {
        std::vector<sub_interval_t> sub_intervals;
    };

    //--------------------------------------------------------------------------
    struct table_t
    {
        double start;
        double stop;
        std::vector<planet_data_t> data;
    };
}
