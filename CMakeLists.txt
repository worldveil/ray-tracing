cmake_minimum_required(VERSION 3.14)
project(Tracer CXX)

set(CMAKE_CXX_STANDARD          11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    # Enable warnings
    add_compile_options("/W4")

    # Be more standard compliant
    add_compile_options(/permissive-)

    # Always generate debug symbols
    add_compile_options("/Zi")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE       "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

    # Windows API version
    add_compile_definitions("_WIN32_WINNT=0x0601" "WINVER=0x0601" # Windows 7 compatibility
                            "WIN32_LEAN_AND_MEAN")
    
    # optimize compilation
    add_compile_options("/Ox")
    
else ()
    # Enable warnings
    add_compile_options("-Wall" "-Wextra")

    # Always generate good debug symbols
    add_compile_options("-g3")

    # Optimize compilation
    add_compile_options("-O3")
endif ()

add_executable(tracer src/render.cpp)
target_include_directories(tracer PRIVATE lib)