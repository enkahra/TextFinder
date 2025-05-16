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
}


//THIS IS ONLY FOR TESTING


TextFinder::~TextFinder()
{
    delete ui;
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
        searchTextCount = 0;
        currentMatchIndex = 0;

        // Move to start and count all matches
        ui->textEdit->moveCursor(QTextCursor::Start);
        bool found;
        while ((found = ui->textEdit->find(searchString, QTextDocument::FindWholeWords))) {
            searchTextCount++;
        }

        if (searchTextCount == 0) {
            ui->matchInfoLabel->setText("No matches found");
            QMessageBox::information(this, "Not found", "No matches found for: " + searchString);
            return;
        }

        // Move cursor back to top and select the first match
        ui->textEdit->moveCursor(QTextCursor::Start);
        bool firstFound = ui->textEdit->find(searchString, QTextDocument::FindWholeWords);
        if (firstFound) {
            currentMatchIndex = 1;
            ui->matchInfoLabel->setText(QString("Match %1 of %2").arg(currentMatchIndex).arg(searchTextCount));
        }

        return; // Done for this button click
    }

    // Same search term — find next match
    bool found = ui->textEdit->find(searchString, QTextDocument::FindWholeWords);
    if (found) {
        currentMatchIndex++;
        if (currentMatchIndex > searchTextCount)
            currentMatchIndex = 1; // Wrap manually just in case
        ui->matchInfoLabel->setText(QString("Match %1 of %2").arg(currentMatchIndex).arg(searchTextCount));
    } else {
        // Reached end — wrap to start
        ui->textEdit->moveCursor(QTextCursor::Start);
        found = ui->textEdit->find(searchString, QTextDocument::FindWholeWords);
        if (found) {
            currentMatchIndex = 1;
            ui->matchInfoLabel->setText(QString("Match %1 of %2").arg(currentMatchIndex).arg(searchTextCount));
        } else {
            // Something changed, no matches found anymore
            currentMatchIndex = 0;
            searchTextCount = 0;
            ui->matchInfoLabel->setText("No matches found");
            QMessageBox::information(this, "Not found", "No matches found for: " + searchString);
        }
    }
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
}

