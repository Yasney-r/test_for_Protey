#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <iostream>
#include <boost/property_tree/json_parser.hpp>

// Декларация функции для тестирования
inline void openFile();

// Тест на успешное открытие файла
TEST(configTest, OpenFile_Success) {
	// Arrange
	std::ofstream configFile("D:\\config.json");
	configFile.close();

	// Act
	openFile();

	// Assert
	// Проверяем, что файл успешно открыт и нет вывода об ошибке на консоль
	testing::internal::CaptureStdout();
	openFile();
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.empty()) << "Expected no error message";
}

// Тест на ошибку при открытии несуществующего файла
TEST(configTest, OpenFile_Error) {
	// Arrange
	// Удаляем файл, чтобы проверить, что происходит ошибка
	std::remove("D:\\config.json");

	// Act
	testing::internal::CaptureStdout();
	openFile();
	std::string output = testing::internal::GetCapturedStdout();

	// Assert
	EXPECT_FALSE(output.empty()) << "Expected error message";
	EXPECT_THAT(output, testing::HasSubstr("ERROR open config file!"));
}

inline int parseJson(boost::property_tree::ptree root, std::string field);

// Тест на успешный парсинг целочисленного значения из JSON
TEST(configTest, ParseJson_Success) {
	// Arrange
	boost::property_tree::ptree root;
	root.put("field1", 123);

	// Act
	int value = parseJson(root, "field1");

	// Assert
	EXPECT_EQ(value, 123) << "Expected value to be 123";
	testing::internal::CaptureStdout();
	parseJson(root, "field1");
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.empty()) << "Expected no error message";
}

// Тест на ошибку при пустом JSON
TEST(configTest, ParseJson_Error_Empty) {
	// Arrange
	boost::property_tree::ptree root;

	// Act
	testing::internal::CaptureStdout();
	int value = parseJson(root, "field1");
	std::string output = testing::internal::GetCapturedStdout();

	// Assert
	EXPECT_EQ(value, 0) << "Expected value to be 0";
	EXPECT_FALSE(output.empty()) << "Expected error message";
	EXPECT_THAT(output, testing::HasSubstr("error!"));
}

// Тест на ошибку при отсутствии поля в JSON
TEST(configTest, ParseJson_Error_MissingField) {
	// Arrange
	boost::property_tree::ptree root;
	root.put("field1", 123);

	// Act
	testing::internal::CaptureStdout();
	int value = parseJson(root, "field2");
	std::string output = testing::internal::GetCapturedStdout();

	// Assert
	EXPECT_EQ(value, 0) << "Expected value to be 0";
	EXPECT_FALSE(output.empty()) << "Expected error message";
	EXPECT_THAT(output, testing::HasSubstr("error!"));
}


int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}