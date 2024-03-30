﻿// <copyright file="async_sampling.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <chrono>
#include <functional>
#include <type_traits>
#include <utility>

#if defined(_WIN32)
#include <Windows.h>
#else /* defined(_WIN32) */
#define CALLBACK
#endif /* defined(_WIN32) */

#include "power_overwhelming/measurement.h"
#include "power_overwhelming/measurement_data_series.h"
#include "power_overwhelming/tinkerforge_sensor_source.h"


namespace visus {
namespace power_overwhelming {

    /* Forward declarations. */
    class sensor;


    /// <summary>
    /// Configures the behaviour of ansychronous sampling of a sensor.
    /// </summary>
    /// <remarks>
    /// <para><see cref="async_sampling" /> is not thread-safe. It is intended
    /// to be configured once and then being passed to the <see cref="sensor" />
    /// where it remains unchanged until the sensor is reconfigured using a
    /// new instance of the class.</para>
    /// </remarks>
    class POWER_OVERWHELMING_API async_sampling final {

    public:

        /// <summary>
        /// The type of the  callback to be invoked if a
        /// <see cref="measurement" /> was generated by a sensor.
        /// </summary>
        /// <remarks>
        /// <para>This callback is only provided for backwards compatibility.
        /// It should be avoided as constructing an instance of
        /// <see cref="measurement" /> incurs overhead for heap allocations for
        /// the name of the sensor for every sample delivered. If possible, use
        /// the callback for <see cref="measurement_data" /> and relate it to
        /// its source by means of the <see cref="sensor" /> passed to the
        /// <see cref="on_measurement_data_callback" />.
        /// </remarks>
        typedef measurement_callback on_measurement_callback;

        /// <summary>
        /// The type to represent sampling intervals measured in microseconds.
        /// </summary>
        typedef std::uint64_t microseconds_type;

        /// <summary>
        /// The type of callback to be invoked if a sensor created one or more
        /// of the new &quot;pure&quot; <see cref="measurement_data" /> samples.
        /// </summary>
        typedef void (*on_measurement_data_callback)(_In_z_ const wchar_t *,
            _In_ const measurement_data *, _In_ const std::size_t,
            _In_opt_ void *);

        /// <summary>
        /// The default sampling interval, which is 5000 µs or 5 ms.
        /// </summary>
        static constexpr const microseconds_type default_interval = 5000;

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        /// <remarks>
        /// The new instance is initially configured to disable asynchronous
        /// sampling. Use the fluent API to add the appropriate callback and a
        /// user-defined context as necessary.
        /// </remarks>
        async_sampling(void);

        /// <summary>
        /// Move <paramref name="rhs" /> invalidating it.
        /// </summary>
        /// <param name="rhs">The object to be moved.</param>
        inline async_sampling(_Inout_ async_sampling&& rhs) noexcept
                : _context(nullptr), _context_deleter(nullptr) {
            *this = std::move(rhs);
        }

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        ~async_sampling(void) noexcept;

        /// <summary>
        /// Converts the object into an rvalue reference to move the result of a
        /// chain of method calls directly into <see cref="sensor::sample "/>.
        /// </summary>
        /// <remarks>
        /// The use case for this method is:
        /// <code>
        /// sensor.sample(async_sampling()
        ///     .delivers_measurements_to([](const measurement& m, void *) { })
        ///     .as_rvalue());
        /// </code>
        /// </remarks>
        /// <returns><c>*this</c> as an rvalue reference.</returns>
        inline async_sampling&& as_rvalue(void) noexcept {
            return std::move(*this);
        }

        /// <summary>
        /// Gets the user-defined context, if any, to be passed to the callback.
        /// </summary>
        /// <returns>The user-defined context pointer.</returns>
        inline _Ret_maybenull_ const void *context(void) const noexcept {
            return this->_context;
        }

        /// <summary>
        /// Gets the user-defined context, if any, to be passed to the callback.
        /// </summary>
        /// <returns>The user-defined context pointer.</returns>
        inline _Ret_maybenull_ void *context(void) noexcept {
            return this->_context;
        }

