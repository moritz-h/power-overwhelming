﻿// <copyright file="collector.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2022 - 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <algorithm>
#include <array>
#include <initializer_list>
#include <memory>
#include <numeric>
#include <type_traits>
#include <vector>

#include "power_overwhelming/collector_settings.h"


namespace visus {
namespace power_overwhelming {

    /* Forward declarations */
    namespace detail { struct collector_impl; }

    /// <summary>
    /// A collector is a set of sensors that asynchronously collect data and
    /// write them to a file.
    /// </summary>
    class POWER_OVERWHELMING_API collector final {

    public:

        /// <summary>
        /// Creates a collector for all sensors that could be found and
        /// instantiated on the machine.
        /// </summary>
        /// <param name="settings">The general settings for the collector,
        /// including the sampling interval.</param>
        /// <returns>A collector for all available sensors.</returns>
        static collector for_all(_In_ const collector_settings& settings);

        /// <summary>
        /// Creates a collector for all sensors that could be found and
        /// instantiated on the machne.
        /// </summary>
        /// <param name="output_path">The path where the collector should write
        /// the output to.</param>
        /// <param name="sampling_interval">The sampling interval for the sensors
        /// in microseconds.</param>
        /// <returns>A collector for all available sensors.</returns>
        /// <exception std::invalid_argument">If <paramref name="output_path" />
        /// is <c>nullptr</c>.</exception>
        inline static collector for_all(_In_z_ const wchar_t *output_path,
                _In_ const sensor::microseconds_type sampling_interval
                = collector_settings::default_sampling_interval) {
            return for_all(collector_settings().output_path(output_path)
                .sampling_interval(sampling_interval));
        }

        /// <summary>
        /// Create a collector for the configuration template generated by
        /// <see cref="make_configuration_template" />.
        /// </summary>
        /// <returns>A collector for the default configuration template on the
        /// machine.</returns>
        static collector from_defaults(void);

        /// <summary>
        /// Initialise a new instance from the given JSON configuration file.
        /// </summary>
        /// <param name="path">The path to the JSON configuration file.</param>
        /// <returns>A new collector configured with the sensors in the
        /// specified file.</returns>
        static collector from_json(_In_z_ const wchar_t *path);

        /// <summary>
        /// Initialise a new instance from the given lists of sensors.
        /// </summary>
        /// <typeparam name="TSensorLists">The types of the lists of sensors,
        /// which must be STL collection types (with <c>begin</c> and
        /// <c>end</c>) or sensors.</typeparam>
        /// <param name="settings">The general settings for the collector,
        /// including the sampling interval.</param>
        /// <param name="sensors">The lists of sensors.</param>
        /// <returns></returns>
        template<class... TSensorLists>
        static collector from_sensor_lists(
            _In_ const collector_settings& settings,
            TSensorLists&&... sensors);

        /// <summary>
        /// Initialise a new instance from the given compile-time list of
        /// (possibly different kinds of) sensors.
        /// </summary>
        /// <typeparam name="TSensors>The types of the sensors to be used by
        /// the collector.</typeparam>
        /// <param name="settings">The general settings for the collector,
        /// including the sampling interval.</param>
        /// <param name="sensors">A compile-time list of sensors. The new
        /// instance will take ownership of these sensors, ie they will be
        /// disposed by moving them once the method returns.</param>
        /// <returns>A new collector using the given sensors.</returns>
        template<class... TSensors>
        static collector from_sensors(_In_ const collector_settings& settings,
            TSensors&&... sensors);

        /// <summary>
        /// Creates a configuration file for all sensors currently attached to
        /// the system.
        /// </summary>
        /// <remarks>
        /// <para>This method might run for a significant amount of time as it
        /// tries probing all known sensor types.</para>
        /// <para><see cref="tinkerforge_sensor" />s are only enumerated on
        /// <see cref="tinkerforge_sensor::default_host" /> and must listen on
        /// <see cref="tinkerforge_sensor::default_port" />. For remote machines
        /// or differently configured brickds, you must build the configuration
        /// manually.</para>
        /// </remarks>
        /// <param name="path">The path where the configuration file should be
        /// stored.</param>
        static void make_configuration_template(_In_z_ const wchar_t *path);

        /// <summary>
        /// Initialise a new instance.
        /// </summary>
        /// <remarks>
        /// The default instance is created disposed, ie cannot be used for
        /// anything, but assigning another instance created by one of the
        /// static factory methods.
        /// </remarks>
        inline collector(void) : _impl(nullptr) { }

        /// <summary>
        /// Move <paramref name="rhs" /> into a new instance.
        /// </summary>
        /// <param name="rhs"></param>
        /// <returns></returns>
        inline collector(_In_ collector&& rhs) noexcept : _impl(rhs._impl) {
            rhs._impl = nullptr;
        }

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        ~collector(void);

        /// <summary>
        /// Inject a marker in the command stream which will be included in the
        /// output.
        /// </summary>
        /// <param name="marker"></param>
        void marker(_In_opt_z_ const wchar_t *marker);

        /// <summary>
        /// Answer the number of sensors in the collector.
        /// </summary>
        /// <returns>The number of sensors. For disposed collectors, this is
        /// always zero.</returns>
        std::size_t size(void) const noexcept;

        /// <summary>
        /// Start collecting from all sensors.
        /// </summary>
        void start(void);

        /// <summary>
        /// Stop collecting.
        /// </summary>
        /// <remarks>
        /// It is safe to call this method on disposed instances, in which case it
        /// will have no effect.
        /// </remarks>
        void stop(void);

        /// <summary>
        /// Move assignment.
        /// </summary>
        /// <param name="rhs">The right-hand side operand</param>
        /// <returns><c>*this</c></returns>
        collector& operator =(_In_ collector&& rhs) noexcept;

        /// <summary>
        /// Determines whether the object is valid.
        /// </summary>
        /// <remarks>
        /// The collector is considered valid until it has been disposed by a
        /// move operation.
        /// </remarks>
        /// <returns><c>true</c> if the object is valid, <c>false</c>
        /// otherwise.</returns>
        operator bool(void) const noexcept;

    private:

        /// <summary>
        /// Moves the given list of sensors to the heap.
        /// </summary>
        /// <typeparam name="TSensorList"></typeparam>
        /// <param name="sensors"></param>
        /// <returns></returns>
        template<class TSensorList>
        static std::vector<std::unique_ptr<sensor>> move_to_heap(
            TSensorList&& sensors);

        /// <summary>
        /// Creates a new collector that has no sensors, but reserved space for
        /// the given number of sensors.
        /// </summary>
        /// <param name="settings"></param>
        /// <param name="capacity"></param>
        /// <returns>A new collector without sensors.</returns>
        static collector prepare(_In_ const collector_settings& settings,
            _In_ const std::size_t capacity);

        /// <summary>
        /// Initialise a new instance.
        /// </summary>
        /// <param name="impl"></param>
        inline collector(_In_opt_ detail::collector_impl *impl)
            : _impl(impl) { }

        /// <summary>
        /// Adds a new sensor to the collector.
        /// </summary>
        /// <remarks>
        /// This method is not thread-safe and must only be called while the
        /// collector is not running. It is therefore purposefully not part of
        /// the public interface.
        /// </remarks>
        /// <param name="sensor">The sensor to be added. The object takes
        /// ownership of the object designated by this pointer. The object
        /// must have been allocated using C++ <c>new</c>.</param>
        void add(_In_ sensor *sensor);

        detail::collector_impl *_impl;

    };

} /* namespace power_overwhelming */
} /* namespace visus */

#include "power_overwhelming/collector.inl"
