#include <iostream>
#include <boost/asio.hpp>
#include "../Protocol.h" // Подключаем наш протокол (он уровнем выше)

using boost::asio::ip::tcp;

int main() {
    try {
        // Главный объект Boost для управления вводом-выводом
        boost::asio::io_context io_context;

        // Создаем "приемник" (acceptor), который слушает порт 1234
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));

        std::cout << "--- Сервер SmartChat запущен на порту 1234 ---" << std::endl;
        std::cout << "Ожидание подключений..." << std::endl;

        while (true) {
            // Создаем пустой сокет для нового клиента
            tcp::socket socket(io_context);

            // Ждем, пока кто-то подключится (программа замирает здесь до коннекта)
            acceptor.accept(socket);

            std::cout << "Клиент успешно подключился!" << std::endl;

            // В будущем здесь будет код чтения сообщений и фото
        }

    } catch (std::exception& e) {
        // Если что-то пойдет не так (например, порт занят), выведем ошибку
        std::cerr << "Критическая ошибка сервера: " << e.what() << std::endl;
    }

    return 0;
}
