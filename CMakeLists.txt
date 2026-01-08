cmake_minimum_required(VERSION 3.31)
project(marlon LANGUAGES C CXX)

add_subdirectory(dependencies/Catch2-3.11.0)
add_subdirectory(dependencies/Vulkan-Headers)

add_library(
  rhi
  src/rhi/object.cpp
  src/rhi/interface.cpp
  src/rhi/descriptor_set_layout.cpp
  src/rhi/descriptor_set.cpp
  src/rhi/command_buffer.cpp
  src/rhi/ptr.cpp
)

add_library(
  rhi-vulkan
  dependencies/volk-1.4.304/volk.c
  src/rhi/vulkan.cpp
)
target_include_directories(
  rhi-vulkan
  SYSTEM
  PRIVATE
  ${CMAKE_SOURCE_DIR}/dependencies/volk-1.4.304
)
target_link_libraries(
  rhi-vulkan
  PRIVATE
  Vulkan::Headers
)

add_executable(
  test-enum_bitset
  tests/enum_bitset.cpp
)
target_link_libraries(
  test-enum_bitset
  PRIVATE
  Catch2::Catch2WithMain
)

set(
  target_list
  "rhi"
  "rhi-vulkan"
  "test-enum_bitset"
)

foreach(target ${target_list})
  set_property(TARGET ${target} PROPERTY CXX_STANDARD 23)
  set_property(TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET ${target} PROPERTY CXX_EXTENSIONS OFF)
  target_include_directories(${target} SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/include)
  target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror -O3)
endforeach()
