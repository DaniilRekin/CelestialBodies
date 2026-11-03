cmake_minimum_required(VERSION 3.20)

# Название проекта
project(MyProject
    VERSION 1.0
    DESCRIPTION "My C++ project"
    LANGUAGES CXX
)

# Стандарт C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Папка с заголовочными файлами
include_directories(${PROJECT_SOURCE_DIR}/include)

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/Engine.cpp
)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES})

# Подключение заголовочных файлов
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Предупреждения компилятора
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Установка директории для бинарных файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)