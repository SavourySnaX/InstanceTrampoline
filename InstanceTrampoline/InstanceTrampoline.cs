
using System.Runtime.InteropServices;

namespace InstanceTrampoline
{
    /// <summary>
    /// Static class that provides the interface to the native InstanceTrampoline library.
    /// </summary>
    public static class InterfaceTrampoline
    {
        /// <summary>
        /// Delegate for the allocate_trampoline method in the native library.
        /// </summary>
        /// <param name="instance">native pointer to the instance parameter you wish to have injected</param>
        /// <param name="numParams">number of parameters in the original function pointer</param>
        /// <param name="method">pointer to the native method to be wrapped</param>
        /// <returns>address of newly created trampoline</returns>
        public delegate nint Initialise(nint instance, int numParams, nint method);
        /// <summary>
        /// Delegate for the allocate_printer method in the native library.
        /// </summary>
        /// <param name="method">original printf method</param>
        /// <returns>address of newly created printf wrapper</returns>
        public delegate nint Printf(nint method);

        private static nint GetMethod(string method)
        {
            string instanceTrampolinePath = Path.Combine(Directory.GetCurrentDirectory(), "Native", "InstanceTrampoline", "build");
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                instanceTrampolinePath = Path.Combine(instanceTrampolinePath, "Debug", "InstanceTrampoline.dll");
            }
            else if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            {
                instanceTrampolinePath = Path.Combine(instanceTrampolinePath, "libInstanceTrampoline.so");
            }
            else
            {
                throw new Exception("Unsupported platform");
            }
            var lib = NativeLibrary.Load(instanceTrampolinePath);
            return NativeLibrary.GetExport(lib, method);
        }

        /// <summary>
        /// Get the delegate for the allocate_trampoline method in the native library.
        /// </summary>
        public static Initialise GetInitialise()
        {
            return Marshal.GetDelegateForFunctionPointer<Initialise>(GetMethod("allocate_trampoline"));
        }

        /// <summary>
        /// Get the delegate for the allocate_printer method in the native library.
        /// </summary>
        public static Printf GetPrintf()
        {
            return Marshal.GetDelegateForFunctionPointer<Printf>(GetMethod("allocate_printer"));
        }

    }
}