cmake_minimum_required(VERSION 3.20)
project(MyTests)

# ��������� Google Test
#set(GTEST_ROOT /����/�/�������������/GoogleTest)  # ������� ���� � �������������� Google Test
find_package(GTest REQUIRED)

# ��������� ����-������
enable_testing()

# ���������� ������������ ����� � �������
add_executable(tests
   "test_test.cpp"
   "test_config.cpp"
   "test_cdr.cpp"
)

# ����������� ������������ ������ Google Test
include_directories(${GTEST_INCLUDE_DIRS})

# ����������� ��������� Google Test
target_link_libraries(tests
   ${GTEST_BOTH_LIBRARIES}
   pthread
)

# ���������� ������� ��� ������� ������
add_test(NAME MyTests COMMAND tests)