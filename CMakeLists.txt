cmake_minimum_required(VERSION 3.16)

project(LibraryManagement LANGUAGES CXX)

# =========================
# 1. C++ standard
# =========================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =========================
# 2. Core source files
#    Tự động lấy các file .cpp trong dsa_core và persistence
# =========================
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dsa_core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/persistence/*.cpp"
)

# Tạo thư viện lõi của chương trình
add_library(library_core STATIC
    ${CORE_SOURCES}
)

# Cho phép các file include theo đường dẫn trong src
target_include_directories(library_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dsa_core"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/persistence"
)

# Nếu máy đã cài nlohmann_json thì tự liên kết.
# Nếu nhóm để json.hpp trong project hoặc compiler đã tìm thấy được,
# chương trình vẫn có thể build mà không cần package này.
find_package(nlohmann_json QUIET)

if(nlohmann_json_FOUND)
    target_link_libraries(library_core PUBLIC nlohmann_json::nlohmann_json)
endif()

# =========================
# 3. Main program
# =========================
add_executable(LibraryManagement
    src/main.cpp
)

target_link_libraries(LibraryManagement PRIVATE
    library_core
)

# =========================
# 4. Copy thư mục data sang thư mục build
#    để chương trình có thể đọc JSON khi chạy
# =========================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data")
    add_custom_command(
        TARGET LibraryManagement POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/data"
                "$<TARGET_FILE_DIR:LibraryManagement>/data"
        COMMENT "Copying data folder..."
    )
endif()

# =========================
# 5. Tests
#    Mặc định OFF để build chương trình chính đơn giản.
#    Khi cần test:
#    cmake -S . -B build -DBUILD_PROJECT_TESTS=ON
# =========================
option(BUILD_PROJECT_TESTS "Build project test files" OFF)

if(BUILD_PROJECT_TESTS)
    enable_testing()

    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    foreach(TEST_FILE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

        add_executable(${TEST_NAME}
            ${TEST_FILE}
        )

        target_link_libraries(${TEST_NAME} PRIVATE
            library_core
        )

        target_include_directories(${TEST_NAME} PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/src"
        )

        add_test(
            NAME ${TEST_NAME}
            COMMAND ${TEST_NAME}
        )
    endforeach()
endif()
