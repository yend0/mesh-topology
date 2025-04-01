# Checking the CMake version
cmake_minimum_required(VERSION 3.28)

# Project name
project(mesh-topology)

# Set the variable with the list of sources
set(server 
# sources
mesh/sources/server.c
mesh/sources/common.c
mesh/sources/graph.c
mesh/sources/packet.c
mesh/sources/logger.c
mesh/sources/user_interface.c
# headers
mesh/headers/common.h
mesh/headers/graph.h
mesh/headers/globals.h
mesh/headers/stdafx.h
mesh/headers/constants.h
mesh/headers/packet.h
mesh/headers/logger.h
mesh/headers/user_interface.h
)

set(node 
# sources
mesh/sources/node.c
mesh/sources/common.c
mesh/sources/graph.c
mesh/sources/packet.c
mesh/sources/logger.c
# headers
mesh/headers/common.h
mesh/headers/graph.h
mesh/headers/stdafx.h
mesh/headers/constants.h
mesh/headers/packet.h
mesh/headers/logger.h
)

set(test_zlib
# sources
mesh/tests/test_zlib.c
mesh/sources/packet.c
mesh/sources/common.c
# headers
mesh/headers/common.h
mesh/headers/packet.h
mesh/headers/stdafx.h
)

# Location of header files
include_directories(mesh/headers/)

# Creates an executable file for the server
add_executable(app-server ${server})

# Creates an executable file for the client
add_executable(app-node ${node})

# Creates an executable file for the compression and decompression packet
add_executable(app-test-zlib ${test_zlib})


find_package(ZLIB REQUIRED)

# Linking libraries
target_link_libraries(app-node ZLIB::ZLIB)
target_link_libraries(app-server ZLIB::ZLIB)
target_link_libraries(app-test-zlib ZLIB::ZLIB)