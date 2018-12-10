cmake_minimum_required(VERSION 3.0)
project(my-bittorrent LANGUAGES C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pedantic -std=c99 -g")

include_directories(${CMAKE_SOURCE_DIR}/src/include)
add_executable(my-bittorrent
    ${CMAKE_SOURCE_DIR}/src/main.c
)
target_link_libraries(my-bittorrent)

add_custom_target(check
    COMMAND ${SHELL_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/launch.sh
    DEPENDS my-bittorrent
    COMMENT "Launching testsuite"
)
