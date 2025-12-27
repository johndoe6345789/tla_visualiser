cmake_minimum_required(VERSION 3.22)
project(tla_visualiser VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Qt6 setup
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Widgets)
find_package(CURL REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/main.cpp
    src/github_importer.cpp
    src/tlc_runner.cpp
    src/state_graph_model.cpp
    src/trace_viewer_model.cpp
)

set(HEADERS
    include/github_importer.h
    include/tlc_runner.h
    include/state_graph_model.h
    include/trace_viewer_model.h
)

# QML files
set(QML_FILES
    qml/main.qml
    qml/ImportView.qml
    qml/GraphView.qml
    qml/TraceView.qml
    qml/InvariantView.qml
)

# Add executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Widgets
    ${CURL_LIBRARIES}
)

# Set properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Install targets
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

# Copy QML files to build directory
foreach(QML_FILE ${QML_FILES})
    configure_file(${QML_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${QML_FILE} COPYONLY)
endforeach()

# Enable testing
enable_testing()
add_subdirectory(tests)
