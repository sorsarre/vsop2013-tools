project(libvsop2013)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VSOP2013_SRC
        src/ascii_parser.cpp
        )

add_library(vsop2013 STATIC ${VSOP2013_SRC})
target_include_directories(vsop2013 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

install(TARGETS vsop2013
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include
        DESTINATION include)
