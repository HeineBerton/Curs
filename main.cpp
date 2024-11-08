#include "Logger.h"
#include "server.h"

#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <system_error>
using namespace std;
namespace po = boost::program_options;

int main(int argc, char** argv)
{
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "Вывести справку")
        ("data,d", po::value<string>()->default_value("/etc/vcalc.conf"),
         "Имя файла с базой клиентов (необходимо запускать от имени администратора для доступа к этому файлу)")
        ("log,l", po::value<string>()->default_value("/var/log/vcalc.log"),
         "Имя файла с журналом работы (необходимо запускать от имени администратора для записи в этот файл)")
        ("port,p", po::value<int>()->default_value(33333), "Номер порта для прослушки");

    po::variables_map vm;

    try {
        if (argc == 1) {
            cout << desc << endl;
            return 0;
        }
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << endl;
            return 0;
        }

        string clientDataFile = vm["data"].as<string>();
        string logFile = vm["log"].as<string>();
        int port = vm["port"].as<int>();

        ifstream clientData(clientDataFile);
        if (!clientData.is_open()) {
            string error_message = "Файл с базой клиентов " + clientDataFile + " не найден.";
            cerr << error_message << endl;
            return 1; 
        }

        ofstream logFileStream(logFile, ios::app); 
        if (!logFileStream.is_open()) {
            string error_message = "Не удалось открыть файл журнала: " + logFile;
            cerr << error_message << endl;
            return 1; 
        }

        Logger logger(logFile); 

        logger.log(INFO, "Файл с базой клиентов: " + clientDataFile);
        logger.log(INFO, "Файл с журналом работы: " + logFile);
        logger.log(INFO, "Порт: " + to_string(port));

        Server server(port, clientDataFile, logger);
        server.start(); 
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl; 
        return 1; 
    }

    return 0; 
}
