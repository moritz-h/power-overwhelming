﻿// <copyright file="rtx_instrument_configuration.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "power_overwhelming/oscilloscope_quantity.h"
#include "power_overwhelming/oscilloscope_single_acquisition.h"
#include "power_overwhelming/rtx_instrument.h"


namespace visus {
namespace power_overwhelming {

    /// <summary>
    /// This class stores the shared part of the configuration of an
    /// <see cref="rtx_sensor" />, which cannot be configured individually if
    /// multiple sensors are created on the same <see cref="rtx_instrument" />.
    /// </summary>
    /// <remarks>
    /// <para>When using an oscilloscope with four channels or more, it is
    /// possible to create an <see cref="rtx_sensor" /> for each pair of
    /// channels (one for voltage and one for current), thus having more than
    /// one sensor operating on the same <see cref="rtx_instrument" />. In this
    /// situation, not all of the parameters of the instrument can be configured
    /// individually. For instance, the length of the waveform being recorded
    /// on a trigger signal is the same for all channels. This configuration
    /// object groups all of these settings that will be shared between sensors
    /// on the same instrument.</para>
    /// <para>Settings grouped in this class need to be set only once for the
    /// set of sensors created from the same instruments. Likewise, it is
    /// strongly recommended to apply the same settings to all instruments when
    /// working with multiple oscilloscopes in order to produce consistent
    /// results.</para>
    /// </remarks>
    class POWER_OVERWHELMING_API rtx_instrument_configuration final {

    public:


        /// <summary>
        /// Answers the way the instrument will acquire one or more samples.
        /// </summary>
        /// <returns>The acquisition configuration of the instrument.</returns>
        inline const oscilloscope_single_acquisition& acquisition(
                void) const noexcept {
            return this->_acquisition;
        }

        /// <summary>
        /// Applies the configuration on the given instrument.
        /// </summary>
        /// <remarks>
        /// <para>This method will block the calling thread until the instrument
        /// has applied all changes.</para>
        /// </remarks>
        /// <param name="instrument">The instrument to apply the configuration
        /// to.</param>
        void apply(_Inout_ rtx_instrument& instrument) const;

        /// <summary>
        /// Answers the timeout of the instrument in milliseconds.
        /// </summary>
        /// <returns>The timeout of the instrument.</returns>
        inline visa_instrument::timeout_type timeout(void) const noexcept {
            return this->_timeout;
        }

        /// <summary>
        /// Answers the time range the instrument will cover when a new sampl
        /// is being requested.
        /// </summary>
        /// <returns>The time range the instrument will acquire.</returns>
        inline const oscilloscope_quantity& time_tange(void) const noexcept {
            return this->_time_range;
        }

    private:

        oscilloscope_single_acquisition _acquisition;
        visa_instrument::timeout_type _timeout;
        oscilloscope_quantity _time_range;

    };

} /* namespace power_overwhelming */
} /* namespace visus */
