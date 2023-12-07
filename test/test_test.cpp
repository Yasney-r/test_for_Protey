##include <gtest / gtest.h>
#include <gmock/gmock.h>
#include <asio.hpp>
#include <asio/use_awaitable.hpp>

class MockAcceptor : public asio::ip::tcp::acceptor {
public:
    using asio::ip::tcp::acceptor::acceptor;
    MOCK_METHOD(asio::awaitable<std::error_code>, async_accept,
        (asio::ip::tcp::socket&, asio::use_awaitable_t),
        (override));
};

class MockSocket : public asio::ip::tcp::socket {
public:
    using asio::ip::tcp::socket::socket;
    MOCK_METHOD(void, close, (), (override));
};

class ServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a mock executor using asio::any_io_executor
        auto io_context = std::make_shared<asio::io_context>();
        executor = asio::any_io_executor(io_context);
    }

    asio::any_io_executor executor;
};

TEST_F(ServerTest, Listen_AcceptSuccess) {
    // Create an instance of MockAcceptor
    MockAcceptor acceptor(executor, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 80));

    // Expect the async_accept to be called once and return a successful result
    MockSocket socket(executor);
    EXPECT_CALL(acceptor, async_accept(::testing::_, asio::use_awaitable))
        .WillOnce([](asio::ip::tcp::socket& socket, asio::use_awaitable_t) {
        // Simulate a successful accept
        socket = MockSocket(asio::any_io_executor());
        co_return std::error_code();
            });

    // Create an instance of server and call listen()
    server::listen().resume(); // Call the listen coroutine

    // Assert that the acceptor and socket methods were called
    ::testing::Mock::VerifyAndClear(&acceptor);
    ::testing::Mock::VerifyAndClear(&socket);
}

TEST_F(ServerTest, Listen_AcceptError) {
    // Create an instance of MockAcceptor
    MockAcceptor acceptor(executor, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 80));

    // Expect the async_accept to be called once and return an error code
    EXPECT_CALL(acceptor, async_accept(::testing::_, asio::use_awaitable))
        .WillOnce([](asio::ip::tcp::socket& socket, asio::use_awaitable_t) {
        // Simulate an error code
        const std::error_code error_code = asio::error::connection_aborted;
        co_return error_code;
            });

    // Create an instance of server and call listen()
    server::listen().resume(); // Call the listen coroutine

    // Assert that the acceptor and socket methods were called
    ::testing::Mock::VerifyAndClear(&acceptor);
}

int main(int argc, char** argv) {
	// Инициализация Google Test
	::testing::InitGoogleTest(&argc, argv);

	// Запуск тестов
	return RUN_ALL_TESTS();
}


