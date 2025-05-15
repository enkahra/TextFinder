#ifndef TEXTFINDER_H
#define TEXTFINDER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class TextFinder;
}
QT_END_NAMESPACE

class TextFinder : public QWidget
{
    Q_OBJECT

public:
    TextFinder(QWidget *parent = nullptr);
    QString lastSearchText;
    qint64 searchTextCount = 0;
    qint64 currentMatchIndex = 0;
    ~TextFinder();

private slots:
    void on_FindButton_clicked();

private:
    Ui::TextFinder *ui;
    void loadTextFile();
};
#endif // TEXTFINDER_H
