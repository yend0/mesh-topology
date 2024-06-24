# Проверка версии CMake
cmake_minimum_required(VERSION 3.28)

# Название проекта
project(mesh)

# Установка переменной 'SOURCE_LIB'
set(SOURCE_LIB sources/)	

# Создание статической библиотеки
add_library(mesh STATIC ${SOURCE_LIB})