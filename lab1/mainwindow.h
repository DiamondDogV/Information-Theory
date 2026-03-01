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
    void on_pbDecimalCipher_clicked();
    void on_pbVigenereCipher_clicked();
    void on_pbFileRead_clicked();
    void on_pbFileWrite_clicked();

private:
    QString decimalCipher(QString message, int key);
    QString vigenereCipher(QString message, QString key);
    int alphabetSize;
    QString alphabet;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
