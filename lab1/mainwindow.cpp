#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTextStream>
#include <QString>
#include <QFile>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qApp->setStyleSheet(
        "QMessageBox QLabel { color: white; font-size: 12pt; font-weight: bold; }"
        );
}
MainWindow::~MainWindow()
{
    delete ui;
}





void MainWindow::on_pbFileRead_clicked()
{
    QFile file("message.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Проверьте файл message.txt");
        return;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    QString content = stream.readAll();

    file.close();

    ui->pteMessage->setPlainText(content);
}
void MainWindow::on_pbFileWrite_clicked()
{
    QFile file("ciphertext.txt");

    if (!file.open(QIODevice::WriteOnly  | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Проверьте файл ciphertext.txt");
        return;
    }

    QString content = ui->pteCipherText->toPlainText();

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << content;

    file.close();
}










int gcd (int a, int b) {
    if (!b)return a;
    return gcd(b, a%b);
}

void MainWindow::on_pbDecimalCipher_clicked()
{
    QString key = ui->pteDecimalKey->toPlainText();
    alphabetSize = 26;
    alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJLMNOPQRSTUVWXYZ";

    QString filteredKey;
    for (int i=0; i<key.length(); i++) {
        if (key[i].isDigit()) {
            filteredKey.append(key[i]);
        }
    }
    ui->pteDecimalKey->setPlainText(filteredKey);

    if (filteredKey.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пустой ключ");
        return;
    }

    int intKey = filteredKey.toInt();

    if (gcd(intKey, alphabetSize) > 1) {
        QMessageBox::warning(this, "Ошибка", "Неправильный ключ");
        return;
    }

    QString message = ui->pteMessage->toPlainText();
    QString cipherText = decimalCipher(message, intKey);
    ui->pteCipherText->setPlainText(cipherText);
}

QString MainWindow::decimalCipher(QString message, int key) {
    QString cipherText;

    for (int i=0; i<message.length(); i++) {
        QChar c=message[i];
        if (c >= 'a' && c <= 'z') {
            int i = c.unicode() - 'a';
            i = (i * key) % alphabetSize;
            c = QChar(i + 'a');
        }
        if (c >= 'A' && c <= 'Z') {
            int i = c.unicode() - 'A';
            i = (i * key) % alphabetSize;
            c = QChar(i + 'A');
        }
        cipherText.append(c);
    }

    return cipherText;
}










void MainWindow::on_pbVigenereCipher_clicked()
{
    QString key = ui->pteVigenereKey->toPlainText();
    alphabetSize = 33;
    alphabet = "абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";

    QString filteredKey;
    for (int i=0; i<key.length(); i++) {
        if (alphabet.indexOf(key[i]) != -1) {
            if (alphabet.indexOf(key[i]) > 32)
                key[i] = alphabet[alphabet.indexOf(key[i])-32];
            filteredKey.append(key[i]);
        }
    }
    ui->pteVigenereKey->setPlainText(filteredKey);

    if (filteredKey.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пустой ключ");
        return;
    }

    QString message = ui->pteMessage->toPlainText();
    QString cipherText = vigenereCipher(message, filteredKey);
    ui->pteCipherText->setPlainText(cipherText);
}

QString MainWindow::vigenereCipher(QString message, QString key) {
    QString cipherText;
    int keyInd=0;
    bool upCase;

    for (int i=0; i<message.length(); i++) {
        int c = alphabet.indexOf(message[i]);
        if (c == -1) {
            cipherText.append(message[i]);
            continue;
        }

        int k = alphabet.indexOf(key[keyInd++]);
        if (keyInd == key.length())
            keyInd = 0;

        if (c<33)
            upCase = false;
        else {
            upCase = true;
            c -= 33;
        }

        c = (c + k) % alphabetSize;

        if (upCase)
            c += 33;

        cipherText.append(alphabet[c]);
    }

    return cipherText;
}

