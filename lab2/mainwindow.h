#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbInputFile_clicked();
    void on_pbOutputFile_clicked();

    void on_pbEncipherDecipher_clicked();

    void on_leRegStart_returnPressed();

private:
    Ui::MainWindow *ui;

    void generateKey();
    QByteArray binaryStringToBytes(const QString &binaryString);

    QByteArray inputFile, outputFile;

    QByteArray key;
    int fileSize;
};
#endif // MAINWINDOW_H
