#ifndef DIALOG_H
#define DIALOG_H

#include <QtWidgets>
#include <QtSql>
#include <QtXml>

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QSqlRelationalTableModel *books, QDomDocument details,
           QFile *output, QWidget *parent = nullptr);

private slots:
    void revert();
    void submit();

private:
    int addNewbook(const QString &title, int authorId);
    int addNewAuthor(const QString &name);
    void addTracks(int bookId, const QStringList &tracks);
    QDialogButtonBox *createButtons();
    QGroupBox *createInputWidgets();
    int findAuthorId(const QString &author);
    static int generatebookId();
    static int generateAuthorId();
    void increasebookCount(QModelIndex authorIndex);
    QModelIndex indexOfAuthor(const QString &author);

    QSqlRelationalTableModel *model;
    QDomDocument bookDetails;
    QFile *outputFile;

    QLineEdit *authorEditor;
    QLineEdit *titleEditor;
    QSpinBox *yearEditor;
    QLineEdit *tracksEditor;
};

#endif
