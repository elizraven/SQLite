#include "mainwindow.h"
#include "dialog.h"

#include <QtWidgets>
#include <QtSql>
#include <QtXml>

extern int uniquebookId;
extern int uniqueAuthorId;

MainWindow::MainWindow(const QString &authorTable, const QString &bookTable,
                       QFile *bookDetails, QWidget *parent)
     : QMainWindow(parent)
{
    file = bookDetails;
    readbookData();

    model = new QSqlRelationalTableModel(this);
    model->setTable(bookTable);
    model->setRelation(2, QSqlRelation(authorTable, "id", "author"));
    model->select();

    QGroupBox *authors = createAuthorGroupBox();
    QGroupBox *books = createbookGroupBox();
    QGroupBox *details = createDetailsGroupBox();

    authorView->setCurrentIndex(0);
    uniquebookId = model->rowCount();
    uniqueAuthorId = authorView->count();

    connect(model, &QSqlRelationalTableModel::rowsInserted,
            this, &MainWindow::updateHeader);
    connect(model, &QSqlRelationalTableModel::rowsRemoved,
            this, &MainWindow::updateHeader);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(authors, 0, 0);
    layout->addWidget(books, 1, 0);
    layout->addWidget(details, 0, 1, 2, 1);
    layout->setColumnStretch(1, 1);
    layout->setColumnMinimumWidth(0, 500);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
    createMenuBar();

    showImageLabel();
    resize(850, 400);
    setWindowTitle(tr("База данных книг русских писателей"));
}

void MainWindow::changeAuthor(int row)
{
    if (row > 0) {
        QModelIndex index = model->relationModel(2)->index(row, 1);
        model->setFilter("author = '" + index.data().toString() + '\'') ;
        showAuthorProfile(index);
    } else if (row == 0) {
        model->setFilter(QString());
        showImageLabel();
    } else {
        return;
    }
}

void MainWindow::showAuthorProfile(QModelIndex index)
{
    QSqlRecord record = model->relationModel(2)->record(index.row());

    QString name = record.value("author").toString();
    QString count = record.value("bookcount").toString();
    profileLabel->setText(tr("Автор: %1 \n" \
                             "Количество книг: %2").arg(name).arg(count));

    profileLabel->show();
    iconLabel->show();

    titleLabel->hide();
    trackList->hide();
    imageLabel->hide();
}

void MainWindow::showbookDetails(QModelIndex index)
{
    QSqlRecord record = model->record(index.row());

    QString author = record.value("author").toString();
    QString title = record.value("title").toString();
    QString year = record.value("year").toString();
    QString bookId = record.value("bookid").toString();

    showAuthorProfile(indexOfAuthor(author));
    titleLabel->setText(tr("Название: %1 (%2)").arg(title).arg(year));
    titleLabel->show();


    QDomNodeList books = bookData.elementsByTagName("book");
    for (int i = 0; i < books.count(); ++i) {
        QDomNode book = books.item(i);

        }
    }





void MainWindow::addbook()
{
    Dialog *dialog = new Dialog(model, bookData, file, this);
    int accepted = dialog->exec();

    if (accepted == 1) {
        int lastRow = model->rowCount() - 1;
        bookView->selectRow(lastRow);
        bookView->scrollToBottom();
        showbookDetails(model->index(lastRow, 0));
    }
}

void MainWindow::deletebook()
{
    QModelIndexList selection = bookView->selectionModel()->selectedRows(0);

    if (!selection.empty()) {
        QModelIndex idIndex = selection.at(0);
        int id = idIndex.data().toInt();
        QString title = idIndex.sibling(idIndex.row(), 1).data().toString();
        QString author = idIndex.sibling(idIndex.row(), 2).data().toString();

        QMessageBox::StandardButton button;
        button = QMessageBox::question(this, tr("Удалить книгу"),
                                       tr("Вы уверены что хотите удалить '%1', '%2'?")
                                       .arg(title, author),
                                       QMessageBox::Yes | QMessageBox::No);

        if (button == QMessageBox::Yes) {
            removebookFromFile(id);
            removebookFromDatabase(idIndex);
            decreasebookCount(indexOfAuthor(author));

            showImageLabel();
        }
    } else {
        QMessageBox::information(this, tr("Удалить книгу"),
                                 tr("Выберите книгу, которую хотите удалить."));
    }
}

void MainWindow::removebookFromFile(int id)
{

    QDomNodeList books = bookData.elementsByTagName("book");

    for (int i = 0; i < books.count(); ++i) {
        QDomNode node = books.item(i);
        if (node.toElement().attribute("id").toInt() == id) {
            bookData.elementsByTagName("archive").item(0).removeChild(node);
            break;
        }
    }

}

