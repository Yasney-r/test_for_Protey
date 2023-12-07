


cmake_minimum_required(VERSION 3.27)

project(MyProject)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Указываем путь к библиотеке Boost
set(BOOST_ROOT C:/boost_1_83_0)

# Находим и подключаем библиотеку Boost
find_package(Boost REQUIRED COMPONENTS)

# Создаем переменную со списком файлов исходного кода
set(SOURCES test.cpp config.h cdr.h timer.h)

# Добавляем исполняемый файл
add_executable(MyProject ${SOURCES})

# Указываем пути для поиска заголовочных файлов Boost
target_include_directories(MyProject PRIVATE ${Boost_INCLUDE_DIRS})

# Указываем пути для поиска библиотек Boost
target_link_directories(MyProject PRIVATE ${Boost_LIBRARY_DIRS})

# Линкуем наш проект с библиотеками Boost
target_link_libraries(MyProject PRIVATE ${Boost_LIBRARIES})

# Указываем путь к библиотеке spdlog
#set(SPDLOG_ROOT spdlog)

# Находим и подключаем библиотеку spdlog
#find_package(spdlog REQUIRED)

# Указываем пути для поиска заголовочных файлов spdlog
#target_include_directories(MyProject PRIVATE ${SPDLOG_INCLUDE_DIRS})

# Линкуем наш проект с библиотекой spdlog
#target_link_libraries(MyProject PRIVATE ${SPDLOG_LIBRARIES})

# Добавляем поддержку для запуска юнит-тестов
enable_testing()

# Добавляем исполняемый файл для юнит-тестов
add_executable(UnitTests ../test/test_test.cpp)

# Указываем пути для поиска заголовочных файлов Boost при компиляции тестов
target_include_directories(UnitTests PRIVATE ${Boost_INCLUDE_DIRS})

# Указываем пути для поиска заголовочных файлов spdlog при компиляции тестов
target_include_directories(UnitTests PRIVATE ${SPDLOG_INCLUDE_DIRS})

# Линкуем юнит-тесты с нашим проектом и библиотеками Boost и spdlog
target_link_libraries(UnitTests PRIVATE ${Boost_LIBRARIES})

# Добавляем тест в CTest
add_test(NAME MyProjectTests COMMAND UnitTests)