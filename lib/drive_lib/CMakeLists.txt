# Set the project name
project(drive_lib)

message(STATUS "Lib name: ${PROJECT_NAME} start building")

# Add a library with the above sources
add_library(${PROJECT_NAME}
            src/Disk.cpp
            )

add_library(sub::ds ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
                            PUBLIC ${PROJECT_SOURCE_DIR}/include
                            )

message(STATUS "Lib name: ${PROJECT_NAME} build finished")