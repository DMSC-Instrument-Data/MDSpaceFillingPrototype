function(UnitTest NAME)
  add_executable(${NAME} ${NAME}.cpp)
  add_test(${NAME} ${NAME})

  target_include_directories(
    ${NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/src
  )

  target_link_libraries(
    ${NAME}
    ${CONAN_LIBS_GTEST}
  )
endfunction(UnitTest)

function(Benchmark NAME)
  add_executable(${NAME} ${NAME}.cpp)
  add_test(${NAME} ${NAME})
  add_dependencies(${NAME} benchmark)

  target_include_directories(
    ${NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/src
  )

  target_link_libraries(
    ${NAME}
    ${GBENCH_LIBRARIES}
    ${CONAN_LIBS}
  )
endfunction(Benchmark)
