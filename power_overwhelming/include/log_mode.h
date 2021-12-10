// <copyright file="log_mode.h" company="Visualisierungsinstitut der Universit�t Stuttgart">
// Copyright � 2021 Visualisierungsinstitut der Universit�t Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph M�ller</author>

#pragma once


namespace visus {
namespace power_overwhelming {

    /// <summary>
    /// Defines possible logging modes of the <see cref="hmc8015_sensor" />.
    /// </summary>
    enum class log_mode {

        /// <summary>
        /// Capture data until explicitly stopped or the memory is full.
        /// </summary>
        unlimited,

        /// <summary>
        /// Capture the specified number of samples.
        /// </summary>
        count,

        /// <summary>
        /// Capture for the specified duration after start.
        /// </summary>
        duration,

        /// <summary>
        /// Capture for the specified duration after the given start time.
        /// </summary>
        time_span
    };

} /* namespace power_overwhelming */
} /* namespace visus */
