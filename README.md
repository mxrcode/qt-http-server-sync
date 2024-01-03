# qt-http-server-sync

This is a simple header-only HTTP server for Qt, designed to provide basic functionality for handling HTTP requests.

## Usage
To use the HTTP server in your Qt application, include the httpserver.hpp header file and create an instance of the HttpServer class. You can then update or remove URL mappings as needed using the provided methods.

```cpp
#include <QCoreApplication>
#include "httpserver.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HttpServer server;

    // Example: Update URL mapping
    server.updateUrlMappings("about", "<h1>New Content</h1>", "text/html");

    // Example: Remove URL mapping
    server.removeUrlMappings("about");

    if (server.listen(QHostAddress::Any, 8080)) {
        qDebug() << "Server is listening on port 8080...";
    } else {
        qCritical() << "Failed to start the server!";
        return 1;
    }

    return a.exec();
}
```

## Features
* Header-only HTTP server for Qt.
* Ability to update and remove URL mappings dynamically.
* Basic handling of incoming HTTP requests.
* Provides a simple example demonstrating server usage.

## Disclaimer
The code is distributed without any warranty, and the author shall not be held liable for any damages or consequences arising from the use, distribution, or modification of this code. Users are encouraged to review and comply with the terms outlined in the LGPL-3.0 license.

## Contributing
Feel free to contribute to the development and improvement of this HTTP server. Pull requests and suggestions are welcome.

## License
**qt-http-server-sync** is released under the GNU Lesser General Public License (LGPL-3.0), which means it is free and open-source software that you can use, modify, and distribute. Please refer to the [LICENSE file](https://github.com/mxrcode/qt-http-server-sync/blob/main/LICENSE) for more details.
