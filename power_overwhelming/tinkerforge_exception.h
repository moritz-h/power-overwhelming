// <copyright file="tinkerforge_exception.h" company="Visualisierungsinstitut der Universit�t Stuttgart">
// Copyright � 2021 Visualisierungsinstitut der Universit�t Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph M�ller</author>

#pragma once

#include <exception>
#include <string>


namespace visus {
namespace power_overwhelming {

    /// <summary>
    /// An exception representing an error in the Tinkerforge API.
    /// </summary>
    class tinkerforge_exception final : public std::exception {

    public:

        /// <summary>
        /// The native error type.
        /// </summary>
        typedef int value_type;

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        /// <param name="code">The error code, which also determines the error
        /// message.</param>
        tinkerforge_exception(const value_type code);

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        /// <param name="code">The error code.</param>
        /// <param name="message">A custom error message.</param>
        tinkerforge_exception(const value_type code, const std::string& message)
            : std::exception(message.c_str()), _code(code) { }

        /// <summary>
        /// Answer the native error code associated with the exception.
        /// </summary>
        /// <returns></returns>
        value_type code(void) const noexcept {
            return this->_code;
        }

    private:

        value_type _code;
    };

} /* namespace power_overwhelming */
} /* namespace visus */