        /// <summary>
        /// Invoke the callback for a <see cref="measurement" /> or for
        /// <see cref="measurement_data" />, whichever is set, to deliver the
        /// given samples.
        /// </summary>
        /// <remarks>
        /// <para>This method is not thread-safe. Callers must make sure that
        /// the instance is not changed while the callback is invoked from the
        /// sampler thread.</para>
        /// <para>If the method must deliver an instance of
        /// <see cref="measurement" />, it will create this instance on the fly
        /// from the given <paramref name="sample" /> and the name of the given
        /// <paramref name="source" />.</para>
        /// </remarks>
        /// <param name="source">The name of the sensor from which the
        /// <paramref name="samples" /> originate. This must not be
        /// <c>nullptr</c>.</param>
        /// <param name="samples">A pointer to <paramref name="cnt" /> samples
        /// to deliver to the registered callback.</param>
        /// <param name="cnt">The number of samples to deliver.</param>
        /// <returns><c>true</c> if a callback was invoked, <c>false</c> if none
        /// has been set.</returns>
        bool deliver(_In_z_ const wchar_t *source,
            _In_reads_(cnt) const measurement_data *samples,
            _In_ const std::size_t cnt) const;

        /// <summary>
        /// Invoke the callback for a <see cref="measurement" /> or for
        /// <see cref="measurement_data" />, whichever is set, to deliver the
        /// given samples.
        /// </summary>
        /// <remarks>
        /// <para>This method is not thread-safe. Callers must make sure that
        /// the instance is not changed while the callback is invoked from the
        /// sampler thread.</para>
        /// <para>If the method must deliver an instance of
        /// <see cref="measurement" />, it will create this instance on the fly
        /// from the given <paramref name="sample" /> and the name of the given
        /// <paramref name="source" />.</para>
        /// </remarks>
        /// <param name="data">The measurement data to be delivered.</param>
        /// <returns><c>true</c> if a callback was invoked, <c>false</c> if none
        /// has been set.</returns>
        bool deliver(_In_ const measurement_data_series& data) const {
            const auto sensor = data.sensor();
            const auto samples = data.data();
            _Analysis_assume_(sensor != nullptr);
            _Analysis_assume_(samples != nullptr);
            return this->deliver(sensor, samples, data.size());
        }

        /// <summary>
        /// Invoke the callback for a <see cref="measurement" /> or for
        /// <see cref="measurement_data" />, whichever is set, to deliver the
        /// given sample.
        /// </summary>
        /// <param name="source">The name of the sensor from which the
        /// <paramref name="sample" /> originates. This must not be
        /// <c>nullptr</c>.</param>
        /// <param name="sample">The sample to deliver.</param>
        /// <returns><c>true</c> if a callback was invoked, <c>false</c> if none
        /// has been set.</returns>
        inline bool deliver(_In_ const wchar_t *source,
                _In_ const measurement_data& sample) const {
            return this->deliver(source, &sample, 1);
        }

        /// <summary>
        /// Configures the <see cref="sensor" /> such that it produces samples
        /// of type <see cref="measurement" />.
        /// </summary>
        /// <param name="callback">The callbeck to deliver to. If this is
        /// <c>nullptr</c>, sampling will be disabled (this is equivalent to
        /// calling <see cref="is_disabled" />).</param>
        /// <returns><c>*this</c>.</returns>
        async_sampling& delivers_measurements_to(
            _In_opt_ const on_measurement_callback callback) noexcept;

        /// <summary>
        /// Configures the <see cref="sensor" /> such that it produces samples
        /// of type <see cref="measurement_data" />.
        /// </summary>
        /// <param name="callback">The callbeck to deliver to. If this is
        /// <c>nullptr</c>, sampling will be disabled (this is equivalent to
        /// calling <see cref="is_disabled" />).</param>
        /// <returns><c>*this</c>.</returns>
        async_sampling& delivers_measurement_data_to(
            _In_opt_ const on_measurement_data_callback callback) noexcept;

