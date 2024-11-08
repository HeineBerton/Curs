#pragma once
#include "ClientDataBase.h"
#include "Logger.h"
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
#include <iostream>
#include <random>
#include <string>
#include <stdexcept>

class Authenticator {
public:
    // Конструктор
    Authenticator(const std::string& db_filename, Logger& logger);

    // Метод для проверки логина и пароля из строки
    bool validateUserFromString(const std::string& input);

private:
    // Структура для хранения результата разбиения строки
    struct Result {
        std::string login;
        std::string salt;
        std::string hash;
    };

    // Проверка существования логина
    bool isLoginExists(const std::string& login);
    // Хэширование пароля с солью
    std::string hashPassword(const std::string& salt, const std::string& password);

    // Метод для разделения строки на логин, соль и хэш
    Result splitUserData(const std::string& input);

    // Вспомогательный метод для разделения строки
    Result splitString(const std::string& input);

    ClientDataBase db; // База данных пользователей
    Logger& logger; // Логгер для ведения журнала
};

