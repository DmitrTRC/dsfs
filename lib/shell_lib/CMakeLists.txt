# Set the project name
project(shell_lib)

message(STATUS "Lib name: ${PROJECT_NAME} start building")

# Add a library with the above sources
add_library(${PROJECT_NAME} src/Shell_Lib.cpp)

add_library(sub::shell ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
                           PUBLIC ${PROJECT_SOURCE_DIR}/include
                           )

message(STATUS "Lib name: ${PROJECT_NAME} build finished")