        /// <summary>
        /// Configures the <see cref="sensor" /> to deliver samples of type
        /// <see cref="measurement_data" /> to a callable like a functor object
        /// or <see cref="std::function" />.
        /// </summary>
        /// <remarks>
        /// <para>The functor object is stored as the <see cref="context" /> of
        /// the callback. <b>Callers must not set their own
        /// <see cref="context" /> when registering a functor. The behaviour
        /// when doing so is undefined, ie the whole thing will most likely
        /// crash!</b></para>
        /// <para>Using this method incurs additional overhead for a heap
        /// allocation and for additional indirections when delivering a sample.
        /// If you can use a lambda without a capture, you should register it
        /// using <see cref="delivers_measurement_data_to" /> instead. A more
        /// efficient way to pass a single object to a free callback function
        /// pointer would be using <see cref="stores_and_passes_context" /> to
        /// create a copy of the context object that is managed by the
        /// <see cref="async_sampling" /> object. This could look like:
        /// <code>
        /// std::wstring context(L"Bla, bla, bla, Mr Freeman");
        /// config.stores_and_passes_context(std::move(context))
        ///     .delivers_measurement_data_to(
        ///             [](const wchar_t *sensor,
        ///             const measurement_data *data,
        ///             const std::size_t cnt,
        ///             void *context) {
        ///         auto bla = static_cast<std::wstring *>(context);
        ///     });
        /// </code>
        /// This method avoids the additional indirection when invoking the
        /// callback that is required when dereferencing the functor object to
        /// invoke it.
        /// </para>
        /// <para>There is not variant for <see cref="measurement" /> instances,
        /// because no backwards compatibility is required for functors and it
        /// is discouraged to use <see cref="measurement" />-based callbacks for
        /// performance reasons.</para>
        /// </remarks>
        /// <typeparam name="TFunctor">The type of the functor being called.
        /// This must be a functional accepting a <see cref="sensor" /> and
        /// <see cref="measurement_data" /> returning <c>void</c>. This type
        /// must be convertible to
        /// <c>std::function<void(const sensor&amp;, const measurement_data *, const std::size_t)></c>.
        /// Note that you cannot pass a context to your callback here, because
        /// the context is reserved to store the <c>std::function</c> itself. If
        /// you need contextual information, you have to use a lambda capture.
        /// </typeparam>
        /// <param name="callback"></param>
        /// <returns><c>*this</c>.</returns>
        template<class TFunctor>
        async_sampling& delivers_measurement_data_to_functor(
            _In_ TFunctor&& callback);

        /// <summary>
        /// If the sensor this sampling configuration is passed to is a
        /// Tinkerforge sensor, instructs the sensor to obtain data only from
        /// the specified sources.
        /// </summary>
        /// <remarks>
        /// If the sensor is not a Tinkerforge sensor, this setting has no
        /// effect.
        /// </remarks>
        /// <param name="source">A bitmask of the sources that should be
        /// returned.</param>
        /// <returns><c>*this</c>.</returns>
        async_sampling& from_source(
            _In_ const tinkerforge_sensor_source source) noexcept;

        /// <summary>
        /// Answer the requested sampling interval in microseconds.
        /// </summary>
        /// <returns>The sampling interval in microseconds.</returns>
        inline microseconds_type interval(void) const noexcept {
            return this->_interval;
        }

        /// <summary>
        /// Sets the desired sampling interval.
        /// </summary>
        /// <param name="interval">The desired sampling interval in
        /// microseconds.</param>
        /// <returns><c>*this</c>.</returns>
        inline async_sampling& interval(
                _In_ const microseconds_type interval) noexcept {
            return this->samples_every(interval);
        }

        /// <summary>
        /// Configures the asynchronous sampling such that it is disabled by
        /// removing all previously configured callbacks.
        /// </summary>
        /// <returns><c>*this</c>.</returns>
        async_sampling& is_disabled(void) noexcept;

        /// <summary>
        /// Configures the asynchronous sampler to pass the given context
        /// pointer around with every sample.
        /// </summary>
        /// <param name="context">A user-defined context pointer to be passed
        /// to the sampling callback. The caller is responsible for making sure
        /// that this pointer remains valid as long as the sensor is producing
        /// samples.</param>
        /// <returns><c>*this</c>.</returns>
        async_sampling& passes_context(_In_opt_ void *context) noexcept;

        /// <summary>
        /// Answer the minimum time span a sampler thread should give other
        /// threads the ability to modify the sensor list.
        /// </summary>
        /// <returns>The minimum sleep time in microseconds.</returns>
        inline microseconds_type minimum_sleep(void) const noexcept {
            return this->_minimum_sleep;
        }

