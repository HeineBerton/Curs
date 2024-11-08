#include "server.h"

#include "Communicator.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <string>


void Server::listen_socket()
{
    if(listen(server_socket, 5) == -1) {
        throw std::system_error(errno, std::generic_category(), "Ошибка прослушивания сокета");
    }
}

Server::Server(int port, const std::string& data, Logger& logger)
    : port(port)
    , data(data)
    , logger(logger) 
{
    logger.log(INFO, "Создание сервера на порту " + std::to_string(port));

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1) {
        std::string error_message = strerror(errno);
        std::string exception_message = "Ошибка создания сокета: " + error_message;
        logger.log(CRITICAL, exception_message);
        throw std::system_error(errno, std::generic_category(), exception_message);
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::string error_message = strerror(errno);
        std::string exception_message = "Ошибка привязки сокета: " + error_message;
        logger.log(CRITICAL, exception_message);
        throw std::system_error(errno, std::generic_category(), exception_message);
    }

    listen_socket();
}

void Server::start()
{
    while(true) {
        sockaddr_in client_addr{};
        socklen_t client_addr_size = sizeof(client_addr);

        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
        if(client_socket == -1) {
            std::string error_message = strerror(errno);
            std::string exception_message = "Ошибка при приёме соединения: " + error_message;
            logger.log(CRITICAL, exception_message);
            throw std::system_error(errno, std::generic_category(), exception_message);
        }
        std::string client_ip = inet_ntoa(client_addr.sin_addr);
        logger.log(INFO, "Новое подключение от " + client_ip);
        Communicator communicator(client_socket, data, logger);
        communicator.handleClient();
        close(client_socket);
    }
}