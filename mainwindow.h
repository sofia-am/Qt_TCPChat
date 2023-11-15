#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QSerialPort>
#include <QComboBox>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QTextBrowser *tb;
    //QSerialPort *port;
    QComboBox *cbPort;
    QLineEdit *leText;
    QPushButton *pbSend;
    QTcpServer *server;
    QString serverAdr;
    int serverPort;
    QUrl serverInfo;

    QList<QTcpSocket*> clients;

    void runServer();
private slots:
    void onNewMessage();
    void onOpenClosePort();
    void onSendMessage();
    void onNewConnection();
};
#endif // MAINWINDOW_H
