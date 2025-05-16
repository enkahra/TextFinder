#include "textfinder.h"
#include "./ui_textfinder.h"
#include <QFile>
#include <QTextStream>
#include <QTextCursor>
#include <QTextDocument>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>

TextFinder::TextFinder(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TextFinder)
{
    ui->setupUi(this);  // Also calls connectSlotsByName(this) internally
    connect(ui->textEdit->document(), &QTextDocument::contentsChanged, this, &TextFinder::onTextChanged);

}

TextFinder::~TextFinder()
{
    delete ui;
}

void TextFinder::indexShowFunc(int currentMatchIndex, int searchTextCount)
{
    ui->matchInfoLabel->setText(QString("Match %1 of %2").arg(currentMatchIndex).arg(searchTextCount));
}

void TextFinder::on_FindButton_clicked()
{
    QString searchString = ui->lineEdit->text().trimmed();

    if (searchString.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Search box is empty.");
        return;
    }

    // If new search term, count matches and reset everything
    if (searchString != lastSearchText) {
        lastSearchText = searchString;
        matchList.clear();

        // Move to start and collect all matches
        QTextDocument *doc = ui->textEdit->document();
        QTextCursor cursor(doc);
        cursor.movePosition(QTextCursor::Start);

        while (!cursor.isNull() && !cursor.atEnd()) {
            cursor = doc->find(searchString, cursor, QTextDocument::FindWholeWords | QTextDocument::FindCaseSensitively);
            if (!cursor.isNull()) {
                matchList.append(cursor);
            }
        }

        searchTextCount = matchList.size();

        if (searchTextCount == 0) {
            currentMatchIndex = 0;
            ui->matchInfoLabel->setText("No matches found");
            QMessageBox::information(this, "Not found", "No matches found for: " + searchString);
            return;
        }

        // For testing remove later
        ui->TotalMatchCount->setText(QString("%1").arg(searchTextCount));

        // Select the first match
        currentMatchIndex = 1;
        ui->textEdit->setTextCursor(matchList[currentMatchIndex - 1]);
        indexShowFunc(currentMatchIndex, searchTextCount);
        return;
    }

    // Same search term — go to next match
    if (searchTextCount == 0 || matchList.isEmpty()) {
        currentMatchIndex = 0;
        ui->matchInfoLabel->setText("No matches found");
        return;
    }

    currentMatchIndex++;
    if (currentMatchIndex > searchTextCount) {
        currentMatchIndex = 1; // wrap to start
    }

    ui->textEdit->setTextCursor(matchList[currentMatchIndex - 1]);
    indexShowFunc(currentMatchIndex, searchTextCount);
}


void TextFinder::on_OpenTextFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Text File", "", "Text Files (*.txt);;All Files (*)");
    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open the input file.");
        return;
    }

    QTextStream in(&inputFile);
    QString content = in.readAll();
    inputFile.close();

    ui->textEdit->setPlainText(content);

    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);
    ui->textEdit->setTextCursor(cursor);
    ui->OpenedTextFileLabel->setText(QString("Opened file path: %1").arg(filePath));
}

void TextFinder::on_lineEdit_returnPressed()
{
    TextFinder::on_FindButton_clicked();
}

void TextFinder::goToMatchIndex(int index)
{
    if (index < 1 || index > matchList.size()) {
        QMessageBox::warning(this, "Invalid Index", "Index out of range.");
        return;
    }

    currentMatchIndex = index;
    ui->textEdit->setTextCursor(matchList[currentMatchIndex - 1]);
    indexShowFunc(currentMatchIndex, searchTextCount);
}

void TextFinder::onTextChanged()
{
    // Clear previous search state because content changed
    matchList.clear();
    searchTextCount = 0;
    currentMatchIndex = 0;
    lastSearchText.clear();

    ui->matchInfoLabel->setText("Search reset due to text change.");
    ui->TotalMatchCount->clear();
}


void TextFinder::on_lineEdit_2_returnPressed()
{
    bool ok;
    int index = ui->lineEdit_2->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid number.");
        return;
    }

    goToMatchIndex(index);  // This sets currentMatchIndex internally
}

