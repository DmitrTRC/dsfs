### Add Tests

enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


add_executable(dsfs_test
               src/shell_test.cpp
               src/disk_test.cpp
               src/fs_test.cpp
               )

target_include_directories(dsfs_test PRIVATE
                           ${CMAKE_SOURCE_DIR}/src
                           )

target_link_libraries(dsfs_test
                      sub::shell
                      GTest::gtest_main
                      )

include(GoogleTest)

gtest_discover_tests(dsfs_test)