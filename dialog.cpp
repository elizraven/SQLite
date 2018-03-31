#include "dialog.h"

int uniquebookId;
int uniqueAuthorId;

Dialog::Dialog(QSqlRelationalTableModel *books, QDomDocument details,
               QFile *output, QWidget *parent)
     : QDialog(parent)
{
    model = books;
    bookDetails = details;
    outputFile = output;

    QGroupBox *inputWidgetBox = createInputWidgets();
    QDialogButtonBox *buttonBox = createButtons();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(inputWidgetBox);
    layout->addWidget(buttonBox);
    setLayout(layout);

    setWindowTitle(tr("Добавить книгу"));
}

void Dialog::submit()
{
    QString author = authorEditor->text();
    QString title = titleEditor->text();

    if (author.isEmpty()) {
        QString message(tr("Пожалуйста, введите имя автора "
                           "и название книги."));
        QMessageBox::information(this, tr("Добавить книгу"), message);
    } else {
        int authorId = findAuthorId(author);
        int bookId = addNewbook(title, authorId);

        QStringList tracks;
        tracks = tracksEditor->text().split(',', QString::SkipEmptyParts);
        addTracks(bookId, tracks);

        increasebookCount(indexOfAuthor(author));
        accept();
    }
}

int Dialog::findAuthorId(const QString &author)
{
    QSqlTableModel *authorModel = model->relationModel(2);
    int row = 0;

    while (row < authorModel->rowCount()) {
        QSqlRecord record = authorModel->record(row);
        if (record.value("author") == author)
            return record.value("id").toInt();
        else
            row++;
    }
    return addNewAuthor(author);
}


int Dialog::addNewAuthor(const QString &name)
{
    QSqlTableModel *authorModel = model->relationModel(2);
    QSqlRecord record;

    int id = generateAuthorId();

    QSqlField f1("id", QVariant::Int);
    QSqlField f2("author", QVariant::String);
    QSqlField f3("bookcount", QVariant::Int);

    f1.setValue(QVariant(id));
    f2.setValue(QVariant(name));
    f3.setValue(QVariant(0));
    record.append(f1);
    record.append(f2);
    record.append(f3);

    authorModel->insertRecord(-1, record);
    return id;
}

int Dialog::addNewbook(const QString &title, int authorId)
{
    int id = generatebookId();
    QSqlRecord record;

    QSqlField f1("bookid", QVariant::Int);
    QSqlField f2("title", QVariant::String);
    QSqlField f3("authorid", QVariant::Int);
    QSqlField f4("year", QVariant::Int);

    f1.setValue(QVariant(id));
    f2.setValue(QVariant(title));
    f3.setValue(QVariant(authorId));
    f4.setValue(QVariant(yearEditor->value()));
    record.append(f1);
    record.append(f2);
    record.append(f3);
    record.append(f4);

    model->insertRecord(-1, record);
    return id;
}

void Dialog::addTracks(int bookId, const QStringList &tracks)
{
    QDomElement bookNode = bookDetails.createElement("book");
    bookNode.setAttribute("id", bookId);

    for (int i = 0; i < tracks.count(); ++i) {
        QString trackNumber = QString::number(i);
        if (i < 10)
            trackNumber.prepend('0');

        QDomText textNode = bookDetails.createTextNode(tracks.at(i));

        QDomElement trackNode = bookDetails.createElement("track");
        trackNode.setAttribute("number", trackNumber);
        trackNode.appendChild(textNode);

        bookNode.appendChild(trackNode);
    }

    QDomNodeList archive = bookDetails.elementsByTagName("archive");
    archive.item(0).appendChild(bookNode);


}

void Dialog::increasebookCount(QModelIndex authorIndex)
{
    QSqlTableModel *authorModel = model->relationModel(2);

    QModelIndex bookCountIndex;
    bookCountIndex = authorIndex.sibling(authorIndex.row(), 2);

    int bookCount = bookCountIndex.data().toInt();
    authorModel->setData(bookCountIndex, QVariant(bookCount + 1));
}


void Dialog::revert()
{
    authorEditor->clear();
    titleEditor->clear();
    yearEditor->setValue(QDate::currentDate().year());
    tracksEditor->clear();
}

QGroupBox *Dialog::createInputWidgets()
{
    QGroupBox *box = new QGroupBox(tr("Добавить книгу"));

    QLabel *authorLabel = new QLabel(tr("Автор:"));
    QLabel *titleLabel = new QLabel(tr("Навание:"));
    QLabel *yearLabel = new QLabel(tr("Год:"));
    QLabel *tracksLabel = new QLabel(tr("Описание:"));

    authorEditor = new QLineEdit;
    titleEditor = new QLineEdit;

    yearEditor = new QSpinBox;
    yearEditor->setMinimum(1);
    yearEditor->setMaximum(QDate::currentDate().year());
    yearEditor->setValue(yearEditor->maximum());
    yearEditor->setReadOnly(false);

    tracksEditor = new QLineEdit;

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(authorLabel, 0, 0);
    layout->addWidget(authorEditor, 0, 1);
    layout->addWidget(titleLabel, 1, 0);
    layout->addWidget(titleEditor, 1, 1);
    layout->addWidget(yearLabel, 2, 0);
    layout->addWidget(yearEditor, 2, 1);
    layout->addWidget(tracksLabel, 3, 0, 1, 2);
    layout->addWidget(tracksEditor, 4, 0, 1, 2);
    box->setLayout(layout);

    return box;
}

QDialogButtonBox *Dialog::createButtons()
{
    QPushButton *closeButton = new QPushButton(tr("&Закрыть"));
    QPushButton *revertButton = new QPushButton(tr("&Вернуть"));
    QPushButton *submitButton = new QPushButton(tr("&Применить"));

    closeButton->setDefault(true);

    connect(closeButton, &QPushButton::clicked, this, &Dialog::close);
    connect(revertButton, &QPushButton::clicked,  this, &Dialog::revert);
    connect(submitButton, &QPushButton::clicked, this, &Dialog::submit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(submitButton, QDialogButtonBox::ResetRole);
    buttonBox->addButton(revertButton, QDialogButtonBox::ResetRole);
    buttonBox->addButton(closeButton, QDialogButtonBox::RejectRole);

    return buttonBox;
}

QModelIndex Dialog::indexOfAuthor(const QString &author)
{
    QSqlTableModel *authorModel = model->relationModel(2);

    for (int i = 0; i < authorModel->rowCount(); ++i) {
        QSqlRecord record =  authorModel->record(i);
        if (record.value("author") == author)
            return authorModel->index(i, 1);
    }

    return QModelIndex();
}

int Dialog::generateAuthorId()
{
    uniqueAuthorId += 1;
    return uniqueAuthorId;
}

int Dialog::generatebookId()
{
    uniquebookId += 1;
    return uniquebookId;
}
