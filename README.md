# Instance Trampoline

A simple library which allows wrapping native calls that could otherwise be tricky in C#.

Primarily developed to allow usage of LibRetro endpoints from C#.


# usage

Here is how I use it to wrap the raylib audio callback, which in the c# library provides
a function pointer, without an instance parameter (which would make it tricky to play
multiple streams from C#).  The sample is incomplete, and just provides enough information
to show how a 2 parameter function pointer, gets modified with a hidden parameter.

```csharp

/*

 RayLib CS audio callbacks have no way to track an instance (for multiple games for instance). 
 This class wraps the audio system and along with a trampoline dll, allows multiple instances to be tracked.

*/

internal class RayLibAudioHelper
{
    struct AudioShared
    {
        public nint audioBuffer;
    }

    private unsafe delegate* unmanaged[Cdecl]<void*, void*, uint, void> audioCallback;
    private unsafe delegate* unmanaged[Cdecl]<void*, uint, void> audioCallbackTrampoline;
    private nint trampoline;
    private nint audioSharedData;

    public RayLibAudioHelper()
    {
        audioSharedData = Marshal.AllocHGlobal(Marshal.SizeOf<AudioShared>());
        var initialise = InstanceTrampoline.InterfaceTrampoline.GetInitialise();

        unsafe
        {
            audioCallback = &RayLibAudioCallback;
            trampoline = initialise(audioSharedData, 2, (nint)audioCallback);
            audioCallbackTrampoline = (delegate* unmanaged[Cdecl]<void*,uint, void>)trampoline;
        }

    }


    [UnmanagedCallersOnly(CallConvs = new Type[] { typeof(CallConvCdecl) })]
    private unsafe static void RayLibAudioCallback(void* instance, void* ptr, uint size)
    {
        var audioShared = (AudioShared*)instance;
        //.....
    }

}


```
