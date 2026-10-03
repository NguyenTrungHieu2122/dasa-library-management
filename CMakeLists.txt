cmake_minimum_required(VERSION 3.16)
project(LibraryManagement LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE LIBRARY_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(library_management ${LIBRARY_SOURCES})
target_include_directories(library_management PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_custom_command(TARGET library_management POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data" "$<TARGET_FILE_DIR:library_management>/data"
)

if(MSVC)
    target_compile_options(library_management PRIVATE /W4 /utf-8)
else()
    target_compile_options(library_management PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_property(TARGET library_management PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(dsa_benchmark
    tests/Benchmark.cpp
    src/dsa_core/models/Book.cpp
    src/dsa_core/models/BookCopy.cpp
    src/dsa_core/models/Loan.cpp
    src/dsa_core/structures/BST.cpp
    src/dsa_core/structures/HashTable.cpp
)
target_include_directories(dsa_benchmark PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
if(MSVC)
    target_compile_options(dsa_benchmark PRIVATE /W4 /utf-8)
else()
    target_compile_options(dsa_benchmark PRIVATE -Wall -Wextra -Wpedantic)
endif()