        /// <summary>
        /// Instructs any active asynchronous sampling thread to sleep at least
        /// for the specified number of microseconds to give other threads
        /// (mostly the main thread) time to modify the sensor list to be
        /// sampled.
        /// </summary>
        /// <param name="minimum_sleep">The minimum sleep time in microseconds.
        /// You can set zero to enforce the sampling interval even if the
        /// sensors are slow, but in this case, it will be very hard to
        /// terminate the sampler thread.</param>
        /// <returns><c>*this</c></returns>
        async_sampling& minimum_sleep(
                _In_ const microseconds_type minimum_sleep) noexcept {
            return this->must_sleep_at_least(minimum_sleep);
        }

        /// <summary>
        /// Instructs any active asynchronous sampling thread to sleep at least
        /// for the specified number of microseconds to give other threads
        /// (mostly the main thread) time to modify the sensor list to be
        /// sampled.
        /// </summary>
        /// <param name="minimum_sleep">The minimum sleep time in microseconds.
        /// You can set zero to enforce the sampling interval even if the
        /// sensors are slow, but in this case, it will be very hard to
        /// terminate the sampler thread.</param>
        /// <returns><c>*this</c></returns>
        async_sampling& must_sleep_at_least(
                _In_ const microseconds_type minimum_sleep) noexcept {
            this->_minimum_sleep = minimum_sleep;
            return *this;
        }

        /// <summary>
        /// Instructs any active asynchronous sampling thread to sleep at least
        /// for the specified number of microseconds to give other threads
        /// (mostly the main thread) time to modify the sensor list to be
        /// sampled.
        /// </summary>
        /// <typeparam name="TValue">The type of the counter.</typeparam>
        /// <typeparam name="TPeriod">The period of the duration.</typeparam>
        /// <param name="interval">The minimal sleep duration for the
        /// sampler thread.</param>
        /// <returns><c>*this</c>.</returns>
        template<class TValue, class TPeriod>
        inline async_sampling& must_sleep_at_least(
                _In_ const std::chrono::duration<TValue, TPeriod> interval) {
            return this->must_sleep_at_least(std::chrono::duration_cast<
                std::chrono::microseconds>(interval).count());
        }

        /// <summary>
        /// Answer whether the <see cref="sensor" /> should produce samples of
        /// type <see cref="measurement" />.
        /// </summary>
        /// <returns></returns>
        inline bool on_measurement(void) const noexcept {
            return (this->_on_measurement != nullptr);
        }

        /// <summary>
        /// Configures the <see cref="sensor" /> such that it produces samples
        /// of type <see cref="measurement" />.
        /// </summary>
        /// <param name="callback">The callbeck to deliver to. If this is
        /// <c>nullptr</c>, sampling will be disabled (this is equivalent to
        /// calling <see cref="is_disabled" />).</param>
        /// <returns><c>*this</c>.</returns>
        inline async_sampling& on_measurement(
                _In_opt_ const on_measurement_callback callback) noexcept {
            return this->delivers_measurements_to(callback);
        }

        /// <summary>
        /// Answer whether the <see cref="sensor" /> should produce samples of
        /// type <see cref="measurement_data" />.
        /// </summary>
        /// <returns></returns>
        inline bool on_measurement_data(void) const noexcept {
            return (this->_on_measurement_data != nullptr);
        }

        /// <summary>
        /// Configures the <see cref="sensor" /> such that it produces samples
        /// of type <see cref="measurement_data" />.
        /// </summary>
        /// <param name="callback">The callbeck to deliver to. If this is
        /// <c>nullptr</c>, sampling will be disabled (this is equivalent to
        /// calling <see cref="is_disabled" />).</param>
        /// <returns><c>*this</c>.</returns>
        inline async_sampling& on_measurement_data(
                _In_opt_ const on_measurement_data_callback callback) noexcept {
            return this->delivers_measurement_data_to(callback);
        }

        /// <summary>
        /// Sets the desired sampling interval.
        /// </summary>
        /// <param name="interval">The desired sampling interval in
        /// microseconds.</param>
        /// <returns><c>*this</c>.</returns>
        async_sampling& samples_every(
            _In_ const microseconds_type interval) noexcept;

