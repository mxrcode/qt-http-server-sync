#include <QCoreApplication>

#include "httpserver.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HttpServer server;

    if (server.listen(QHostAddress("127.0.0.1"), 8080)) {
        qDebug() << "Server is listening on port 8080...";
    } else {
        qCritical() << "Failed to start the server!";
        return 1;
    }

    return a.exec();
}
