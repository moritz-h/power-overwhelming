﻿// <copyright file="rtx_instrumen.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "power_overwhelming/oscilloscope_acquisition_state.h"
#include "power_overwhelming/oscilloscope_channel.h"
#include "power_overwhelming/oscilloscope_edge_trigger.h"
#include "power_overwhelming/oscilloscope_quantity.h"
#include "power_overwhelming/oscilloscope_reference_point.h"
#include "power_overwhelming/oscilloscope_single_acquisition.h"
#include "power_overwhelming/oscilloscope_trigger.h"
#include "power_overwhelming/oscilloscope_trigger_output.h"
#include "power_overwhelming/oscilloscope_waveform.h"
#include "power_overwhelming/oscilloscope_waveform_points.h"
#include "power_overwhelming/visa_instrument.h"


namespace visus {
namespace power_overwhelming {

    /// <summary>
    /// Specialisation for a Rohde &amp; Schwarz RTA/RTB oscilloscope.
    /// </summary>
    /// <remarks>
    /// <para>This specialisation only provides additional APIs for commonly
    /// used commands on the oscilloscopes. The rationale for exposing this
    /// in a separate public class rather than putting it in the sensor is
    /// twofold: First, there might be cases for controlling the oscilloscopes
    /// that are not related to Power Overwhelming sensors. By using this class,
    /// you can do that without reimplementing the logic for all of the methods
    /// in this class. Second, there are things that need to be configured
    /// only once, even if there are two sensors configured on a four-port
    /// oscilloscope. By iterating over all <see cref="rtx_instruments" />,
    /// these settings can be made easily without tracking the topology of
    /// the oscilloscope sensors manually.</para>
    /// </remarks>
    class POWER_OVERWHELMING_API rtx_instrument final
            : public visa_instrument {

    public:

        /// <summary>
        /// Creates a new istrument and invokes the given callback if the
        /// instrument was newly opened in contrast to be reused from other
        /// instances.
        /// </summary>
        /// <param name="path">The VISA resource path of the instrument.</param>
        /// <param name="on_new">The callback to be invoked if the instrument
        /// is new.</param>
        /// <param name="context">A user-defined context pointer to be passed
        /// to the <see cref="on_new" /> callback.</param>
        /// <param name="timeout">The timeout for the connection attempt in
        /// milliseconds. This parameter defaults to
        /// <see cref="default_timeout" />.</param>
        /// <exception cref="std::invalid_argument">If <paramref name="path" />
        /// is <c>nullptr</c> or if <paramref name="on_new" /> is
        /// <c>nullptr</c>.</exception>
        /// <exception cref="std::bad_alloc">If the memory for the sensor state
        /// could not be allocated.</exception>
        /// <exception cref="std::system_error">If the VISA library could not be
        /// loaded.</exception>
        /// <exception cref="visa_exception">If the sensor could not be
        /// initialised.</exception>
        static rtx_instrument create(_In_z_ const wchar_t *path,
            _In_ void (*on_new)(rtx_instrument&, void *),
            _In_opt_ void *context = nullptr,
            _In_ const timeout_type timeout = default_timeout);

        /// <summary>
        /// Creates a new istrument and invokes the given callback if the
        /// instrument was newly opened in contrast to be reused from other
        /// instances.
        /// </summary>
        /// <param name="path">The VISA resource path of the instrument.</param>
        /// <param name="on_new">The callback to be invoked if the instrument
        /// is new.</param>
        /// <param name="context">A user-defined context pointer to be passed
        /// to the <see cref="on_new" /> callback.</param>
        /// <param name="timeout">The timeout for the connection attempt in
        /// milliseconds. This parameter defaults to
        /// <see cref="default_timeout" />.</param>
        /// <exception cref="std::invalid_argument">If <paramref name="path" />
        /// is <c>nullptr</c> or if <paramref name="on_new" /> is
        /// <c>nullptr</c>.</exception>
        /// <exception cref="std::bad_alloc">If the memory for the sensor state
        /// could not be allocated.</exception>
        /// <exception cref="std::system_error">If the VISA library could not be
        /// loaded.</exception>
        /// <exception cref="visa_exception">If the sensor could not be
        /// initialised.</exception>
        static rtx_instrument create(_In_z_ const char *path,
            _In_ void (*on_new)(rtx_instrument&, void *),
            _In_opt_ void *context = nullptr,
            _In_ const timeout_type timeout = default_timeout);

        /// <summary>
        /// Creates a new istrument and perform a full reset of the device and
        /// the status and the error queue provided there is no other instrument
        /// instance running on the same hardware yet.
        /// </summary>
        /// <param name="path">The VISA resource path of the instrument.</param>
        /// <param name="timeout">The timeout for the connection attempt in
        /// milliseconds. This parameter defaults to
        /// <see cref="default_timeout" />.</param>
        /// <exception cref="std::invalid_argument">If <paramref name="path" />
        /// is <c>nullptr</c>.</exception>
        /// <exception cref="std::bad_alloc">If the memory for the sensor state
        /// could not be allocated.</exception>
        /// <exception cref="std::system_error">If the VISA library could not be
        /// loaded.</exception>
        /// <exception cref="visa_exception">If the sensor could not be
        /// initialised.</exception>
        static rtx_instrument create_and_reset_new(_In_z_ const wchar_t *path,
            _In_ const timeout_type timeout = default_timeout);

        /// <summary>
        /// Creates a new istrument and perform a full reset of the device and
        /// the status and the error queue provided there is no other instrument
        /// instance running on the same hardware yet.
        /// </summary>
        /// <param name="path">The VISA resource path of the instrument.</param>
        /// <param name="timeout">The timeout for the connection attempt in
        /// milliseconds. This parameter defaults to
        /// <see cref="default_timeout" />.</param>
        /// <exception cref="std::invalid_argument">If <paramref name="path" />
        /// is <c>nullptr</c>.</exception>
        /// <exception cref="std::bad_alloc">If the memory for the sensor state
        /// could not be allocated.</exception>
        /// <exception cref="std::system_error">If the VISA library could not be
        /// loaded.</exception>
        /// <exception cref="visa_exception">If the sensor could not be
        /// initialised.</exception>
        static rtx_instrument create_and_reset_new(_In_z_ const char *path,
            _In_ const timeout_type timeout = default_timeout);

        /// <summary>
        /// The product ID of the RTA and RTB series oscilloscopes.
        /// </summary>
        /// <remarks>
        /// This is the ID reported by the RTB2004 and RTA4004 devices we use at
        /// VISUS. If this does not work, use the RsVisaTester tool and discover
        /// the device via the &quot;Find Resource&quot; functionality. Do not
        /// forget to put the device in USB mode if discovering via USB.
        /// </remarks>
        static constexpr const char *product_id = "0x01D6";

        /// <summary>
        /// Initialises a new, but invalid instance.
        /// </summary>
        rtx_instrument(void);

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        /// <param name="path">The VISA resource path of the instrument.</param>
        /// <param name="timeout">The timeout for the connection attempt in
        /// milliseconds. This parameter defaults to
        /// <see cref="default_timeout" />.</param>
        /// <exception cref="std::invalid_argument">If <paramref name="path" />
        /// is <c>nullptr</c>.</exception>
        /// <exception cref="std::bad_alloc">If the memory for the sensor state
        /// could not be allocated.</exception>
        /// <exception cref="std::system_error">If the VISA library could not be
        /// loaded.</exception>
        /// <exception cref="visa_exception">If the sensor could not be
        /// initialised.</exception>
        rtx_instrument(_In_z_ const wchar_t *path,
            _In_ const timeout_type timeout = default_timeout);

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        /// <param name="path">The VISA resource path of the instrument.</param>
        /// <param name="timeout">The timeout for the connection attempt in
        /// milliseconds. This parameter defaults to
        /// <see cref="default_timeout" />.</param>
        /// <exception cref="std::invalid_argument">If <paramref name="path" />
        /// is <c>nullptr</c>.</exception>
        /// <exception cref="std::bad_alloc">If the memory for the sensor state
        /// could not be allocated.</exception>
        /// <exception cref="std::system_error">If the VISA library could not be
        /// loaded.</exception>
        /// <exception cref="visa_exception">If the sensor could not be
        /// initialised.</exception>
        rtx_instrument(_In_z_ const char *path,
            _In_ const timeout_type timeout = default_timeout);

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        /// <param name="is_new_connection">Indicates whether a new connection
        /// to an instrument was established or an existing one was reused. If
        /// this parameter returns <c>false</c>, callers should be aware that
        /// multiple code paths might manipulate the instrument independently.
        /// </param>
        /// <param name="path">The VISA resource path of the instrument.</param>
        /// <param name="timeout">The timeout for the connection attempt in
        /// milliseconds. This parameter defaults to
        /// <see cref="default_timeout" />.</param>
        /// <exception cref="std::invalid_argument">If <paramref name="path" />
        /// is <c>nullptr</c>.</exception>
        /// <exception cref="std::bad_alloc">If the memory for the sensor state
        /// could not be allocated.</exception>
        /// <exception cref="std::system_error">If the VISA library could not be
        /// loaded.</exception>
        /// <exception cref="visa_exception">If the sensor could not be
        /// initialised.</exception>
        rtx_instrument(_Out_ bool& is_new_connection,
            _In_z_ const wchar_t *path,
            _In_ const timeout_type timeout = default_timeout);

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        /// <param name="is_new_connection">Indicates whether a new connection
        /// to an instrument was established or an existing one was reused. If
        /// this parameter returns <c>false</c>, callers should be aware that
        /// multiple code paths might manipulate the instrument independently.
        /// </param>
        /// <param name="path">The VISA resource path of the instrument.</param>
        /// <param name="timeout">The timeout for the connection attempt in
        /// milliseconds. This parameter defaults to
        /// <see cref="default_timeout" />.</param>
        /// <exception cref="std::invalid_argument">If <paramref name="path" />
        /// is <c>nullptr</c>.</exception>
        /// <exception cref="std::bad_alloc">If the memory for the sensor state
        /// could not be allocated.</exception>
        /// <exception cref="std::system_error">If the VISA library could not be
        /// loaded.</exception>
        /// <exception cref="visa_exception">If the sensor could not be
        /// initialised.</exception>
        rtx_instrument(_Out_ bool& is_new_connection,
            _In_z_ const char *path,
            _In_ const timeout_type timeout = default_timeout);

        /// <summary>
        /// Configures signle acquisition mode on the device.
        /// </summary>
        /// <param name="acquisition">The configuration of single acquisition
        /// mode.</param>
        /// <param name="run">If <c>true</c>, start the acquisiton. This
        /// parameter defaults to <c>false</c>.</param>
        /// <param name="wait">If <paramref name="run" /> is <c>true</c> and
        /// this parameter is set <c>true</c> as well, the method will add an
        /// <c>*OPC?</c> query and block the calling code until the acquisition
        /// has completed.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::runtime_error">If the instument has been
        /// disposed by a move.</exception>
        /// <exception cref="visa_exception">If a VISA call failed.</exception>
        rtx_instrument& acquisition(
            _In_ const oscilloscope_single_acquisition& acquisition,
            _In_ const bool run = false,
            _In_ const bool wait = false);

        /// <summary>
        /// Changes the acquisition state of the instrument.
        /// </summary>
        /// <param name="state">The new state of the instrument.</param>
        /// <param name="wait">If the <paramref name="state" /> parameter is
        /// <see cref="oscilloscope_acquisition_state::single" /> and this
        /// parameter is <c>true</c>, the method will issue an <c>*OPC?</c>
        /// query after the single acquisition causing the calling code to block
        /// until the acquisition completed. Likewise, if a continuous
        /// acquisition is stopped gracefully by
        /// <see cref="oscilloscope_acquisition_state::stop" />, an <c>*OPC?</c>
        /// query will be added causing the calling code to block until the
        /// acquisition actually ended.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::runtime_error">If the instument has been
        /// disposed by a move.</exception>
        /// <exception cref="visa_exception">If a VISA call failed.</exception>
        const rtx_instrument& acquisition(
            _In_ const oscilloscope_acquisition_state state,
            _In_ const bool wait = false) const;

#if false
        /// <summary>
        /// 
        /// </summary>
        /// <param name="channel"></param>
        /// <returns></returns>
        blob ascii_data(_In_ const std::uint32_t channel);
#endif

        /// <summary>
        /// Downloads the data of the specified channel as floating-point
        /// numbers.
        /// </summary>
        /// <param name="channel">The one-based index of the channel to
        /// retrieve.</param>
        /// <returns>The channel data as a series of <c>float</c> values.
        /// </returns>
        blob binary_data(_In_ const std::uint32_t channel) const;

        /// <summary>
        /// Retrieve the configuration for the specified channel.
        /// </summary>
        /// <param name="channel">The one-based index of the channel to
        /// retrieve the configuration for.</param>
        /// <returns>The configuration for the specified channel.</returns>
        /// <exception cref="std::runtime_error">If the instance was disposed
        /// by moving it.</exception>
        /// <exception cref="visa_exception">If the sensor could not be
        /// initialised.</exception>
        /// <exception cref="std::logic_error">If the library was compiled
        /// without support for VISA.</exception>
        oscilloscope_channel channel(_In_ const std::uint32_t channel) const;

        /// <summary>
        /// Apply the specified channel configuration.
        /// </summary>
        /// <remarks>
        /// <para>This method does nothing if the library has been compiled
        /// without VISA support.</para>
        /// </remarks>
        /// <param name="channel">The channel configuration to apply.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::runtime_error">If the instance was disposed
        /// by moving it.</exception>
        /// <exception cref="visa_exception">If the sensor could not be
        /// initialised.</exception>
        rtx_instrument& channel(_In_ const oscilloscope_channel& channel);

        /// <summary>
        /// Counts the number of channels the instrument has.
        /// </summary>
        /// <remarks>
        /// It is safe to call this method on an instrument that has been
        /// disposed.
        /// </remarks>
        /// <remarks>An override timeout that the implementation uses for
        /// probing the channels. The previous timeout will be restored before
        /// the method returns.</remarks>
        /// <returns>The number of channels the instrument has.</returns>
        std::size_t channels(_In_ const timeout_type timeout = 500) const;

        /// <summary>
        /// Retrieves the waveform data for the specified channel.
        /// </summary>
        /// <param name="channel">The one-based index of the channel to retrieve
        /// the waveform for.</param>
        /// <param name="points">Specifies which sample points should be
        /// transferred.</param>
        /// <returns>The waveform for the specified channel.</returns>
        oscilloscope_waveform data(_In_ const std::uint32_t channel,
            _In_ const oscilloscope_waveform_points points) const;

        /// <summary>
        /// Enable and configure one of the mathematical expressions.
        /// </summary>
        /// <param name="channel">The maths channel to be configured. For an
        /// RTB2004, this must be within [1, 4].</param>
        /// <param name="expression">The arithmetic expression to be computed,
        /// ie &quot;CH1*CH2&quot;.</param>
        /// <param name="unit">The unit of the resulting values. If
        /// <c>nullptr</c>, the currently set unit will be unchanged. This
        /// parameter defaults to <c>nullptr</c>.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::runtime_error">If the method is called on an
        /// object that has been disposed by moving it.</exception>
        /// <exception cref="visa_exception">If any of the API calls to the
        /// instrument failed.</exception>
        rtx_instrument& expression(_In_ const std::uint32_t channel,
            _In_opt_z_ const wchar_t *expression,
            _In_opt_z_ const wchar_t *unit = nullptr);

        /// <summary>
        /// Enable and configure one of the mathematical expressions.
        /// </summary>
        /// <param name="channel">The maths channel to be configured. For an
        /// RTB2004, this must be within [1, 4].</param>
        /// <param name="expression">The arithmetic expression to be computed,
        /// ie &quot;CH1*CH2&quot;.</param>
        /// <param name="unit">The unit of the resulting values. If
        /// <c>nullptr</c>, the currently set unit will be unchanged. This
        /// parameter defaults to <c>nullptr</c>.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::runtime_error">If the method is called on an
        /// object that has been disposed by moving it.</exception>
        /// <exception cref="visa_exception">If any of the API calls to the
        /// instrument failed.</exception>
        rtx_instrument& expression(_In_ const std::uint32_t channel,
            _In_opt_z_ const char *expression,
            _In_opt_z_ const char *unit = nullptr);

        /// <summary>
        /// Queries the currently displayed history segment index.
        /// </summary>
        /// <returns>The history segment index currently displayed.</returns>
        /// <exception cref="std::runtime_error">If the method is called on an
        /// object that has been disposed by moving it.</exception>
        /// <exception cref="visa_exception">If any of the API calls to the
        /// instrument failed.</exception>
        /// <exception cref="std::logic_error">If the method is called while
        /// the library was compiled without support for VISA.</exception>
        int history_segment(void) const;

        /// <summary>
        /// Accesses a specific history segment in the memory.
        /// </summary>
        /// <param name="channel">The one-based channel index.</param>
        /// <param name="segment">The segment index, which can either be a
        /// negative index count where the latest segment is 0 and the older
        /// segments have a negative index, or positive index cound where the
        /// oldest segment has index 1 and the newest has index <c>n</c> where
        /// is the number of segments returned by
        /// <see cref="history_segments" />.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::runtime_error">If the method is called on an
        /// object that has been disposed by moving it.</exception>
        /// <exception cref="visa_exception">If any of the API calls to the
        /// instrument failed.</exception>
        rtx_instrument& history_segment(_In_ const int segment);

        /// <summary>
        /// Gets the number of history segments currently available in memory.
        /// </summary>
        /// <returns>The number of history segments in memory.</returns>
        /// <exception cref="std::runtime_error">If the method is called on an
        /// object that has been disposed by moving it.</exception>
        /// <exception cref="visa_exception">If any of the API calls to the
        /// instrument failed.</exception>
        /// <exception cref="std::logic_error">If the method is called while
        /// the library was compiled without support for VISA.</exception>
        std::size_t history_segments(void) const;

        /// <summary>
        /// Gets the current position of the reference point in the diagram.
        /// </summary>
        /// <returns>The location of the reference point on the horizontal
        /// axis.</returns>
        oscilloscope_reference_point reference_position(void) const;

        /// <summary>
        /// Sets the reference point in the diagram.
        /// </summary>
        /// <param name="position">The location of the reference point on the
        /// horizontal axis, which can be the left side, the middle or the
        /// right side.</param>
        /// <returns><c>*this</c>.</returns>
        rtx_instrument& reference_position(
            _In_ const oscilloscope_reference_point position);

        /// <summary>
        /// Gets the length of a single acquisition covering all horizontal grid
        /// divisions.
        /// </summary>
        /// <returns>The time range of the whole waveform displayed.</returns>
        oscilloscope_quantity time_range(void) const;

        /// <summary>
        /// Sets the time range of a single acquisition covering all grid
        /// divisions.
        /// </summary>
        /// <param name="scale">Time range within [250e-12, 500].</param>
        /// <returns><c>*this</c>.</returns>
        rtx_instrument& time_range(_In_ const oscilloscope_quantity& scale);

        /// <summary>
        /// Gets the length of a horizontal single grid division.
        /// </summary>
        /// <returns>The time scale of a single grid division.</returns>
        oscilloscope_quantity time_scale(void) const;

        /// <summary>
        /// Sets the horizontal scale for all channels in time units per grid
        /// division.
        /// </summary>
        /// <param name="scale">Time scale within [1e-9, 50].</param>
        /// <returns><c>*this</c>.</returns>
        rtx_instrument& time_scale(_In_ const oscilloscope_quantity& scale);

        /// <summary>
        /// Configures the trigger.
        /// </summary>
        /// <param name="trigger">The trigger configuration.</param>
        /// <returns><c>*this</c>.</returns>
        rtx_instrument& trigger(_In_ const oscilloscope_trigger& trigger);

        /// <summary>
        /// Forces a manual trigger.
        /// </summary>
        /// <param name="wait">If <c>true</c>, the trigger is combined with an
        /// <c>*OPC?</c> query that blocks the calling code until the instrument
        /// has actually triggered. This parameter defaults to <c>false</c>.
        /// </param>
        /// <returns><c>*this</c>.</returns>
        rtx_instrument& trigger(_In_ const bool wait = false);

        /// <summary>
        /// Convfigures which signal is generated on the auxilliary trigger
        /// output.
        /// </summary>
        /// <param name="output">The behaviour of the trigger output.</param>
        /// <returns><c>*this</c>.</returns>
        rtx_instrument& trigger_output(
            _In_ const oscilloscope_trigger_output output);

        /// <summary>
        /// Sets the trigger position, which is the time distance from the
        /// trigger point to the reference point.
        /// </summary>
        /// <remarks>
        /// <para>The trigger point is the horizontal origin of the diagram.
        /// Changing the horizontal position you can move the trigger, even
        /// outside the screen.</para>
        /// </remarks>
        /// <param name="offset">The offset of the trigger point.</param>
        /// <returns><c>*this</c>.</returns>
        rtx_instrument& trigger_position(
            _In_ const oscilloscope_quantity& offset);

        /// <summary>
        /// Sets the unit of the specified channel.
        /// </summary>
        /// <param name="channel">The number (starting at 1) of the channel to
        /// be configured.</param>
        /// <param name="unit">The unit being measured (either &quot;A&quot;
        /// or &quot;V&quot;).</param>
        /// <exception cref="std::invalid_argument">If <paramref name="unit" />
        /// is <c>nullptr</c>.</exception>
        /// <returns><c>*this</c>.</returns>
        rtx_instrument& unit(_In_ const std::uint32_t channel,
            _In_z_ const wchar_t *unit);

        /// <summary>
        /// Sets the unit of the specified channel.
        /// </summary>
        /// <param name="channel">The number (starting at 1) of the channel to
        /// be configured.</param>
        /// <param name="unit">The unit being measured (either &quot;A&quot;
        /// or &quot;V&quot;).</param>
        /// <exception cref="std::invalid_argument">If <paramref name="unit" />
        /// is <c>nullptr</c>.</exception>
        /// <returns><c>*this</c>.</returns>
        rtx_instrument& unit(_In_ const std::uint32_t channel,
            _In_z_ const char *unit);
    };

} /* namespace power_overwhelming */
} /* namespace visus */
