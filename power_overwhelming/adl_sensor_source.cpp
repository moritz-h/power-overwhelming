// <copyright file="adl_sensor_source.cpp" company="Visualisierungsinstitut der Universit�t Stuttgart">
// Copyright � 2022 Visualisierungsinstitut der Universit�t Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph M�ller</author>

#include "power_overwhelming/adl_sensor_source.h"

#include <stdexcept>


/*
 * visus::power_overwhelming::operator |
 */
visus::power_overwhelming::adl_sensor_source
visus::power_overwhelming::operator |(const adl_sensor_source lhs,
        const adl_sensor_source rhs) {
    typedef std::underlying_type<adl_sensor_source>::type mask_type;
    auto retval = static_cast<mask_type>(lhs) | static_cast<mask_type>(rhs);
    return static_cast<adl_sensor_source>(retval);
}


/*
 * visus::power_overwhelming::operator &
 */
visus::power_overwhelming::adl_sensor_source
visus::power_overwhelming::operator &(const adl_sensor_source lhs,
        const adl_sensor_source rhs) {
    typedef std::underlying_type<adl_sensor_source>::type mask_type;
    auto retval = static_cast<mask_type>(lhs) & static_cast<mask_type>(rhs);
    return static_cast<adl_sensor_source>(retval);
}


/*
 * visus::power_overwhelming::to_string
 */
const wchar_t *visus::power_overwhelming::to_string(
        const adl_sensor_source source) {
#define _TO_STRING_CASE(v) case adl_sensor_source::##v: return L#v

    switch (source) {
        _TO_STRING_CASE(asic);
        _TO_STRING_CASE(cpu);
        _TO_STRING_CASE(graphics);
        _TO_STRING_CASE(soc);
        _TO_STRING_CASE(all);

        default:
            throw std::invalid_argument("The specified sensor source is "
                "unknown. Make sure to add all new sources in to_string.");
    }

#undef _TO_STRING_CASE
}
