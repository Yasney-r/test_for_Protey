cmake_minimum_required(VERSION 3.20)
project(MyTests)

# Настройка Google Test
#set(GTEST_ROOT /путь/к/установленной/GoogleTest)  # Укажите путь к установленному Google Test
find_package(GTest REQUIRED)

# Включение юнит-тестов
enable_testing()

# Добавление исполняемого файла с тестами
add_executable(tests
   "test_test.cpp"
   "test_config.cpp"
   "test_cdr.cpp"
)

# Подключение заголовочных файлов Google Test
include_directories(${GTEST_INCLUDE_DIRS})

# Подключение библиотек Google Test
target_link_libraries(tests
   ${GTEST_BOTH_LIBRARIES}
   pthread
)

# Добавление команды для запуска тестов
add_test(NAME MyTests COMMAND tests)