// <copyright file="oscilloscope_channel_unit.h" company="Visualisierungsinstitut der Universit�t Stuttgart">
// Copyright � 2023 Visualisierungsinstitut der Universit�t Stuttgart.
// Licenced under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph M�ller</author>

#pragma once


namespace visus {
namespace power_overwhelming {

    /// <summary>
    /// Defines possible units for a channel of the oscilloscope.
    /// </summary>
    enum class oscilloscope_channel_unit {

        /// <summary>
        /// The channel measures voltage.
        /// </summary>
        volt,

        /// <summary>
        /// The channel measures current.
        /// </summary>
        ampere
    };

} /* namespace power_overwhelming */
} /* namespace visus */
