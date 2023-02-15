# Set the project name
project(fs_lib)

message(STATUS "Lib name: ${PROJECT_NAME} start building")

# Add a library with the above sources
add_library(${PROJECT_NAME}
            src/File_System.cpp
            src/sha256.cpp
            include/sha256.hpp
            include/sha2xx_common.hpp
            #            src/FS_Extension.cpp
            )
add_library(sub::fs ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
                           PUBLIC ${PROJECT_SOURCE_DIR}/include
                           )

target_link_libraries(${PROJECT_NAME} PUBLIC sub::ds)

message(STATUS "Lib name: ${PROJECT_NAME} build finished")