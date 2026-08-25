cmake_minimum_required(VERSION 3.8)

# Visual Studio uses a different CRT for Debug and Release builds
# OpenSim's DLL are built for release, therefore the simulation MUST also be built for release
set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)
project(simulation)

# Put the predefined targets in a separate folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakePredefinedTargets")

# Output the executable in ./opensim
set(RUNTIME_DIR "${CMAKE_SOURCE_DIR}/opensim")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RUNTIME_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${RUNTIME_DIR}")

# Add all the source and header files to the project
file(GLOB SOURCE_FILES "src/*.cpp")
file(GLOB HEADER_FILES "include/*.hpp")

# Find the OpenSim install directory
find_package(OpenSim REQUIRED)

# Set the executable's name
add_executable(simulation ${SOURCE_FILES} ${HEADER_FILES})

# OpenSim uses C++20 language features.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Link the OpenSim library to the project
target_link_libraries(simulation ${OpenSim_LIBRARIES})

# Add the header files to the included directories
target_include_directories(simulation PRIVATE "${CMAKE_SOURCE_DIR}/include")

# Tell Visual Studio to make our project the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT simulation)

# Set Visual Studio's working directory as ./opensim
set_target_properties(simulation PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${RUNTIME_DIR}")

# Pass the installed OpenSim root directory as a macro
target_compile_definitions(simulation PRIVATE OPENSIM_ROOT_DIR="${OpenSim_ROOT_DIR}")
