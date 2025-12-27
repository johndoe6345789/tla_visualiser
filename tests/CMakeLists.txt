cmake_minimum_required(VERSION 3.22)

# Find Qt Test
find_package(Qt6 REQUIRED COMPONENTS Test)

# Test for GitHubImporter
add_executable(test_github_importer
    test_github_importer.cpp
    ../src/github_importer.cpp
)

target_include_directories(test_github_importer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
)

target_link_libraries(test_github_importer
    Qt6::Test
    Qt6::Core
    ${CURL_LIBRARIES}
)

add_test(NAME test_github_importer COMMAND test_github_importer)

# Test for TLCRunner
add_executable(test_tlc_runner
    test_tlc_runner.cpp
    ../src/tlc_runner.cpp
)

target_include_directories(test_tlc_runner PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(test_tlc_runner
    Qt6::Test
    Qt6::Core
)

add_test(NAME test_tlc_runner COMMAND test_tlc_runner)
