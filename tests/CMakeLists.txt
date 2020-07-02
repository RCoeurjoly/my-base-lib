cmake_minimum_required(VERSION 3.0)
project(VTFeedHandlerMem_tests)
enable_testing()
include_directories(
    "../include")
file(GLOB_RECURSE HEADERS "../include/*.h")

create_new_test(timestamp Timestamp.cpp)
create_new_test(bin_strings BinStrings.cpp)
create_new_test(fmt_lib FmtLib.cpp)
create_new_test(spdlog Spdlog.cpp)
create_new_test(async_file_writer AsyncFileWriter.cpp)
create_new_test(ilogger ILog.cpp)
create_new_test(defer Defer.cpp)
