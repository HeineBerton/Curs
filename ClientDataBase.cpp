#include "ClientDataBase.h"
using namespace std;
ClientDataBase::ClientDataBase(const string& filename)
{
    loadDatabase(filename);
}
bool ClientDataBase::isLoginExists(const string& login)
{
    return database.find(login) != database.end();                   
}
bool ClientDataBase::userExists(const string& login)
{
    return isLoginExists(login); // Используем существующий метод
}
std::string ClientDataBase::getPasswordHash(const string& login)
{
    if (isLoginExists(login))
    {
        return database[login]; // Возвращаем пароль (или хэш, если вы его храните)
    }
    throw std::invalid_argument("Логин не существует"); // Обработка ошибки
}
void ClientDataBase::loadDatabase(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line, ':')) {
            string login = line;
            string password;
            getline(file, password); 
            password.erase(password.find_last_not_of(" \t\r\n") + 1);
            database[login] = password; // Тут пароль без знака переноса строки
        }
        file.close();
    } else {
        cerr << "Ошибка: Не удалось открыть файл " << filename << endl;
    }
}
std::string& ClientDataBase::operator[](const std::string& login)
{
    return database[login]; 
}
const std::string& ClientDataBase::operator[](const std::string& login) const 
{
    return database.at(login); 
}