void MainWindow::removebookFromDatabase(QModelIndex index)
{
    model->removeRow(index.row());
}

void MainWindow::decreasebookCount(QModelIndex authorIndex)
{
    int row = authorIndex.row();
    QModelIndex bookCountIndex = authorIndex.sibling(row, 2);
    int bookCount = bookCountIndex.data().toInt();

    QSqlTableModel *authors = model->relationModel(2);

    if (bookCount == 1) {
        authors->removeRow(row);
        showImageLabel();
    } else {
        authors->setData(bookCountIndex, QVariant(bookCount - 1));
    }
}

void MainWindow::readbookData()
{
    if (!file->open(QIODevice::ReadOnly))
        return;

    if (!bookData.setContent(file)) {
        file->close();
        return;
    }
    file->close();
}

QGroupBox* MainWindow::createAuthorGroupBox()
{
    authorView = new QComboBox;
    authorView->setModel(model->relationModel(2));
    authorView->setModelColumn(1);

    connect(authorView, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::changeAuthor);

    QGroupBox *box = new QGroupBox(tr("Автор"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(authorView, 0, 0);
    box->setLayout(layout);

    return box;
}

QGroupBox* MainWindow::createbookGroupBox()
{
    QGroupBox *box = new QGroupBox(tr("Книги"));

    bookView = new QTableView;
    bookView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bookView->setSortingEnabled(true);
    bookView->setSelectionBehavior(QAbstractItemView::SelectRows);
    bookView->setSelectionMode(QAbstractItemView::SingleSelection);
    bookView->setShowGrid(false);
    bookView->verticalHeader()->hide();
    bookView->setAlternatingRowColors(true);
    bookView->setModel(model);
    adjustHeader();

    QLocale locale = bookView->locale();
    locale.setNumberOptions(QLocale::OmitGroupSeparator);
    bookView->setLocale(locale);

    connect(bookView, &QTableView::clicked,
            this, &MainWindow::showbookDetails);
    connect(bookView, &QTableView::activated,
            this, &MainWindow::showbookDetails);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(bookView, 0, 0);
    box->setLayout(layout);

    return box;
}

QGroupBox* MainWindow::createDetailsGroupBox()
{
    QGroupBox *box = new QGroupBox(tr("Информация"));

    profileLabel = new QLabel;
    profileLabel->setWordWrap(true);
    profileLabel->setAlignment(Qt::AlignBottom);

    titleLabel = new QLabel;
    titleLabel->setWordWrap(true);
    titleLabel->setAlignment(Qt::AlignBottom);

    iconLabel = new QLabel();
    iconLabel->setAlignment(Qt::AlignBottom | Qt::AlignRight);


    imageLabel = new QLabel;
    imageLabel->setWordWrap(true);
    imageLabel->setAlignment(Qt::AlignCenter);


    trackList = new QListWidget;

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(imageLabel, 0, 0, 3, 2);
    layout->addWidget(profileLabel, 0, 0);
    layout->addWidget(iconLabel, 0, 1);
    layout->addWidget(titleLabel, 1, 0, 1, 2);
    layout->addWidget(trackList, 2, 0, 1, 2);
    layout->setRowStretch(2, 1);
    box->setLayout(layout);

    return box;
}

void MainWindow::createMenuBar()
{
    QAction *addAction = new QAction(tr("&Добавить книгу в базу данных..."), this);
    QAction *deleteAction = new QAction(tr("&Удалить книгу из базы данных..."), this);
    QAction *quitAction = new QAction(tr("&Выход"), this);




    QMenu *fileMenu = menuBar()->addMenu(tr("&Опции"));
    fileMenu->addAction(addAction);
    fileMenu->addAction(deleteAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);



    connect(addAction, &QAction::triggered,
            this, &MainWindow::addbook);
    connect(deleteAction, &QAction::triggered,
            this, &MainWindow::deletebook);
    connect(quitAction, &QAction::triggered,
            this, &MainWindow::close);

}

void MainWindow::showImageLabel()
{
    profileLabel->hide();
    titleLabel->hide();
    iconLabel->hide();
    trackList->hide();

    imageLabel->show();
}

QModelIndex MainWindow::indexOfAuthor(const QString &author)
{
    QSqlTableModel *authorModel = model->relationModel(2);

    for (int i = 0; i < authorModel->rowCount(); i++) {
        QSqlRecord record =  authorModel->record(i);
        if (record.value("author") == author)
            return authorModel->index(i, 1);
    }
    return QModelIndex();
}

void MainWindow::updateHeader(QModelIndex, int, int)
{
    adjustHeader();
}

void MainWindow::adjustHeader()
{
    bookView->hideColumn(0);
    bookView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    bookView->resizeColumnToContents(2);
    bookView->resizeColumnToContents(3);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About"),
            tr(""));
}
