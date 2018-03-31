HEADERS   = database.h \
            dialog.h \
            mainwindow.h
SOURCES   = dialog.cpp \
            main.cpp \
            mainwindow.cpp

QT += sql widgets
QT += xml widgets
requires(qtConfig(tableview))




