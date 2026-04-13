#include <iostream>
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <pqxx/pqxx>    // Библиотека для PostgreSQL
#include "../Protocol.h" // Твой заголовок (8 байт)

using boost::asio::ip::tcp;

// Функция для обработки каждого клиента в отдельном потоке
void handle_client(tcp::socket socket) {
    try {
        std::cout << "[Thread " << std::this_thread::get_id() << "] [NET] Клиент подключился!" << std::endl;

        // Подключение к базе данных (ваши параметры)
        pqxx::connection db_conn("dbname=smartchat_db user=igor");
        std::cout << "[Thread " << std::this_thread::get_id() << "] [DB] Подключено к базе: " << db_conn.dbname() << std::endl;

        while (true) {
            PacketHeader header;
            boost::system::error_code ec;

            // Читаем заголовок (ровно 8 байт по Protocol.h)
            boost::asio::read(socket, boost::asio::buffer(&header, sizeof(header)), ec);

            if (ec == boost::asio::error::eof) {
                std::cout << "[Thread " << std::this_thread::get_id() << "] [NET] Клиент отключился." << std::endl;
                break; // Клиент закрыл соединение
            } else if (ec) {
                throw boost::system::system_error(ec);
            }

            std::cout << "[Thread " << std::this_thread::get_id() << "] [PROTO] Тип сообщения: " << static_cast<uint32_t>(header.type)
                      << ", Размер данных: " << header.dataSize << " байт" << std::endl;

            // Читаем само сообщение (если размер > 0)
            if (header.dataSize > 0) {
                std::vector<char> data(header.dataSize);
                boost::asio::read(socket, boost::asio::buffer(data), ec);

                if (!ec) {
                    std::string messageText(data.begin(), data.end());
                    std::cout << "[Thread " << std::this_thread::get_id() << "] [DATA] Получено: " << messageText << std::endl;

                    // Записываем в базу данных
                    pqxx::work tx(db_conn);
                    tx.exec_params("INSERT INTO chat_messages (sender, message) VALUES ($1, $2)",
                                  "Client", messageText);
                    tx.commit();
                    std::cout << "[Thread " << std::this_thread::get_id() << "] [DB] Сообщение сохранено в таблицу!" << std::endl;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[Thread " << std::this_thread::get_id() << "] Ошибка: " << e.what() << std::endl;
    }
}

int main() {
    try {
        boost::asio::io_context io_context;

        // Защита от ошибки "Address already in use" (ваш отличный код)
        tcp::acceptor acceptor(io_context);
        tcp::endpoint endpoint(tcp::v4(), 1234);

        acceptor.open(endpoint.protocol());
        acceptor.set_option(tcp::acceptor::reuse_address(true));
        acceptor.bind(endpoint);
        acceptor.listen();

        std::cout << "--- Сервер SmartChat запущен на порту 1234 ---" << std::endl;
        std::cout << "Ожидание подключений..." << std::endl;

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            // Создаем новый поток на каждое подключение
            std::thread client_thread(handle_client, std::move(socket));
            client_thread.detach(); // Поток работает независимо
        }

    } catch (std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
