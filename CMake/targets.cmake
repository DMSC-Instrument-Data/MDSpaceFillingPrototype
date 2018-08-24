macro(parse_arguments)
  set(options)
  set(oneValueArgs NAME)
  set(multiValueArgs SOURCES HEADERS LIBRARIES)
  cmake_parse_arguments(
    TARGET
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
  )
endmacro()

function(UnitTest)
  parse_arguments(${ARGV})

  add_executable(
    ${TARGET_NAME}
    ${TARGET_SOURCES}
  )
  add_test(${TARGET_NAME} ${TARGET_NAME})

  target_include_directories(
    ${TARGET_NAME}
    PRIVATE
    ${TARGET_HEADERS}
  )

  target_link_libraries(
    ${TARGET_NAME}
    ${CONAN_LIBS_GTEST}
    ${TARGET_LIBRARIES}
  )
endfunction()

function(Benchmark)
  parse_arguments(${ARGV})

  add_executable(
    ${TARGET_NAME}
    ${TARGET_SOURCES}
  )

  target_include_directories(
    ${TARGET_NAME}
    PRIVATE
    ${TARGET_HEADERS}
  )

  target_link_libraries(
    ${TARGET_NAME}
    ${GBENCH_LIBRARIES}
    ${CONAN_LIBS}
    ${TARGET_LIBRARIES}
  )
endfunction()

function(Executable)
  parse_arguments(${ARGV})

  add_executable(
    ${TARGET_NAME}
    ${TARGET_SOURCES}
  )

  target_include_directories(
    ${TARGET_NAME}
    PRIVATE
    ${TARGET_HEADERS}
  )

  target_link_libraries(
    ${TARGET_NAME}
    ${TARGET_LIBRARIES}
  )
endfunction()

function(Library)
  parse_arguments(${ARGV})

  add_library(
    ${TARGET_NAME}
    SHARED
    ${TARGET_SOURCES}
  )

  target_link_libraries(
    ${TARGET_NAME}
    ${TARGET_LIBRARIES}
  )
endfunction()
