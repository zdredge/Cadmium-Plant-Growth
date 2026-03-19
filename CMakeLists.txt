# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

if(ESP_PLATFORM)
    message(STATUS "Building with ESP32")
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(Basil_Plant)
    
else()
    message(STATUS "Building without ESP32")
    project(Basil_Plant)
    
    # Route all compiled executables to the bin directory
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

   \# Cadmium Path
    if(DEFINED ENV{CADMIUM})
        # Check both possibilities: if CADMIUM points to the root, or directly to the include folder
        set(CADMIUM_PATHS $ENV{CADMIUM} $ENV{CADMIUM}/include)
    else()
        # Fallback if the environment variable is missing entirely
        set(CADMIUM_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../cadmium_v2/include)
    endif()

    # executable targets
    add_executable(greenhouse top_model/main.cpp)
    add_executable(test_nature test/test_nature.cpp)
    add_executable(test_soil test/test_soil.cpp)
    add_executable(test_stem test/test_stem.cpp)
    add_executable(test_basil_plant test/test_basil_plant.cpp)

    # Apply the include directories and compiler flags to all targets
    set(TARGETS greenhouse test_nature test_soil test_stem test_basil_plant)
    
    foreach(TARGET ${TARGETS})
        target_include_directories(${TARGET} PRIVATE "." "atomics" "coupled" ${CADMIUM_PATHS})
        target_compile_options(${TARGET} PUBLIC -std=gnu++17)
    endforeach()
endif()