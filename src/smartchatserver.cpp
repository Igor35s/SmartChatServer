#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <sstream>
#include <boost/asio.hpp>
#include <pqxx/pqxx>
#include "Protocol.h"

using boost::asio::ip::tcp;

struct ClientInfo {
    std::shared_ptr<tcp::socket> socket;
    std::string username;
};

std::map<uint32_t, ClientInfo> online_clients;
std::mutex clients_mtx;

void handle_client(std::shared_ptr<tcp::socket> socket) {
    uint32_t current_user_id = 0;
    std::string current_username = "Guest";

    try {
        pqxx::connection db_conn("dbname=smartchat_db user=igor");

        while (true) {
            PacketHeader header;
            boost::system::error_code ec;

            boost::asio::read(*socket, boost::asio::buffer(&header, sizeof(header)), ec);
            if (ec) break;

            std::vector<char> data(header.dataSize);
            if (header.dataSize > 0) {
                boost::asio::read(*socket, boost::asio::buffer(data), ec);
                if (ec) break;
            }
            std::string payload(data.begin(), data.end());

            switch (header.type) {

                case MessageType::RegistrationRequest: {
                    try {
                        std::stringstream ss(payload);
                        std::string email, username, password;
                        std::getline(ss, email, '\n');
                        std::getline(ss, username, '\n');
                        std::getline(ss, password, '\n');

                        pqxx::work tx(db_conn);
                        pqxx::result res = tx.exec_params(
                            "INSERT INTO users (username, email, password_hash) VALUES ($1, $2, $3) RETURNING id",
                            username, email, password);
                        tx.commit();

                        // ИСПРАВЛЕНО: Доступ через [0][0]
                        uint32_t new_id = res[0][0].as<uint32_t>();

                        current_user_id = new_id;
                        current_username = username;

                        {
                            std::lock_guard<std::mutex> lock(clients_mtx);
                            online_clients[current_user_id] = {socket, current_username};
                        }

                        header.target_id = new_id;
                        header.dataSize = 0;
                        boost::asio::write(*socket, boost::asio::buffer(&header, sizeof(header)));

                        std::cout << "[REG] Юзер ID: " << new_id << " (" << username << ") в сети." << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "[SQL Error] " << e.what() << std::endl;
                    }
                    break;
                }

                case MessageType::AuthRequest: {
                    try {
                        pqxx::nontransaction nt(db_conn);
                        pqxx::result r = nt.exec_params("SELECT username FROM users WHERE id = $1", header.sender_id);
                        if (!r.empty()) {
                            current_user_id = header.sender_id;
                            // ИСПРАВЛЕНО: Доступ через [0][0]
                            current_username = r[0][0].as<std::string>();

                            std::lock_guard<std::mutex> lock(clients_mtx);
                            online_clients[current_user_id] = {socket, current_username};

                            std::cout << "[AUTH] Пользователь " << current_username << " (ID: " << current_user_id << ") вошел." << std::endl;

                            PacketHeader ok = {MessageType::RegistrationRequest, 0, current_user_id, 0};
                            boost::asio::write(*socket, boost::asio::buffer(&ok, sizeof(ok)));
                        }
                    } catch (...) {}
                    break;
                }

                case MessageType::TextMessage: {
                    if (current_user_id == 0) current_user_id = header.sender_id;
                    header.sender_id = current_user_id;

                    std::lock_guard<std::mutex> lock(clients_mtx);

                    if (header.target_id == 0) {
                        std::cout << "[ROUTE] Broadcast от " << current_user_id << " (" << current_username << ")" << std::endl;
                        for (auto const& [id, info] : online_clients) {
                            if (id != current_user_id) {
                                boost::asio::write(*info.socket, boost::asio::buffer(&header, sizeof(header)));
                                boost::asio::write(*info.socket, boost::asio::buffer(data));
                            }
                        }
                    } else {
                        if (online_clients.count(header.target_id)) {
                            std::cout << "[ROUTE] Private: " << current_user_id << " -> " << header.target_id << std::endl;
                            auto target = online_clients[header.target_id];
                            boost::asio::write(*target.socket, boost::asio::buffer(&header, sizeof(header)));
                            boost::asio::write(*target.socket, boost::asio::buffer(data));
                        }
                    }
                    break;
                }
                default: break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[NET] Ошибка: " << e.what() << std::endl;
    }

    std::lock_guard<std::mutex> lock(clients_mtx);
    if (current_user_id > 0) {
        online_clients.erase(current_user_id);
        std::cout << "[NET] Юзер " << current_user_id << " вышел." << std::endl;
    }
}

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
        acceptor.set_option(tcp::acceptor::reuse_address(true));

        std::cout << "========================================" << std::endl;
        std::cout << "   SmartChat Server v2.3 [STABLE]" << std::endl;
        std::cout << "   Listening on port: 1234" << std::endl;
        std::cout << "========================================" << std::endl;

        while (true) {
            auto socket = std::make_shared<tcp::socket>(io_context);
            acceptor.accept(*socket);
            std::thread(handle_client, socket).detach();
        }
    } catch (std::exception& e) {
        return 1;
    }
    return 0;
}
