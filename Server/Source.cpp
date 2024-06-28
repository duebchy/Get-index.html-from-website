#include <iostream>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

std::vector<char> vBuffer(20 * 1024);

void GrabSomeData(asio::ip::tcp::socket& socket) {
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
		[&](std::error_code ec, std::size_t length) {
			if (!ec) {
				std::cout << "\n\nRead " << length << " bytes\n\n";

				for (int i = 0; i < length; i++) {
					std::cout << vBuffer[i];
				}
				
			}
		}
	);
	
}


std::string ip = "0.000.000.0";
int main() {
	setlocale(LC_ALL, "ru");
	asio::error_code ec;

	asio::io_context context;

	asio::io_context::work idleWork(context);

	std::thread thrContext = std::thread([&]() {context.run();  });
	//Getting addres of somewhere we want to connect
	std::cout << "enter help to get ip of website" << std::endl;
	
	
	while (true) {
		std::cout << "enter ip of website or quit" << std::endl;
	
		std::cin >> ip;
		if (ip == "help") {
			std::cout << "open cmd and write command: ping example.com";
		}
		if (ip == "quit") {
			break;
		}
		asio::ip::tcp::endpoint endpoint(asio::ip::make_address(ip, ec), 80);
		//create socket
		asio::ip::tcp::socket socket(context);
		//Try to connect
		socket.connect(endpoint, ec);
		if (!ec) {
			std::cout << "connected" << std::endl;
		}
		else {
			std::cout << "Fail: " << ec.message() << std::endl;
		}

		if (socket.is_open()) {
			GrabSomeData(socket);
			
			std::string sRequest =
				"GET /index.html HTTP/1.1\r\n"
				"Host: example.com\r\n"
				"Connection: close\r\n\r\n";

			socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

			using namespace std::chrono_literals;
			std::this_thread::sleep_for(2000ms);
		}
		socket.close();
		ip.clear();
	}
	return 0;
}