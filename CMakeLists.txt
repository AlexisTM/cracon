cmake_minimum_required(VERSION 3.14)
project(flacon LANGUAGES CXX)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if (NOT DEFINED CMAKE_CXX_FLAGS_RELEASE)
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

if (NOT DEFINED CMAKE_CXX_FLAGS_DEBUG)
  set(CMAKE_CXX_FLAGS_DEBUG "-g2 -ggdb")
endif()

option(BUILD_EXAMPLES "Build the examples" ON)

include(cmake/CPM.cmake)

add_library(${PROJECT_NAME} src/flacon.cpp)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

CPMAddPackage("gh:nlohmann/json@3.11.3")
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
add_dependencies(${PROJECT_NAME} nlohmann_json)

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

if(BUILD_EXAMPLES)
  add_executable(${PROJECT_NAME}_basic_usage examples/basic_usage.cpp)
  target_link_libraries(${PROJECT_NAME}_basic_usage ${PROJECT_NAME})

  add_executable(${PROJECT_NAME}_group_param_usage examples/group_param_usage.cpp)
  target_link_libraries(${PROJECT_NAME}_group_param_usage ${PROJECT_NAME})
endif()

if(BUILD_TESTING)
  enable_testing()
  CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG v1.14.0
    VERSION 1.14.0
    OPTIONS "INSTALL_GTEST OFF" "BUILD_GMOCK OFF" "BUILD_GMOCK OFF" "gtest_force_shared_crt"
  )

  add_executable(${PROJECT_NAME}_is_similar_test test/is_similar_test.cpp)
  target_link_libraries(${PROJECT_NAME}_is_similar_test ${PROJECT_NAME} GTest::gtest_main)

  add_executable(${PROJECT_NAME}_file_test test/file_test.cpp)
  target_link_libraries(${PROJECT_NAME}_file_test ${PROJECT_NAME} GTest::gtest_main)

  add_executable(${PROJECT_NAME}_group_test test/group_test.cpp)
  target_link_libraries(${PROJECT_NAME}_group_test ${PROJECT_NAME} GTest::gtest_main)

  # Under Windows, the runtime location depends on the target. This is the safest bet to keep compatiblity across OSes
  add_custom_command(TARGET ${PROJECT_NAME}_is_similar_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/test/static_test_data.json
    $<TARGET_FILE_DIR:${PROJECT_NAME}_is_similar_test>/static_test_data.json)

  include(GoogleTest)
  gtest_discover_tests(${PROJECT_NAME}_is_similar_test)
  gtest_discover_tests(${PROJECT_NAME}_file_test)
  gtest_discover_tests(${PROJECT_NAME}_group_test)
endif()
