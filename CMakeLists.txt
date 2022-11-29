cmake_minimum_required(VERSION 3.23)

project(dsfs VERSION 0.0.1)

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
#add_library(dsfs_lib STATIC
#
#            )

#target_include_directories(dsfs_lib
#                           PUBLIC
#                           ${PROJECT_SOURCE_DIR}/include
#                           )

#target_link_libraries(dsfs_lib PUBLIC)

############################################################
# Create an executable
############################################################

add_executable(
        DSFS
        App/main.cpp
)


#target_link_libraries(bot_binary PUBLIC
#                      dsfs_lib)

######### TEST SECTION ###########


add_subdirectory(tests)