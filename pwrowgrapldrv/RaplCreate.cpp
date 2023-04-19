// <copyright file="RaplCreate.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include <intrin.h>

#include "RaplDriver.h"
#include "RaplString.h"
#include "RaplThreadAffinity.h"


/// <summary>
/// Possible CPU vendors.
/// </summary>
/// <remarks>
/// At this point, we are only interested whether the CPU is AMD, Intel or any
/// other, because others do not support RAPL MSRs.
/// </remarks>
enum class RaplCpuVendor {
    Amd,
    Intel,
    Other
};


/// <summary>
/// A structure holding all necessary information about the CPU we are running
/// on.
/// </summary>
struct RaplCpuInfo {
    __int8 BaseFamily;
    __int8 ExtendedFamily;
    __int8 BaseModel;
    __int8 ExtendedModel;
    __int8 Stepping;
    RaplCpuVendor Vendor;
};


/// <summary>
/// Retrieve all members of <see cref="RaplCpuInfo" /> for the CPU the calling
/// thread is running on.
/// </summary>
/// <param name="dst">A pointer to a <see cref="RaplCpuInfo" /> to retrieve
/// the CPU vendor and model information.</param>
/// <returns><c>STATUS_SUCCESS</c> in case of success,
/// <c>STATUS_INVALID_PARAMETER</c> if <paramref name="dst" /> is
/// <c>nullptr</c>,
/// <c>STATUS_UNSUCCESSFUL</c> if the CPU does not allow for retrieving the
/// family and model.</returns>
NTSTATUS RaplIdentifyCpu(_In_ RaplCpuInfo *dst) {
    __int32 info[4];

    NTSTATUS status = (dst != nullptr)
        ? STATUS_SUCCESS
        : STATUS_INVALID_PARAMETER;

    if (NT_SUCCESS(status)) {
        ::__cpuid(info, 0);
        if (info[0] < 2) {
            // We need to be able to retrieve the family and model, which is at
            // index 1.
            status = STATUS_UNSUCCESSFUL;
        }
    }

    if (NT_SUCCESS(status)) {
        char vendor[0x20];
        ::RtlZeroMemory(vendor, sizeof(vendor));

        // Kansas city shuffle.
        *reinterpret_cast<int *>(vendor + 0) = info[1];
        *reinterpret_cast<int *>(vendor + 4) = info[3];
        *reinterpret_cast<int *>(vendor + 8) = info[2];

#define _IS_VENDOR(v) (::strncmp(vendor, (v), sizeof(vendor)) == 0)
        if (_IS_VENDOR("AuthenticAMD") || _IS_VENDOR("AMDisbetter!")) {
            dst->Vendor = RaplCpuVendor::Amd;
        } else if (_IS_VENDOR("GenuineIntel")) {
            dst->Vendor = RaplCpuVendor::Intel;
        } else {
            dst->Vendor = RaplCpuVendor::Other;
        }
#undef _IS_VENDOR
    }

    if (NT_SUCCESS(status)) {
        ::__cpuid(info, 1);
        dst->Stepping = (info[0] & 0x0000000F) >> 0;
        dst->BaseModel = (info[0] & 0x000000F0) >> 4;
        dst->BaseFamily = (info[0] & 0x00000F00) >> 8;
        dst->ExtendedModel = (info[0] & 0x0000F0000) >> 16;
        dst->ExtendedFamily = (info[0] & 0x00FF00000) >> 20;
    }

    return status;
}


