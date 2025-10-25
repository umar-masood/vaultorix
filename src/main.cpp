#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>

#include <string>
#include <fstream>
#include <iostream>
#include <unordered_set>

void lower(std::string &str) {
    for (auto &c : str) c = std::tolower(static_cast<unsigned char>(c));
}

bool checkDisposableEmail(std::string email) {
    std::ifstream file("C:/Users/umara/Downloads/tempMails.conf");
    if (!file.is_open()) {
        std::cerr << "File cannot opened...";
        return false;
    }

    std::unordered_set<std::string> tempMails;
    tempMails.reserve(72000);

    std::string line;
    while(std::getline(file, line)) {
        if (!line.empty() && (line.back() == '\r' || line.back() == '\n')) line.pop_back();
        lower(line);
        if (!line.empty()) tempMails.insert(line);
    }
    file.close();

    size_t pos = email.find('@');
    if (pos == std::string::npos) return false;

    std::string domain = email.substr(pos + 1);
    lower(domain);

    return (tempMails.find(domain) != tempMails.end());
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    std::cout << std::boolalpha <<  checkDisposableEmail("pheasant6114@google.com");

    return 0;
}