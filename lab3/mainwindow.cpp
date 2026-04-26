#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rabincryptosystem.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

// Метод обработки данных
void MainWindow::processData(bool shouldSaveFile) {
    long long p = ui->spinBoxP->value();
    long long q = ui->spinBoxQ->value();
    long long b = ui->spinBoxB->value();
    long long n = p * q;
    ui->spinBoxN->setValue(n);

    if (!RabinCryptoSystem::isValid(p) || !RabinCryptoSystem::isValid(q)) {
        QMessageBox::warning(this, "Ошибка", "p и q должны быть простыми числами и давать остаток 3 при делении на 4");
        return;
    }
    if (p == q) {
        QMessageBox::warning(this, "Внимание", "p и q должны быть разными");
        return;
    }
    if (n <= 256) {
        QMessageBox::warning(this, "Внимание", "Недостаточно большие p и q");
        return;
    }
    if (n < b) {
        QMessageBox::warning(this, "Внимание", "Число b должно быть меньше n");
        return;
    }

    QString input = ui->textEditM->toPlainText().trimmed();
    if(input.isEmpty()){
        QMessageBox::warning(this, "Ошибка", "Поле ввода пусто!");
        return;
    }

    bool isEncrypt = (ui->CmbBox->currentIndex() == 0);
    QString screenText; // Текст для вывода на экран (с пояснениями)
    QString fileText;   // Чистый текст для сохранения (только данные)

    if (isEncrypt) {
        // --- ШИФРОВАНИЕ ---
        QByteArray data = input.toUtf8();
        QStringList screenLines;
        QStringList fileNumbers;

        for (int i = 0; i < data.size(); ++i) {
            unsigned char byte = static_cast<unsigned char>(data.at(i));
            long long c = RabinCryptoSystem::encrypt(byte, n, b);

            // Формат для экрана: 'A' (65) -> 12345
            QString charView = QChar(byte).isPrint() ? QString("'%1'").arg(QChar(byte)) : "?";
            screenLines << QString("%1 (%2) -> %3").arg(charView).arg(byte).arg(c);

            // Формат для файла/дешифровки: "12345"
            fileNumbers << QString::number(c);
        }
        screenText = screenLines.join("\n");
        fileText = fileNumbers.join(" ");
    }
    else {
        // --- ДЕШИФРОВАНИЕ ---
        QStringList blocks = input.split(QRegularExpression("[\\s,]+"), Qt::SkipEmptyParts);
        QByteArray decryptedBytes;
        QStringList screenLines;

        for (const QString& block : blocks) {
            bool ok;
            long long c = block.toLongLong(&ok);
            if (!ok) continue;

            QVector<long long> roots = RabinCryptoSystem::decrypt(c, p, q, b);
            bool found = false;
            for (long long root : roots) {
                if (root >= 0 && root < 256) {
                    unsigned char byte = static_cast<unsigned char>(root);
                    decryptedBytes.append(byte);

                    QString charView = QChar(byte).isPrint() ? QString("'%1'").arg(QChar(byte)) : "?";
                    screenLines << QString("%1 -> %2 (%3)").arg(c).arg(charView).arg(byte);

                    found = true;
                    break;
                }
            }
            if (!found) {
                decryptedBytes.append('?');
                screenLines << QString("%1 -> ?").arg(c);
            }
        }
        screenText = screenLines.join("\n");
        fileText = QString::fromUtf8(decryptedBytes);
    }

    // Выводим красивый список на экран
    ui->textEditC->setText(screenText);

    // В файл сохраняем только полезные данные (числа при шифровании, текст при дешифровании)
    if (shouldSaveFile) {
        QString path = QFileDialog::getSaveFileName(this, "Сохранить", "", "BIN (*.bin)");
        if (!path.isEmpty()) {
            QFile file(path);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(fileText.toUtf8());
                file.close();
            }
        }
    }
}

// Слот кнопки "Выполнить"
void MainWindow::on_ButExec_clicked() {
    //processData(false); // Обработка без принудительного сохранения
}

