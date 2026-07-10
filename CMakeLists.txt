cmake_minimum_required(VERSION 3.16)
project(SensorAPI)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Catch2 REQUIRED)

option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)
if(ENABLE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-O0 -g --coverage -fprofile-abs-path)
    add_link_options(--coverage)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-O0 -g -fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate -fcoverage-mapping)
  endif()
endif()

add_library(sensor_decoder
    src/SensorServer.cpp
    src/TemperatureInCelsiusDecoder.cpp
    src/TemperatureInKelvinDecoder.cpp
    src/MultiReadingTemperatureDecoder.cpp
    src/AirpressureInPascalDecoder.cpp
    src/CombinedSensorDecoder.cpp
    src/TestTemperatureDecoder.cpp
    src/CommandEncoder.cpp)

target_include_directories(sensor_decoder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_executable(Tests_e tests/SensorTests.cpp tests/TestObject.cpp)
target_link_libraries(Tests_e PRIVATE sensor_decoder Catch2::Catch2)

add_executable(CmdEncTests_e tests/SensorTestsCmdEncode.cpp)
target_link_libraries(CmdEncTests_e PRIVATE sensor_decoder Catch2::Catch2)

enable_testing()
add_test(NAME unit_tests   COMMAND $<TARGET_FILE:Tests_e>)
add_test(NAME cmdenc_tests COMMAND $<TARGET_FILE:CmdEncTests_e>)

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_custom_target(coverage_gcc
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --test-dir ${CMAKE_BINARY_DIR}
    COMMENT "Running tests to generate .gcda/.gcno for gcov_reporter"
  )
endif()

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(STATUS "Enabling Clang/LLVM coverage")

  set(COV_TEST_EXES Tests_e CmdEncTests_e)

  set(COV_DIR        "${CMAKE_BINARY_DIR}/coverage")
  set(PROFRAW_DIR    "${COV_DIR}/profraw")
  set(PROFDATA_FILE  "${COV_DIR}/default.profdata")
  set(LCOV_FILE      "${COV_DIR}/report.lcov")
  set(REPORT_TXT     "${COV_DIR}/report.txt")
  set(COBERTURA_XML  "${COV_DIR}/cobertura.xml")
  file(MAKE_DIRECTORY "${PROFRAW_DIR}")

  find_program(LLVM_COV       NAMES llvm-cov-18 llvm-cov       REQUIRED)
  find_program(LLVM_PROFDATA  NAMES llvm-profdata-18 llvm-profdata REQUIRED)
  find_program(LCOV_COBERTURA NAMES lcov_cobertura REQUIRED)

  set_tests_properties(unit_tests   PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${PROFRAW_DIR}/unit.%p.profraw")
  set_tests_properties(cmdenc_tests PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${PROFRAW_DIR}/cmdenc.%p.profraw")

  add_custom_target(coverage_run_tests
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROFRAW_DIR}
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${COV_TEST_EXES}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "[coverage] Running tests to generate .profraw"
    VERBATIM
  )

  add_custom_command(
    OUTPUT ${PROFDATA_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "[coverage] Merging raw profiles -> ${PROFDATA_FILE}"
    COMMAND bash -lc "shopt -s nullglob; files=(\"${PROFRAW_DIR}\"/*.profraw); \
                      if (( \${#files[@]} == 0 )); then \
                        echo '[coverage] ERROR: no .profraw files found' >&2; exit 1; \
                      fi; \
                      \"${LLVM_PROFDATA}\" merge -sparse \"${PROFRAW_DIR}\"/*.profraw -o \"${PROFDATA_FILE}\""
    DEPENDS coverage_run_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    VERBATIM
  )
  add_custom_target(coverage_merge DEPENDS ${PROFDATA_FILE})

  add_custom_command(
    OUTPUT ${LCOV_FILE}
    COMMAND ${LLVM_COV} export
            --format=lcov
            --instr-profile=${PROFDATA_FILE}
            $<TARGET_FILE:Tests_e>
            $<TARGET_FILE:CmdEncTests_e>
            > ${LCOV_FILE}
    DEPENDS coverage_merge ${COV_TEST_EXES}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    VERBATIM
  )
  add_custom_target(coverage_lcov DEPENDS ${LCOV_FILE})

  add_custom_command(
    OUTPUT ${REPORT_TXT}
    COMMAND ${LLVM_COV} report
            --instr-profile=${PROFDATA_FILE}
            $<TARGET_FILE:Tests_e>
            $<TARGET_FILE:CmdEncTests_e>
            > ${REPORT_TXT}
    DEPENDS coverage_merge ${COV_TEST_EXES}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    VERBATIM
  )
  add_custom_target(coverage_report DEPENDS ${REPORT_TXT})

  add_custom_command(
    OUTPUT ${COBERTURA_XML}
    COMMAND ${LCOV_COBERTURA} ${LCOV_FILE} -b ${CMAKE_SOURCE_DIR} -o ${COBERTURA_XML}
    DEPENDS coverage_lcov
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    VERBATIM
  )
  add_custom_target(coverage_cobertura DEPENDS ${COBERTURA_XML})

  add_custom_target(coverage_exports
    DEPENDS coverage_report coverage_cobertura
  )
endif()
