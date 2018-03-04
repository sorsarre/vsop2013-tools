cmake_minimum_required(VERSION 3.7)

set(VSOP_CONVERT_SRC
        src/vsop-convert.cpp
        src/sqlite_printer.cpp
        src/text_printer.cpp)

set(VSOP_CONVERT_HDR
        include/sqlite_printer.hpp
        include/text_printer.hpp)

add_executable(vsop-convert ${VSOP_CONVERT_SRC} ${VSOP_CONVERT_HDR})
target_include_directories(vsop-convert PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(vsop-convert vsop2013)

install(TARGETS vsop-convert
        RUNTIME DESTINATION bin)
