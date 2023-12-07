#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

class CDR {
public:
    CDR() : buffer("Sample CDR data") {}

    inline void save() {
        mutex.lock();
        file.open("CDR.txt", std::fstream::out);
        if (!file.is_open()) {
            std::cout << "ERROR open output file!";
        }
        else {
            file << buffer;
            file.close();
        }
        mutex.unlock();
    }

private:
    std::mutex mutex;
    std::ofstream file;
    std::string buffer;
};

// Test case for successful file saving
TEST(CDRTest, Save_Success) {
    // Arrange
    CDR cdr;

    // Act
    cdr.save();

    // Assert
    std::ifstream savedFile("CDR.txt");
    std::stringstream buffer;
    buffer << savedFile.rdbuf();
    std::string fileContent = buffer.str();
    savedFile.close();

    EXPECT_FALSE(fileContent.empty()) << "Expected file to have non-empty content";
    EXPECT_EQ(fileContent, "Sample CDR data") << "Expected file content to match 'Sample CDR data'";
}

// Test case for file opening error
TEST(CDRTest, Save_Error_OpenFile) {
    // Arrange
    CDR cdr;
    std::ofstream testFile("CDR.txt");
    testFile.close();  // Close the file to simulate an error

    // Act
    testing::internal::CaptureStdout();
    cdr.save();
    std::string output = testing::internal::GetCapturedStdout();

    // Assert
    EXPECT_FALSE(output.empty()) << "Expected error message to be printed";
    EXPECT_THAT(output, testing::HasSubstr("ERROR open output file!"));
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
