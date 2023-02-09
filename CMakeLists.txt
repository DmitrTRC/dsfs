cmake_minimum_required(VERSION 3.20)

project(DSFS VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

#set(Boost_USE_MULTITHREADED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
message(STATUS "DSFS Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")


### 3-d Party Libraries
include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

############################################################
# Create a library
############################################################

#Generate the static library from the library sources

add_subdirectory(lib)


############################################################
# Create an executable
############################################################

add_executable(
        ${PROJECT_NAME}
        App/main.cpp
)


target_link_libraries(${PROJECT_NAME}
                      sub::fs
                      sub::ds
                      #                      sub::shell
                      )

######### TEST SECTION ###########


add_subdirectory(tests)