#include "Auth.h"
using namespace std;

// Конструктор
Authenticator::Authenticator(const std::string& db_filename, Logger& logger) 
    : db(db_filename), logger(logger) {
    // Инициализация базы данных или других ресурсов
}

// Новый метод для проверки логина и пароля из строки
bool Authenticator::validateUserFromString(const std::string& input) {
    // Разделяем строку на логин, соль и хэш
    Result result = splitUserData(input);
    std::cout << result.login << std::endl;
    std::cout << result.salt << std::endl;
    std::cout << result.hash << std::endl;

    // Проверяем наличие логина в базе данных
    if (!isLoginExists(result.login)) {
        return false; // Логин не существует
    }

    // Извлекаем пароль из базы данных по логину
    std::string password = db[result.login]; // db возвращает пароль для данного логина
    std::cout << password << std::endl;
    // Генерируется хэш с использованием соли и пароля
    std::string hashedPassword = hashPassword(result.salt, password);
    std::cout << hashedPassword << std::endl;
    // Сравнение хэшей
    return hashedPassword == result.hash;
}

// Проверка существования логина
bool Authenticator::isLoginExists(const std::string& login) {
    return db.userExists(login); // Предполагается, что метод userExists реализован в ClientDataBase
}

std::string Authenticator::hashPassword(const std::string& salt, const std::string& password) {
    namespace CPP = CryptoPP;
    string HashAndPass = salt + password; 
    CPP::SHA256 hash;                        
    CPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];
    hash.CalculateDigest(digest, (const CPP::byte*)HashAndPass.data(), HashAndPass.size());
    string Hash;
    CryptoPP::StringSource(digest, sizeof(digest), true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(Hash)));
    std::cout << "Сервер сгенерировал хэш: "+ Hash << std::endl;
    return Hash;
}

// Метод для разделения строки на логин, соль и хэш
Authenticator::Result Authenticator::splitUserData(const std::string& input) {
    return splitString(input);
}

// Вспомогательный метод для разделения строки
Authenticator::Result Authenticator::splitString(const std::string& input) {
    Result result;

    if (input.length() < 80) {
        throw std::invalid_argument("Input string is too short.");
    }

    // Извлекаем hash
    result.hash = input.substr(input.length() - 64, 64);

    // Извлекаем salt
    result.salt = input.substr(input.length() - 80, 16);

    // Извлекаем login
    result.login = input.substr(0, input.length() - 80);
    return result;
}
