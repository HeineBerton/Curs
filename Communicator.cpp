#include "Auth.h"
#include "ClientDataBase.h"
#include "Communicator.h"
#include "Calculator.h"
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>
#include <system_error> 
#include <vector> 
Communicator::Communicator(int socket, const std::string& dataFile, Logger& logger)
    : dataFile(dataFile)
    , logger(logger)
    , client_socket(socket)
{
}
void Communicator::handleClient()
{
    char str[1024] = { 0 };
    ssize_t recvstr = recv(client_socket, str, sizeof(str), 0);
    if(recvstr == 0) {
        std::string error_message = strerror(errno);
        std::string exception_message = "Клиент закрыл соединение: " + error_message;
        logger.log(ERROR, exception_message);
    } else if(recvstr < 0) {
        std::string error_message = strerror(errno);
        std::string exception_message = "Ошибка получения строки: " + error_message;
        throw std::system_error(errno, std::generic_category(), exception_message);
    }
    for(ssize_t i = 0; i < recvstr; ++i) {
        if(str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
    logger.log(INFO, "Клиент прислал строку: " + std::string(str)); 
    Authenticator authenticator(dataFile, logger);
    if(authenticator.validateUserFromString(str)==false){
        logger.log(ERROR, "Клиент не прошел аутентификацию");
        const std::string errMsg = "ERR"; 
        send(client_socket, errMsg.c_str(), errMsg.size(), 0);
        close(client_socket);
    }
    else{
        logger.log(INFO, "Клиент прошел аутентификацию");
        const std::string successMsg = "OK\n"; 
        send(client_socket, successMsg.c_str(), successMsg.size(), 0);
        processVectors();
    }
}
void Communicator::processVectors()
{
    logger.log(INFO, "Начинаю обработку векторов");
    uint32_t numVectors = 0;
    ssize_t recvBytes = recv(client_socket, &numVectors, sizeof(numVectors), 0);
    if(recvBytes != sizeof(numVectors)) {
        std::string error_message = strerror(errno);
        std::string exception_message = "Ошибка получения количества векторов : " + error_message;
        logger.log(ERROR, exception_message);
        close(client_socket);
    } else {
        logger.log(INFO, "От клиента пришло " + std::to_string(numVectors) + " векторов");
        for(uint32_t i = 0; i < numVectors; ++i) {
            std::vector<double> vectorValues;
            readVector(vectorValues);
            double resultValue = Calculator::processVector(vectorValues);
            logger.log(INFO, "Результат вычисления по " + std::to_string(i + 1) + " вектору: " + std::to_string(resultValue)); // Добавлено std::
            sendResultToClient(resultValue);
        }
    }
}
void Communicator::readVector(std::vector<double>& vectorValues)
{
    uint32_t vectorSize = 0;
    ssize_t recvBytes = recv(client_socket, &vectorSize, sizeof(vectorSize), 0);
    if(recvBytes != sizeof(vectorSize)) {
        std::string error_message = strerror(errno);
        std::string exception_message = "Ошибка получения размера вектора: " + error_message;
        logger.log(ERROR, exception_message);
    } else {
        vectorValues.resize(vectorSize);
        size_t bytesRead = 0;
        while(bytesRead < vectorSize * sizeof(double)) {
            recvBytes = recv(client_socket, reinterpret_cast<char*>(vectorValues.data()) + bytesRead,
                             vectorSize * sizeof(double) - bytesRead, 0);
            if(recvBytes <= 0) {
                std::string error_message = strerror(errno);
                std::string exception_message = "Ошибка чтения из сокета: " + error_message;
                logger.log(CRITICAL, exception_message);
            }
            bytesRead += recvBytes;
        }
    }
}
void Communicator::sendResultToClient(double resultValue)
{
    ssize_t sentBytes = send(client_socket, &resultValue, sizeof(resultValue), 0);
    logger.log(INFO, "Отправил клиенту результаты вычислений");
    if(sentBytes != sizeof(resultValue)) {
        std::string error_message = strerror(errno);
        std::string exception_message = "Ошибка отправки результата: " + error_message;
        logger.log(ERROR, exception_message);
    }
}
Communicator::~Communicator() { close(client_socket); }