
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDomDocument>
#include <QMainWindow>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QComboBox;
class QFile;
class QGroupBox;
class QLabel;
class QListWidget;
class QSqlRelationalTableModel;
class QTableView;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &authorTable, const QString &bookTable,
               QFile *bookDetails, QWidget *parent = nullptr);

private slots:
    void about();
    void addbook();
    void changeAuthor(int row);
    void deletebook();
    void showbookDetails(QModelIndex index);
    void showAuthorProfile(QModelIndex index);
    void updateHeader(QModelIndex, int, int);

private:
    void adjustHeader();
    QGroupBox *createbookGroupBox();
    QGroupBox *createAuthorGroupBox();
    QGroupBox *createDetailsGroupBox();
    void createMenuBar();
    void decreasebookCount(QModelIndex authorIndex);
    void getTrackList(QDomNode book);
    QModelIndex indexOfAuthor(const QString &author);
    void readbookData();
    void removebookFromDatabase(QModelIndex book);
    void removebookFromFile(int id);
    void showImageLabel();

    QTableView *bookView;
    QComboBox *authorView;
    QListWidget *trackList;

    QLabel *iconLabel;
    QLabel *imageLabel;
    QLabel *profileLabel;
    QLabel *titleLabel;

    QDomDocument bookData;
    QFile *file;
    QSqlRelationalTableModel *model;
};

#endif
