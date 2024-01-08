/*

Simple header-only HTTP Server for QT

The code provided here serves as a simple HTTP server for QT and is authored by https://github.com/mxrcode/
The author has released this code under the GNU Lesser General Public License version 3.0.

Please note that the code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
Users are encouraged to review and comply with the terms and conditions outlined in the LGPL-3.0 license, which can be found at https://www.gnu.org/licenses/lgpl-3.0.html.

The author shall not be held liable for any damages or consequences arising from the use, distribution, or modification of this code.
Users are advised to exercise due diligence and discretion when incorporating or adapting this code for their own projects.

*/

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextStream>
#include <QThreadPool>
#include <QHash>

class HttpServer : public QTcpServer
{
    Q_OBJECT

public:
    HttpServer(QObject *parent = nullptr) : QTcpServer(parent) {
        urlMappings["/"] = {"Hello, World!", "text/plain"};
    }

    void updateUrlMappings(const QString &url, const QString &content, const QString &contentType = "text/plain") {
        urlMappings["/" + url] = { content, contentType };
    }
    void removeUrlMappings(const QString &url) {
        urlMappings.remove("/" + url);
    }

protected:
    void incomingConnection(qintptr socketDescriptor) override
    {
        QTcpSocket *clientSocket = new QTcpSocket(this);
        if (!clientSocket->setSocketDescriptor(socketDescriptor)) {
            qDebug() << "Error setting socket descriptor:" << clientSocket->errorString();
            return;
        }

        connect(clientSocket, &QTcpSocket::readyRead, this, &HttpServer::onReadyRead);
        connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    }

    void onReadyRead()
    {
        QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
        if (!clientSocket)
            return;

        QTextStream in(clientSocket);

        QString requestData = clientSocket->property("requestData").toString();
        requestData += in.readAll();
        clientSocket->setProperty("requestData", requestData);

        if (requestData.contains("\r\n\r\n")) {
            processRequest(requestData, clientSocket);
            clientSocket->setProperty("requestData", QString()); // Сброс буфера
            clientSocket->disconnectFromHost();
        }
    }

    QString getRealIpFromHeader(QTcpSocket *clientSocket)
    {
        const QByteArray &requestData = clientSocket->property("requestData").toByteArray();
        const QString xRealIpHeader = "X-Real-IP";

        for (const QString &line : QTextStream(requestData).readAll().split("\r\n")) {
            if (line.startsWith(xRealIpHeader + ":"))
                return line.mid(xRealIpHeader.length() + 1).trimmed();
        }

        return clientSocket->peerAddress().toString();
    }

    void processRequest(const QString &requestData, QTcpSocket *clientSocket)
    {
        QStringList lines = requestData.split("\r\n");
        if (lines.isEmpty()) {
            return;
        }

        QString firstLine = lines.first();
        QStringList parts = firstLine.split(" ");
        if (parts.size() < 2) {
            return;
        }

        QString method = parts[0];
        QString path = parts[1];

        QString clientIp = getRealIpFromHeader(clientSocket);

        QString response;
        QString response_status = "404 Not Found";

        if (urlMappings.contains(path)) {
            const UrlMapping &mapping = urlMappings[path];

            response_status = "200 OK";

            response = "HTTP/1.1 " + response_status + "\r\n"
                       "Content-Type: " + mapping.contentType + "\r\n"
                       "Connection: close\r\n"
                       "\r\n"
                       "" + mapping.content + "";
        } else {

            response_status = "404 Not Found";

            response = "HTTP/1.1 " + response_status + "\r\n"
                       "Content-Type: text/plain\r\n"
                       "Connection: close\r\n"
                       "\r\n"
                       "Not Found";

        }

        qDebug() << clientIp + " " + method + " " + path + " " + response_status;

        clientSocket->write(response.toUtf8());
    }

private:
    struct UrlMapping {
        QString content;
        QString contentType;
    };

    QHash<QString, UrlMapping> urlMappings;
};

#endif // HTTPSERVER_H

/*
    Usage Example:

    HttpServer server;

    server.updateUrlMappings("about", "<h1>New Content</h1>", "text/html");
    server.removeUrlMappings("about");

    if (server.listen(QHostAddress::Any, 8080)) {
        qDebug() << "Server is listening on port 8080...";
    } else {
        qCritical() << "Failed to start the server!";
        return 1;
    }
*/