/// <summary>
/// The framework calls the driver's <c>EvtDeviceFileCreate</c> callback when
/// it receives an <c>IRP_MJ_CREATE</c> request.
/// </summary>
/// <remarks>
/// <para>Cf. https://github.com/microsoft/Windows-driver-samples/blob/f28183b782d1f113492f6eea424172f2addaf565/general/ioctl/kmdf/sys/nonpnp.c#L400
/// </para>
/// <para>The system sends this request when a user application opens the device
/// to perform an I/O operation, such as reading or writing a file. This
/// callback is called synchronously, in the context of the thread that created
/// the <c>IRP_MJ_CREATE</c> request.</para>
/// <param name="device"></param>
/// <param name="request"></param>
/// <param name="fileObject"></param>
extern "C" void RaplCreate(_In_ WDFDEVICE device, _In_ WDFREQUEST request,
        _In_ WDFFILEOBJECT fileObject) noexcept {
    ASSERT(fileObject != nullptr);
    PAGED_CODE();

    PRAPL_FILE_CONTEXT context = nullptr;
    __int32 core = 0;
    RaplCpuInfo cpuInfo { 0 };
    GROUP_AFFINITY originalAffinity { 0 };
    WDF_REQUEST_PARAMETERS parameters { 0 };
    NTSTATUS status = STATUS_SUCCESS;

    // Retrieve the parameters and make sure that we accept nothing but opening
    // existing files for reading them.
    ::WdfRequestGetParameters(request, &parameters);
    //parameters.Parameters.Create.

    // Parsed filename has "\" in the begining. The object manager strips all
    // "\" except one, after the device name.
    const auto fileName = ::WdfFileObjectGetFileName(fileObject);
    KdPrint(("[PWROWG] Open %wZ\"\r\n", fileName));

    // The file name must be the number of the core, i.e. an integral number in
    // the decimal system. If the path is empty or if it contains anything but a
    // number, this is illegal and we reject this as "file not found".
    if (NT_SUCCESS(status)) {
        if (fileName->Length < 2) {
            KdPrint(("[PWROWG] Cannot open a volume.\r\n"));
            status = STATUS_NOT_FOUND;
        }
    }

    if (NT_SUCCESS(status)) {
        ASSERT(fileName->Length >= 2);
        ASSERT(core == 0);
        const auto end = ::RaplStringEnd(fileName);
        // See above: path starts with exactly one "\".
        for (auto d = fileName->Buffer + 1; d < end; ++d) {
            if (::iswdigit(*d)) {
                core *= 10;
                core += *d - L'0';

            } else {
                KdPrint(("[PWROWG] Non-digit found in file name.\r\n"));
                d = end;
                status = STATUS_NOT_FOUND;
            }
        }
    }

    // Bind the calling thread to the CPU which of the RAPL registers have been
    // requested. This way, we make sure that the CPUID instructions return data
    // on the CPU we are interested in.
    if (NT_SUCCESS(status)) {
        status = ::RaplSetThreadAffinity(core, &originalAffinity);
    }
    const auto restoreAffinity = NT_SUCCESS(status);

    // Obtain the CPU vendor and model, which allows us to find out whether the
    // requested core supports RAPL MSRs. Afterwards, we restore the thread
    // affinity to its original state.
    if (NT_SUCCESS(status)) {
        status = ::RaplIdentifyCpu(&cpuInfo);
    }

    if (restoreAffinity) {
        ::KeSetSystemGroupAffinityThread(&originalAffinity, nullptr);
    }

    // Next, make sure that the CPU is by AMD or Intel.
    if (NT_SUCCESS(status)) {
        switch (cpuInfo.Vendor) {
            case RaplCpuVendor::Amd:
                // TODO: retrieve the valid offsets for AMD
                break;

            case RaplCpuVendor::Intel:
                // TODO: retrieve the vaid fofsets for Intel.
                break;

            default:
                // Other vendors do not have the RAPL MSRs, so we tell the
                // caller that we could not find it.
                status = STATUS_NOT_FOUND;
                break;
        }
    }

    // Allocate a context and store what we need to read the registers.
    if (NT_SUCCESS(status)) {
        WDF_OBJECT_ATTRIBUTES attributes { 0 };
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, RAPL_FILE_CONTEXT);
        status = ::WdfObjectAllocateContext(fileObject, &attributes, 
            reinterpret_cast<void **>(&context));
    }

    if (NT_SUCCESS(status)) {
        context->Core = core;
        context->CountMsrs = 0;
        context->Msrs = nullptr;
    }

    KdPrint(("[PWROWG] Complete open with 0x%x\r\n", status));
    ::WdfRequestComplete(request, status);
}
