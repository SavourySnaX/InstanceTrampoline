
using System.Runtime.InteropServices;

namespace InstanceTrampoline
{
    /// <summary>
    /// Static class that provides the interface to the native InstanceTrampoline library.
    /// </summary>
    public static class InterfaceTrampoline
    {
        /// <summary>
        /// Get the delegate for the allocate_trampoline method in the native library.
        /// </summary>
        [DllImport("InstanceTrampolineNative",CallingConvention = CallingConvention.Cdecl,EntryPoint = "allocate_trampoline")]
        public extern static nint AllocateTrampoline(nint instance, int numParams, nint method);

        /// <summary>
        /// Get the delegate for the allocate_printer method in the native library.
        /// </summary>
        [DllImport("InstanceTrampolineNative", CallingConvention = CallingConvention.Cdecl, EntryPoint = "allocate_printer")]
        public extern static nint AllocatePrinter(nint method);

    }
}
