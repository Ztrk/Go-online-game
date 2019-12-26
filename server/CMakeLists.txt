cmake_minimum_required(VERSION 3.0)

project(go-server)

add_executable(server server.c go.c)

target_link_libraries(server PRIVATE pthread)
target_compile_options(server PRIVATE -Wall -Wextra)
