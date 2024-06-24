# Проверка версии CMake
cmake_minimum_required(VERSION 3.28)

# Название проекта
project(mesh-topology)

# Установка переменной со списком исходников
set(SOURCE_EXE sources/main.c)

# Расположение заголовочных файлов
include_directories(sources/mesh/headers/)

# Создает исполняемый файл
add_executable(mesh-network-app src/main.c)

# Добавление подпроекта, указывается имя дирректории
add_subdirectory(sources/mesh)

# Линковка программы с библиотекой
target_link_libraries(mesh-network-app mesh)