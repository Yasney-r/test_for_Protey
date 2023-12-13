
#include <iostream>
#include <string>
#include <thread>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio.hpp>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <set>
#include "spdlog/spdlog.h"

 
#include "config.h"
#include "timer.h"
#include "cdr.h"
#include "log.h"




namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace http = boost::beast::http;





class server {

public:
	server();
	~server() {}

	void startServer();
private:
	asio::awaitable<void> workForPool(tcp::socket socket);

	asio::awaitable<void> listen();
	int randCallTime(int from, int to);
	
	void do_close(tcp::socket socket);
	void do_close_timeout(tcp::socket socket);

	int id;
	int pooler;
	std::deque<int> idPool;															//дек для очереди из id
	std::set<int> numberMap;													    //проверки повторного соединения
	int lengthPool;
	int timeout;
	int nOperator;
	int freeOperator;
	int from, to;
};






asio::awaitable<void> server::workForPool(tcp::socket socket)
{
	cdr cdr;
	cdr.setConnectTime();															//сохраняем в CDR время подключения
    Timer timer;
	id++;
	int callId = id;	
	cdr.setCallId(std::to_string(callId));											//сохраняем в CDR CallId
	idPool.push_back(callId);														//заполняем очередь 
	std::string response;
	asio::streambuf buffer;

	http::response<http::dynamic_body> res;											//создаем объект для входящего запроса
	co_await http::async_read(socket, buffer, res, asio::use_awaitable);			//читаем из сккета запрос
    log_debug("Request number {}");

	
	int number = stoi(boost::beast::buffers_to_string(res.body().data()));			//заносим в number номер из тела запроса
	buffer.consume(buffer.size());													//чистим буфер
	cdr.setNumber(std::to_string(number));											//сохраняем в CDR номер абонент
    log_info("subscriber number: {}", number);

	if (numberMap.find(number) == numberMap.end()) {								//если нету нашгего номера, то добавляем
		numberMap.insert(number);													//добавляем
	}
	else {																			//если есть наш номер, то соккет закрывается и корутина тоже
		log_warn("Call duplication {}", number);
		http::response<http::string_body> deplicateResponse;
		{
			http::status::bad_request;
			deplicateResponse.set(http::field::server, BOOST_BEAST_VERSION_STRING);  //создаем объект ответа с call duplication
			deplicateResponse.set(http::field::content_type, "text/html");
			deplicateResponse.keep_alive(res.keep_alive());
			deplicateResponse.body() = "call duplication";
		}
        log_debug("Create dulication responce. Number: {}", number);
		co_await http::async_write(socket, deplicateResponse, asio::use_awaitable);
        log_error("Error disconnect: Call duplication: {}", number);
		do_close(std::move(socket));

		cdr.setStatus("call duplication");
		std::thread trd_forDisconnect([&]() {
			cdr.setDisconnctTime(false);
			});
        log_debug("Create CDR number {}", number);
		co_return;
	}
	
	
	if (pooler>=lengthPool) {														//если сюжет overload
		cdr.setStatus("overload");
        log_warn("Overload!!! Number: {}", number);
		http::response<http::string_body> overloadResponse;
		{
			http::status::bad_request;
			overloadResponse.set(http::field::server, BOOST_BEAST_VERSION_STRING);  //создаем объект ответа с overload
			overloadResponse.set(http::field::content_type, "text/html");
			overloadResponse.keep_alive(res.keep_alive());
			overloadResponse.body() = "overload";
		}
        log_debug("Create overload response, Number: {}", number);
		co_await http::async_write(socket, overloadResponse, asio::use_awaitable);

		do_close(std::move(socket));
		std::thread trd_forDisconnect([&]() { 
			cdr.setDisconnctTime(false);
			});
        log_debug("Create CDR number {}", number);
		co_return;
	}																				

	http::response<http::string_body> idRes;
	{
		http::status::ok;
		idRes.set(http::field::server, BOOST_BEAST_VERSION_STRING);					//создаем объект ответа с Callid
		idRes.set(http::field::content_type, "text/html");
		idRes.keep_alive(res.keep_alive());
		idRes.body() = std::to_string(callId);
	}
    log_debug("Create Call ID responce, Number: {}", number);
	co_await http::async_write(socket, res, asio::use_awaitable);					//отправляем ответ с Callid
	log_info("Send Call ID responce. Number: {1:d}, Call ID: {0:d}", callId, number);
	//далее мы проверяем услове для очереди

	for (;;) {
		if (callId == idPool[0] && freeOperator < nOperator) {
			freeOperator++;

			cdr.setOperatorTime(true);												//сохраняем в CDR время ответа оператора
			//co_await asio::steady_timer(socket.get_executor(), std::chrono::seconds(randCallTime(from, to)), asio::use_awaitable);
            log_debug("Begin call number: {}", number);
            std::thread trd([&]() {
				timer.sozvon(randCallTime(from, to)*1000);
				});
			trd.join();
            log_debug("End call number: {}", number);
			freeOperator--;

			do_close(std::move(socket));
			std::thread trd_forDisconnect([&]() {
				cdr.setDisconnctTime(true);
				});
            log_debug("Create CDR number {}", number);
            log_info("Disconnect number: {}", number);
			co_return;
		}
	}


}


asio::awaitable<void> server::listen() 
{
	const auto executor = co_await asio::this_coro::executor;

	tcp::acceptor acceptor(executor, tcp::endpoint(tcp::v4(), 80));
	for (;;) {
		tcp::socket socket = co_await acceptor.async_accept(asio::use_awaitable);
        log_info("New Connect!");
		asio::co_spawn(executor, workForPool(std::move(socket)), asio::detached);
	}
}

int server::randCallTime(int from, int to)
{
	srand(unsigned(time(0)));
	if (to == from)
		return to;
	if (to < from)
		return to + rand() % (from - to + 1);

	return from + rand() % (to - from + 1);
}

void server::do_close(tcp::socket socket)
{
	socket.shutdown(tcp::socket::shutdown_send);
}


void server::do_close_timeout(tcp::socket socket) 
{
		socket.shutdown(tcp::socket::shutdown_send);
}

server::server()
{
	config config;
	id = 0;
	pooler = 0;
	freeOperator = 0;
	lengthPool = config.getLengthPool();
	timeout = config.getTimeout();
	nOperator = config.getNoperator();
	from = config.getFrom();
	to = config.getTo();
    log_debug("Max length Pool: {4:d}, Timeout: {3:d}, Numbers operator: {2:d}, Random call time from {1:d} to {0:d}.", to, from, nOperator, timeout, lengthPool);
}

void server::startServer() {
	asio::io_context ioContext;
	asio::co_spawn(ioContext, listen(), asio::detached);
	ioContext.run();
}

int main() {
    spdlog::set_level(spdlog::level::debug);
	server Server;
	Server.startServer();
}

