


cmake_minimum_required(VERSION 3.27)

project(testForProtey)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Указываем путь к библиотеке Boost
set(BOOST_ROOT C:/boost_1_83_0)

# Находим и подключаем библиотеку Boost
find_package(Boost REQUIRED COMPONENTS)

#find_package(spdlog::spdlog REQUIRED)

# Создаем переменную со списком файлов исходного кода
set(SOURCES test.cpp config.h cdr.h timer.h
        log.h)

# Добавляем исполняемый файл
add_executable(testForProtey ${SOURCES})

# Указываем пути для поиска заголовочных файлов Boost
target_include_directories(testForProtey PRIVATE ${Boost_INCLUDE_DIRS})

# Указываем пути для поиска библиотек Boost
target_link_directories(testForProtey PRIVATE ${Boost_LIBRARY_DIRS})

#target_link_libraries(example PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
# Линкуем наш проект с библиотеками Boost
target_link_libraries(testForProtey PRIVATE ${Boost_LIBRARIES})
target_link_libraries(testForProtey PUBLIC spdlog::spdlog)

# Добавляем поддержку для запуска юнит-тестов
#enable_testing()

# Добавляем исполняемый файл для юнит-тестов
#add_executable(UnitTests ../test/test_test.cpp)

# Указываем пути для поиска заголовочных файлов Boost при компиляции тестов
#target_include_directories(UnitTests PRIVATE ${Boost_INCLUDE_DIRS})

# Указываем пути для поиска заголовочных файлов spdlog при компиляции тестов
#target_include_directories(UnitTests PRIVATE ${SPDLOG_INCLUDE_DIRS})

# Линкуем юнит-тесты с нашим проектом и библиотеками Boost и spdlog
#target_link_libraries(UnitTests PRIVATE ${Boost_LIBRARIES})

# Добавляем тест в CTest
#add_test(NAME MyProjectTests COMMAND UnitTests)