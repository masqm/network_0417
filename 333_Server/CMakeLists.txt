cmake_minimum_required(VERSION 3.13)
project(333_Server C)

set(CMAKE_C_STANDARD 99)

set(INC_DIR /usr/local/opt/openssl/include)
set(LINK_DIR /usr/local/opt/openssl/lib)


include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(ssl)
link_libraries(crypto)


add_executable(333_Server main.c zmy.c zmy.h head.h)