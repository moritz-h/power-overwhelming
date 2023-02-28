// <copyright file="library_base.h" company="Visualisierungsinstitut der Universit�t Stuttgart">
// Copyright � 2021 Visualisierungsinstitut der Universit�t Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph M�ller</author>

#pragma once

#include <array>
#include <system_error>

#if defined(_WIN32)
#include <windows.h>
#endif /* defined(_WIN32) */


namespace visus {
namespace power_overwhelming {
namespace detail {

    /// <summary>
    /// Base class for managing libraries that are lazily loaded.
    /// </summary>
    /// <remarks>
    /// Rationale: If we statically link against vendor-specific management
    /// libraries, applications will only work if all of these libraries are
    /// available on the system, ie. if the system has NVIDIA and AMD GPUs. This
    /// is highly unlikely. Explicitly loading the libraries on demand solves
    /// this issue.
    /// </remarks>
    class library_base {

    public:

#if defined(_WIN32)
        typedef HMODULE handle_type;
#else /* defined(_WIN32) */
        typedef void *handle_type;
#endif /* defined(_WIN32) */

        library_base(const library_base&) = delete;

        library_base(library_base&& rhs) noexcept;

        ~library_base(void);

        library_base& operator =(const library_base& rhs) = delete;

        library_base& operator =(library_base&& rhs) noexcept;

        inline operator bool(void) const noexcept {
            return (this->_handle != NULL);
        }

    protected:

        library_base(handle_type && handle);

        library_base(const TCHAR *path);

        template<class... TPaths> library_base(TPaths&&... paths);

        FARPROC get_function(const char *name);

        template<class TFunction>
        inline TFunction get_function(const char *name) {
            return reinterpret_cast<TFunction>(this->get_function(name));
        }

    private:

        handle_type _handle;
    };

} /* namespace detail */
} /* namespace power_overwhelming */
} /* namespace visus */


/*
 * visus::power_overwhelming::detail::library_base::library_base
 */
template<class... TPaths>
visus::power_overwhelming::detail::library_base::library_base(
        TPaths&&... paths) : _handle(NULL) {
    std::array<const TCHAR *, sizeof...(TPaths)> ps = { paths... };

    for (auto& p : ps) {
        this->_handle = ::LoadLibrary(p);
        if (this->_handle != NULL) {
            break;
        }
    }

    if (this->_handle == NULL) {
        throw std::system_error(::GetLastError(), std::system_category());
    }
}

