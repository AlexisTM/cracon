cmake_minimum_required(VERSION 3.8)
project(flacon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(BUILD_EXAMPLES "Build the examples" ON)

add_library(${PROJECT_NAME} src/flacon.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

# nlohmann-json3-dev
find_package(nlohmann_json 3 QUIET)

if(NOT nlohmann_json_FOUND)
  include(ExternalProject)

  ExternalProject_Add(json-dep
    GIT_REPOSITORY "https://github.com/nlohmann/json.git"
    GIT_TAG "v3.11.3"
    GIT_SHALLOW TRUE
    SOURCE_DIR ${CMAKE_BINARY_DIR}/nlohmann-json
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON)
  include_directories(${CMAKE_BINARY_DIR}/nlohmann-json/single_include)
  install(
    DIRECTORY ${CMAKE_BINARY_DIR}/nlohmann-json/single_include/
    DESTINATION include/flacon)

  target_include_directories(${PROJECT_NAME} PUBLIC
    $<INSTALL_INTERFACE:include/flacon>)
  add_dependencies(${PROJECT_NAME} json-dep)
else()
  target_link_libraries(${PROJECT_NAME} nlohmann_json)
  add_dependencies(${PROJECT_NAME} nlohmann_json)
endif()

message("${nlohmann_json_INCLUDE_DIR}")

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
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )

  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
  enable_testing()

  include(GoogleTest)
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
