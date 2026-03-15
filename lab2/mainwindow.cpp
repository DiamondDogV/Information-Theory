#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QString>
#include <QRegExp>
#include <QRegExpValidator>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->leRegStart->setValidator(new QRegExpValidator(QRegExp("[01]+"), this));

    key = "";
    fileSize = 1024;
}

MainWindow::~MainWindow()
{
    delete ui;
}







void MainWindow::on_pbInputFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,                                   // родительское окно
        tr("Выберите файл для шифрования"),     // заголовок диалога
        QCoreApplication::applicationDirPath(), // начальная директория (домашняя папка)
        tr("Все файлы (*.*);")                  // фильтры
        );

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    inputFile = file.readAll();
    file.close();

    QString binaryString;
    for (char byte : inputFile) {
        if (binaryString.length() > 1024)
            break;
        for (int i = 7; i >= 0; i--) {
            bool bit = (byte >> i) & 1;
            binaryString.append(bit ? '1' : '0');
        }
    }

    fileSize = inputFile.length();
    generateKey();
    ui->teMessage->setPlainText(binaryString);
}


void MainWindow::on_pbOutputFile_clicked()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString binDir = appDir + "/bin";

    QString messagePath = binDir + "/Message.bin";

    QFile messageFile(messagePath);
    if (messageFile.open(QIODevice::WriteOnly)) {
        messageFile.write(inputFile);
        messageFile.close();
    }

    QString cipherPath = binDir + "/CipherText.bin";
    QFile cipherFile(cipherPath);
    if (cipherFile.open(QIODevice::WriteOnly)) {
        cipherFile.write(outputFile);
        cipherFile.close();
    }
}








void MainWindow::on_pbEncipherDecipher_clicked()
{
    outputFile.clear();
    outputFile.reserve(inputFile.size());

    if (inputFile.size() != key.size()) {
        QMessageBox::warning(this, "Warning", "Проверьте ключ");
        return;
    }

    for (int i=0; i<inputFile.size(); i++)
        outputFile.append(inputFile[i] ^ key[i]);

    QString binaryString;
    for (char byte : outputFile) {
        if (binaryString.length() > 1024)
            break;
        for (int i = 7; i >= 0; i--) {
            bool bit = (byte >> i) & 1;
            binaryString.append(bit ? '1' : '0');
        }
    }

    ui->teCipherText->setPlainText(binaryString);
}

void MainWindow::on_leRegStart_returnPressed()
{
    generateKey();
}

void MainWindow::generateKey() {
    QString a = ui->leRegStart->text();
    key.clear();

    if (a.length() != 39) {
        QMessageBox::warning(this, "Warning", "Проверьте ключ");
        return;
    }

    char* b = a.toLatin1().data();
    int keyByte = 0, keyCnt = 0;
    QByteArray keyShow;

    while (key.length() < fileSize) {
        if (keyShow.length() < 1024)
            keyShow.append(b[0]);

        keyByte = keyByte * 2 + (b[0]-'0');
        keyCnt++;
        if (keyCnt == 8) {
            key.append(keyByte);
            keyByte = 0;
            keyCnt = 0;
        }

        int bit4 = b[39-4] - '0';
        int bit39 = b[39-39] - '0';
        int xorResult = bit4 ^ bit39;

        memmove(b, b + 1, 38);
        b[38] = xorResult + '0';
    }

    ui->teGenKey->setText(QString::fromLatin1(keyShow));
}

