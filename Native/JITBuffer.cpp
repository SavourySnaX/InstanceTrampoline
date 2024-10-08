#include <memory.h>
#include <cassert>
#include <stdlib.h>
#include "JITBuffer.h"

#if defined(BUILD_WINDOWS_X64)
#include "NativeTrampolineWindowsX64.h"
    #define PLATFORM_TRAMPOLINE_IMPLEMENTATION  NativeTrampolineWindowsX64Implementation
#elif defined(BUILD_MACOS_ARM64) || defined(BUILD_LINUX_ARM64) || defined(BUILD_WINDOWS_ARM64)
#include "NativeTrampolineSystemVAarch64.h"
    #define PLATFORM_TRAMPOLINE_IMPLEMENTATION  NativeTrampolineSystemVAarch64Implementation
#elif defined(BUILD_MACOS_X64) || defined(BUILD_LINUX_X64)
#include "NativeTrampolineSystemVX64.h"
    #define PLATFORM_TRAMPOLINE_IMPLEMENTATION  NativeTrampolineSystemVX64Implementation
#else
    #error Unimplemented Trampoline Implementation for platform
#endif

uint8_t* JitAllocateMemory(size_t sizeInBytes)
{
#if defined(BUILD_USES_WIN32)
    return (uint8_t*)VirtualAlloc(NULL, sizeInBytes, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#elif defined(BUILD_USES_MMAP)
    int mmFlags = MAP_ANON | MAP_PRIVATE;
# if defined(BUILD_USES_JITPAGES)
    mmFlags |= MAP_JIT;
#endif
    return (uint8_t*)mmap(NULL, sizeInBytes, PROT_READ | PROT_WRITE | PROT_EXEC, mmFlags, 0, 0);
#else
#error Unimplemented Allocator for current platform
#endif
}

void JitBeginWriteToMemory()
{
#if defined(BUILD_USES_MMAP) && defined(BUILD_USES_JITPAGES)
    // Unlock page for apple silicon (Make writable)
    pthread_jit_write_protect_np(false);
#endif
}

void JitWriteToMemory(uint8_t* destinationPtr, const void* sourcePtr, size_t length)
{
    memcpy(destinationPtr, sourcePtr, length);
}

void JitEndWriteToMemory(uint8_t* destinationPtr, size_t length)
{
#if defined(BUILD_USES_MMAP) && defined(BUILD_USES_JITPAGES)
    // Relock page for apple silicon (make executable)
    pthread_jit_write_protect_np(true);

    sys_icache_invalidate(destinationPtr, length);
#endif
}

void JitFreeMemory(uint8_t* memory, size_t sizeInBytes)
{
#if defined(BUILD_USES_WIN32)
    VirtualFree(memory, 0, MEM_RELEASE);
#elif defined(BUILD_USES_MMAP)
    munmap(memory, sizeInBytes);
#else
#error Unimplemented Free for current platform
#endif
}

JITBuffer::JITBuffer(size_t blockSize)
{
    jitAllocation = blockSize;
    jitBuffer = JitAllocateMemory(blockSize);
    jitPosition = 0;
}

JITBuffer::~JITBuffer()
{
    JitFreeMemory(jitBuffer, jitAllocation);
}

void* JITBuffer::AllocatePointer(const void* value) 
{
    StartBlock();
    CopyBlock(&value,8);
    return EndBlock();
}

void* JITBuffer::Allocate(size_t size)
{
    StartBlock();
    void* result = jitBuffer+jitPosition;
    jitPosition+=size;
    return EndBlock();
}

void* JITBuffer::AllocateGlobalData(size_t size,size_t alignment)
{
#if defined(BUILD_USES_WIN32)
    return _aligned_malloc(size, alignment);
#else
    return aligned_alloc(alignment, size);
#endif
}

void JITBuffer::Skip(size_t length)
{
    assert(jitPosition+length < jitAllocation && "Initial allocation for jit buffer is too small");
    uint8_t* dest = jitBuffer + jitPosition;
    jitPosition+=length;
}

void JITBuffer::StartBlock()
{
    jitBufferStartLoc=jitPosition;
    JitBeginWriteToMemory();
}

void JITBuffer::CopyBlock(const void* source, size_t length)
{
    assert(jitPosition+length < jitAllocation && "Initial allocation for jit buffer is too small");
    uint8_t* dest = jitBuffer + jitPosition;
    JitWriteToMemory(dest, source, length);
    jitPosition+=length;
}

void* JITBuffer::EndBlock()
{
    JitEndWriteToMemory(jitBuffer + jitBufferStartLoc, jitPosition - jitBufferStartLoc);
    return jitBuffer+jitBufferStartLoc;
}

void JITBuffer::AllocateStackSpace(int8_t slots)
{
    PLATFORM_TRAMPOLINE_IMPLEMENTATION::AllocateStackSpace(slots,*this);
}

void JITBuffer::FreeStackSpace(int8_t slots)
{
    PLATFORM_TRAMPOLINE_IMPLEMENTATION::FreeStackSpace(slots,*this);
}

void JITBuffer::CopySlotToNextSlot(const int stackSlot, const int numParams)
{
    PLATFORM_TRAMPOLINE_IMPLEMENTATION::CopySlotToNextSlot(stackSlot,numParams,*this);
}

void JITBuffer::SetThis(void* ptr)
{
    PLATFORM_TRAMPOLINE_IMPLEMENTATION::SetThis(ptr,*this);
}

void JITBuffer::Return()
{
    PLATFORM_TRAMPOLINE_IMPLEMENTATION::Return(*this);
}

void JITBuffer::CallMemberFunc(void* staticFuncWithMemberPointer)
{
    PLATFORM_TRAMPOLINE_IMPLEMENTATION::CallMemberFunc(staticFuncWithMemberPointer,*this);
}

void JITBuffer::SetTemp0From(void *wrapper)
{
    PLATFORM_TRAMPOLINE_IMPLEMENTATION::SetTemp0From(wrapper,*this);
}

void JITBuffer::MoveParam0ToTemp0Offset(int offset)
{
    PLATFORM_TRAMPOLINE_IMPLEMENTATION::MoveParam0ToTemp0Offset(offset,*this);
}

void JITBuffer::MoveTemp0ToParam0()
{
    PLATFORM_TRAMPOLINE_IMPLEMENTATION::MoveTemp0ToParam0(*this);
}

void JITBuffer::Jmp(void* destination)
{
    PLATFORM_TRAMPOLINE_IMPLEMENTATION::Jmp(destination,*this);
}