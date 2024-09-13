cmake_minimum_required(VERSION 3.28)
project(evaluation_service)

set(CMAKE_CXX_STANDARD 20)

# Set the toolchain file (this is typically done outside the CMakeLists.txt)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "~/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

include_directories(.)

# Find the CURL library
find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)

add_executable(evaluation_service eval_record.cc inference_handler.cc generic_llm_client.cc
        main.cc)


# Explicitly link the CURL library

# Link the library to your executable
target_link_libraries(evaluation_service PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(evaluation_service PRIVATE ${CURL_LIBRARIES})
target_link_libraries(evaluation_service PRIVATE CURL::libcurl JsonCpp::JsonCpp)
