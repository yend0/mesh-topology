# Проверка версии CMake
cmake_minimum_required(VERSION 3.28)

# Название проекта
project(mesh-topology)

# Установка переменной со списком исходников
set(server 
# sources
mesh/sources/server.c
mesh/sources/common.c
mesh/sources/graph.c
mesh/sources/logger.c
# headers
mesh/headers/common.h
mesh/headers/graph.h
mesh/headers/stdafx.h
mesh/headers/constants.h
mesh/headers/packet.h
mesh/headers/logger.h
)

set(node 
# sources
mesh/sources/node.c
mesh/sources/common.c
mesh/sources/graph.c
mesh/sources/logger.c
# headers
mesh/headers/common.h
mesh/headers/graph.h
mesh/headers/stdafx.h
mesh/headers/constants.h
mesh/headers/packet.h
mesh/headers/logger.h

)

# Расположение заголовочных файлов
include_directories(mesh/headers/)

# Создает исполняемый файл
add_executable(app-server ${server})
add_executable(app-node ${node})

find_package(ZLIB REQUIRED)
target_link_libraries(app-node ZLIB::ZLIB)
target_link_libraries(app-server ZLIB::ZLIB)