// Слот кнопки "Загрузить"
void MainWindow::processFile(const QString& inputPath, const QString& outputPath) {
    long long p = ui->spinBoxP->value();
    long long q = ui->spinBoxQ->value();
    long long b = ui->spinBoxB->value();
    long long n = p * q;
    ui->spinBoxN->setValue(n);

    if (!RabinCryptoSystem::isValid(p) || !RabinCryptoSystem::isValid(q)) {
        QMessageBox::warning(this, "Ошибка", "p и q должны быть простыми числами и давать остаток 3 при делении на 4");
        return;
    }
    if (p == q) {
        QMessageBox::warning(this, "Внимание", "p и q должны быть разными");
        return;
    }
    if (n <= 256) {
        QMessageBox::warning(this, "Внимание", "Недостаточно большие p и q");
        return;
    }
    if (n < b){
        QMessageBox::warning(this, "Внимание", "Число b должно быть меньше n");
        return;
    }

    QFile inFile(inputPath);
    QFile outFile(outputPath);

    if (!inFile.open(QIODevice::ReadOnly) || !outFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файлы");
        return;
    }

    bool isEncrypt = (ui->CmbBox->currentIndex() == 0);
    QStringList inputPreview, outputPreview;
    const int maxPreview = 100; // Сколько элементов выводить в интерфейс
    int count = 0;

    if (isEncrypt) {
        // --- ШИФРОВАНИЕ (Байты -> Текст с числами) ---
        QByteArray data = inFile.readAll();
        QTextStream outStream(&outFile); // Используем поток текста для записи чисел

        for (int i = 0; i < data.size(); ++i) {
            unsigned char m = static_cast<unsigned char>(data.at(i));
            long long c = RabinCryptoSystem::encrypt(m, n, b);

            // Записываем число как текст + пробел
            outStream << c << " ";

            if (count < maxPreview) {
                inputPreview << QString::number(m);
                outputPreview << QString::number(c);
                count++;
            }
        }
    }
    else {
        // --- ДЕШИФРОВАНИЕ (Текст с числами -> Бинарный поток байтов) ---
        QTextStream inStream(&inFile); // Читаем как текст, чтобы распознать числа
        while (!inStream.atEnd()) {
            long long c;
            inStream >> c; // Читаем следующее число (пробелы пропускаются автоматически)
            if (inStream.status() != QTextStream::Ok) break;

            QVector<long long> roots = RabinCryptoSystem::decrypt(c, p, q, b);

            bool found = false;
            for (long long root : roots) {
                // Ищем корень-байт
                if (root >= 0 && root < 256) {
                    unsigned char m = static_cast<unsigned char>(root);

                    // Записываем ровно 1 байт в файл (бинарно)
                    outFile.write(reinterpret_cast<const char*>(&m), 1);
                    //outFile.putChar(m);

                    if (count < maxPreview) {
                        inputPreview << QString::number(c);
                        outputPreview << QString::number(m);
                        count++;
                    }
                    found = true;
                    break;
                }
            }
        }
    }

    // Обновление интерфейса (всегда числа)
    if (count >= maxPreview) {
        inputPreview << "...";
        outputPreview << "...";
    }
    ui->textEditM->setPlainText(inputPreview.join(" "));
    ui->textEditC->setPlainText(outputPreview.join(" "));

    inFile.close();
    outFile.close();
    QMessageBox::information(this, "Готово", "Операция успешно завершена");
}

// Изменяем логику кнопки "Загрузить/Выполнить для файла"
void MainWindow::on_ButFile_clicked() {
    QString inPath = QFileDialog::getOpenFileName(this, "Выберите файл для обработки");
    if (inPath.isEmpty()) return;

    QString outPath = "M:/BSUIR/archive/rabin/build/5_15_18_mingw64_static-Debug/debug/testfiles/res.bin";

    processFile(inPath, outPath);
}

MainWindow::~MainWindow()
{
    delete ui;
}
