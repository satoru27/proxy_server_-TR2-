cmake_minimum_required(VERSION 3.1.0)

PROJECT(proxy_server)

add_executable(proxy_server
    main.c
    common.c
    server.c
    blacklist.c
)
set_property(TARGET proxy_server PROPERTY C_STANDARD 11)
