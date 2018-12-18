cmake_minimum_required(VERSION 3.0)
project(my-bittorrent LANGUAGES C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pedantic -std=c99 -g -lcurl")

include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(my-bittorrent
    ${CMAKE_SOURCE_DIR}/src/main.c
    ${CMAKE_SOURCE_DIR}/src/options.c
    ${CMAKE_SOURCE_DIR}/src/contact_tracker.c
    ${CMAKE_SOURCE_DIR}/src/pretty_print.c
    ${CMAKE_SOURCE_DIR}/src/mktorrent.c
)
target_link_libraries(my-bittorrent ${CMAKE_SOURCE_DIR}/lib/libbencode.a
jansson curl)

add_custom_target(check
    COMMAND ${SHELL_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/launch.sh
    DEPENDS my-bittorrent
    COMMENT "Launching testsuite"
)
