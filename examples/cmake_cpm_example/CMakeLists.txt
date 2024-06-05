cmake_minimum_required(VERSION 3.8)
project(example LANGUAGES CXX)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if (NOT DEFINED CMAKE_CXX_FLAGS_RELEASE)
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

if (NOT DEFINED CMAKE_CXX_FLAGS_DEBUG)
  set(CMAKE_CXX_FLAGS_DEBUG "-g2 -ggdb")
endif()

include(cmake/CPM.cmake)
CPMAddPackage(
  NAME flacon
  GITHUB_REPOSITORY alexistm/flacon
  GIT_TAG main
  VERSION 0.0.1
  OPTIONS "BUILD_EXAMPLES OFF"
)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(${PROJECT_NAME} flacon)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