        /// <summary>
        /// Sets the desired sampling interval.
        /// </summary>
        /// <typeparam name="TValue">The type of the counter.</typeparam>
        /// <typeparam name="TPeriod">The period of the duration.</typeparam>
        /// <param name="interval">The desired sampling interval in the unit
        /// encoded by the type of the parameter.</param>
        /// <returns><c>*this</c>.</returns>
        template<class TValue, class TPeriod>
        inline async_sampling& samples_every(
                _In_ const std::chrono::duration<TValue, TPeriod> interval) {
            return this->samples_every(std::chrono::duration_cast<
                std::chrono::microseconds>(interval).count());
        }

        /// <summary>
        /// Stores <paramref name="context" /> as the context of the callback
        /// and changes ownership of the context to the object, which will use
        /// <paramref name="context_deleter" /> to free it.
        /// </summary>
        /// <remarks>
        /// <para>This method can be used to create &quot;fire-and-forget&quot;
        /// configurations that manage context objects on the heap by
        /// themselves. An example for using it would be:
        /// <code>
        /// config.stores_and_passes_context(::malloc(42), ::free);
        /// </code>
        /// Typically, you would not just allocate typeless memory on the heap
        /// here, but construct and destruct objects. To facilitate this use
        /// case, there are templated overloads that accept context objects and
        /// create deleters that destruct and free those objects on your behalf.
        /// </para>
        /// </remarks>
        /// <param name="context">The context to be passed to the callback. The
        /// object takes ownership of this object provided that
        /// <paramref name="context_deleter" /> is not <c>nullptr</c>.</param>
        /// <param name="context_deleter">The deleter that will free
        /// <paramref name="context" /> once it is no longer needed.</param>
        /// <returns><c>*this</c>.</returns>
        async_sampling& stores_and_passes_context(_In_ void *context,
            _In_ void (CALLBACK *context_deleter)(void *));

        /// <summary>
        /// Creates and stores a copy of <paramref name="context" /> and passes
        /// it to the registered callback. Once the object is deleted or a new
        /// context is set, the object will free the copy.
        /// </summary>
        /// <typeparam name="TContext">The type of the context object, which
        /// must be copyable.</typeparam>
        /// <param name="context">The context object to be copied.</param>
        /// <returns><c>*this</c>.</returns>
        template<class TContext>
        async_sampling& stores_and_passes_context(_In_ const TContext& context);

        /// <summary>
        /// Moves the given <paramref name="context" /> into the object. The
        /// object will pass the context to the registered callback and free it
        /// once it is deleted or before a new context is set.
        /// </summary>
        /// <typeparam name="TContext">The type of the context object, which
        /// must be movable.</typeparam>
        /// <param name="context">The context to be moved.</param>
        /// <returns><c>*this</c>.</returns>
        template<class TContext>
        async_sampling& stores_and_passes_context(_In_ TContext&& context);

        /// <summary>
        /// Answer the Tinkerforge sensor data to obtain in case the sensor
        /// is a Tinkerforge sensor.
        /// </summary>
        /// <returns>A bitmask holding the sensors to retrieve.</returns>
        inline power_overwhelming::tinkerforge_sensor_source
        tinkerforge_sensor_source(void) const noexcept {
            return this->_tinkerforge_sensor_source;
        }

        /// <summary>
        /// Move <paramref name="rhs" /> invalidating it.
        /// </summary>
        /// <param name="rhs">The object to be moved.</param>
        /// <returns><c>*this</c>.</returns>
        async_sampling& operator =(_Inout_ async_sampling&& rhs) noexcept;

        /// <summary>
        /// Answer whether the configuration enables asynchronous sampling.
        /// </summary>
        /// <returns><c>true</c> if asynchronous sampling is enabled,
        /// <c>false</c> otherwise.</returns>
        inline operator bool(void) const noexcept {
            return (this->on_measurement() || this->on_measurement_data());
        }

    private:

        void *_context;
        void (CALLBACK *_context_deleter)(void *);
        microseconds_type _interval;
        microseconds_type _minimum_sleep;
        on_measurement_callback _on_measurement;
        on_measurement_data_callback _on_measurement_data;
        power_overwhelming::tinkerforge_sensor_source
            _tinkerforge_sensor_source;
    };

} /* namespace power_overwhelming */
} /* namespace visus */

#include "power_overwhelming/async_sampling.inl"
