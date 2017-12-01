cmake_minimum_required(VERSION 3.1.0)

PROJECT(proxy_server)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})


add_executable(proxy_server
    src/main.c
    src/common.c
    src/server.c
    src/blacklist.c
    src/caching.c
    src/inspecao.c
)
set_property(TARGET proxy_server PROPERTY C_STANDARD 11)
