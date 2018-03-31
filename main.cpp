#include "database.h"

#include "mainwindow.h"

#include <QApplication>
#include <QFile>

#include <stdlib.h>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(masterdetail);

    QApplication app(argc, argv);

    if (!createConnection())
        return EXIT_FAILURE;

    QFile bookDetails("bookdetails.xml");
    MainWindow window("authors", "books", &bookDetails);
    window.show();
    return app.exec();
}
