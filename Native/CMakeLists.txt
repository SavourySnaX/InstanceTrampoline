cmake_minimum_required(VERSION 3.0.0)
project(InstanceTrampolineNative VERSION 0.1.1)

list(APPEND SOURCES "JITBuffer.cpp" "NativeInterface.cpp" "NativeTrampoline.cpp")
list(APPEND SOURCES "JITBuffer.h" "NativeInterface.h")
list(APPEND SOURCES "NativeTrampoline.h" "platform.h")
list(APPEND SOURCES "NativeTrampolineSystemVAarch64.h" "NativeTrampolineSystemVX64.h" "NativeTrampolineWindowsX64.h")

add_library(InstanceTrampolineNative SHARED ${SOURCES})

if (WIN32)
	set_property(TARGET InstanceTrampolineNative PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set_property(TARGET InstanceTrampolineNative PROPERTY CXX_STANDARD 17)

list(APPEND SOURCES "Test.cpp")

add_executable(TestInstanceTrampoline ${SOURCES})
