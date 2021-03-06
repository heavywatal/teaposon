execute_process(
  COMMAND git describe --always --dirty
  OUTPUT_VARIABLE GIT_DESCRIBE
  ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(GIT_DESCRIBE)
  set(PROJECT_VERSION ${GIT_DESCRIBE})
endif()
message(STATUS "PROJECT_VERSION: ${PROJECT_VERSION}")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp.in
  ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp @ONLY
)

# Be patient until 3.13 is popularized
add_library(objlib STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/haploid.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/population.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/program.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/transposon.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp
)
target_compile_features(objlib PUBLIC cxx_std_14)
set_target_properties(objlib PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)
target_include_directories(objlib INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
)
target_link_libraries(objlib PUBLIC
  wtl::wtl
  wtl::sfmt
  clippson::clippson
  ZLIB::ZLIB
  Threads::Threads
)
