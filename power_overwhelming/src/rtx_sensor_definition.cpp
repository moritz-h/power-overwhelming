﻿// <copyright file="rtx_sensor_definition.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2021 - 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "power_overwhelming/rtx_sensor_definition.h"

#include <algorithm>
#include <stdexcept>

#include "power_overwhelming/convert_string.h"
#include "power_overwhelming/rtx_instrument.h"

#include "string_functions.h"


static constexpr const char *error_description = "The description of an "
    "oscilloscope-based sensor must not be empty.";
static constexpr const char *error_same_channel = "The channel measuring "
"voltage cannot be the same as the one measuring current.";


/*
 * visus::power_overwhelming::rtx_sensor_definition::rtx_sensor_definition
 */
visus::power_overwhelming::rtx_sensor_definition::rtx_sensor_definition(
        _In_z_ const wchar_t *description,
        _In_ const oscilloscope_channel& voltage_channel,
        _In_ const oscilloscope_channel& current_channel,
        _In_ const oscilloscope_waveform_points waveform_points)
    : _current_channel(current_channel),
        _description(detail::safe_assign(blob(), description)),
        _voltage_channel(voltage_channel),
        _waveform_points(waveform_points) {
    if (this->_description.empty() || (::wcslen(this->description()) < 1)) {
        throw std::invalid_argument(::error_description);
    }
    if (this->channel_current() == this->channel_voltage()) {
        throw std::invalid_argument(::error_same_channel);
    }
}


/*
 * visus::power_overwhelming::rtx_sensor_definition::rtx_sensor_definition
 */
visus::power_overwhelming::rtx_sensor_definition::rtx_sensor_definition(
        _In_z_ const wchar_t *description,
        _In_ const std::uint32_t channel_voltage,
        _In_ const float attenuation_voltage,
        _In_ const std::uint32_t channel_current,
        _In_ const float attenuation_current,
        _In_ const oscilloscope_waveform_points waveform_points)
    : _current_channel(oscilloscope_channel(channel_current)
        .attenuation(oscilloscope_quantity(attenuation_current, "A"))),
        _description(detail::safe_assign(blob(), description)),
        _voltage_channel(oscilloscope_channel(channel_voltage)
        .attenuation(oscilloscope_quantity(attenuation_voltage, "V"))),
        _waveform_points(waveform_points) {
    if (this->_description.empty() || (::wcslen(this->description()) < 1)) {
        throw std::invalid_argument(::error_description);
    }
    if (this->channel_current() == this->channel_voltage()) {
        throw std::invalid_argument(::error_same_channel);
    }

    // Auto-generate labels to the channels from the description.
    auto desc = detail::remove_spaces(convert_string<char>(
        this->description()));
    if (desc.length() > 7) {
        // RTA/RTB instruments allow only labels up to eight characters, so
        // truncate the description if it is longer.
        desc = desc.substr(0, 7);
    }

    {
        auto label = std::string("I") + desc;
        this->_current_channel.label(oscilloscope_label(label.c_str(), true));
    }

    {
        auto label = std::string("U") + desc;
        this->_voltage_channel.label(oscilloscope_label(label.c_str(), true));
    }
}


/*
 * visus::power_overwhelming::rtx_sensor_definition::apply
 */
void visus::power_overwhelming::rtx_sensor_definition::apply(
        _Inout_ rtx_instrument& instrument) const {
    instrument.channel(this->_current_channel)
        .channel(this->_voltage_channel)
        .operation_complete();
}
