cmake_minimum_required(VERSION 3.16)

project(wxHelpersProject VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# wxHelpers Interface / Header Library
add_library(wxHelpers INTERFACE)
target_include_directories(wxHelpers INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(wxHelpers INTERFACE 
    ${wxWidgets_LIBRARIES}
)

# Demo / Example Executable
set(SOURCES
    src/main.cpp
)

if(APPLE)
    add_executable(wxDemoApp MACOSX_BUNDLE ${SOURCES})
    set_target_properties(wxDemoApp PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "wxDemoApp"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.wxdemoapp"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    )
elseif(WIN32)
    add_executable(wxDemoApp WIN32 ${SOURCES})
else()
    add_executable(wxDemoApp ${SOURCES})
endif()

target_link_libraries(wxDemoApp PRIVATE wxHelpers)
