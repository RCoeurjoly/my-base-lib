# Create json database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BIN_TARGET ${PROJECT_NAME})
# This variable is for tests
SET(LIB_TARGET "${BIN_TARGET}")

message("${BIN_TARGET} is a header only library")

file(TOUCH ${CMAKE_BINARY_DIR}/dumb_${BIN_TARGET}.h )
add_library(${BIN_TARGET} STATIC "${CMAKE_BINARY_DIR}/dumb_${BIN_TARGET}.h" )
set_target_properties(${BIN_TARGET} PROPERTIES LINKER_LANGUAGE CXX)


# Dependencies
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)

# by pkgconfig
find_package(PkgConfig REQUIRED)
pkg_check_modules(SPDLOG REQUIRED IMPORTED_TARGET spdlog)

target_include_directories(${BIN_TARGET} PUBLIC ${fmt_INCLUDE_DIRS})
target_link_libraries(${BIN_TARGET}
    dl
    fmt::fmt
    nlohmann_json::nlohmann_json
    PkgConfig::SPDLOG
)

macro(create_new_test test_name test_src)
  set(test_exe_name "test_${test_name}")
  add_executable(${test_exe_name} ${test_src} ${HEADERS})
  target_link_libraries(${test_exe_name} ${LIB_TARGET})
  add_test(${test_name} ${test_exe_name} -s -d -fc)
endmacro(create_new_test)

install(TARGETS ${BIN_TARGET})

install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/" DESTINATION "include"
        FILES_MATCHING 
                PATTERN "*.h" 
                PATTERN "*.hpp")



# Support for test
if (EXISTS "${CMAKE_SOURCE_DIR}/tests")
        message("Enabling unit tests for ${PROJECT_NAME}")
        enable_testing()
        add_subdirectory("./tests")

endif (EXISTS "${CMAKE_SOURCE_DIR}/tests")

