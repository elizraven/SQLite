#ifndef DATABASE_H
#define DATABASE_H

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
            QObject::tr("Unable to establish a database connection.\n"
                        "This example needs SQLite support. Please read "
                        "the Qt SQL driver documentation for information how "
                        "to build it.\n\n"
                        "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }

    QSqlQuery query;

    query.exec("create table authors (id int primary key, "
                                     "author varchar(40), "
                                     "bookcount int)");

    query.exec("insert into authors values(0, '<Все>', 0)");
    query.exec("insert into authors values(1, 'Федор Достоевский', 2)");
    query.exec("insert into authors values(2, 'Лев Толстой', 3)");
    query.exec("insert into authors values(3, 'Александр Пушкин', 3)");

    query.exec("create table books (bookid int primary key, "
                                     "title varchar(50), "
                                     "authorid int, "
                                     "year int)");

    query.exec("insert into books values(1, 'Преступление и наказание ', 1, "
                       "1866)");
    query.exec("insert into books values(2, 'Идиот', 1, 1869)");
    query.exec("insert into books values(3, 'Война и мир', 2, "
                       "1868)");
    query.exec("insert into books values(4, 'Анна Каренина', 2, 1877)");
    query.exec("insert into books values(5, 'Путь жизни', 2, 1910)");
    query.exec("insert into books values(6, 'Евгений Онегин', 3, 1837)");
    query.exec("insert into books values(7, 'Капитанская дочка ', 3, 1836)");
    query.exec("insert into books values(8, 'Дубровский ', 3, 1842)");

    return true;
}

#endif


