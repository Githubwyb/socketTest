#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_clearRecvPushButton_clicked();

    void on_clearSendPushButton_clicked();

    void on_clientTypeComboBox_currentIndexChanged(int index);

    void on_socketProtocol_currentIndexChanged(int index);

    void on_connectPushButton_clicked();

    void tcpClientDisconnected();

    void tcpClientReceiveData();

    void tcpServerDisconnected();

    void tcpServerNewConnect();

    void tcpServerReceiveData();

    void on_sendPushButton_clicked();

private:
    /*
     * @description tcp客户端连接
     * @return 0，成功；其他，错误码
     */
    int tcpClientConnect();

    /*
     * @description tcp服务器连接
     * @return 0，成功；其他，错误码
     */
    int tcpServerConnect();

    Ui::MainWindow *ui;
    std::shared_ptr<QTcpSocket> m_pTcpSocket = nullptr;         //tcp客户端连接指针
    std::shared_ptr<QTcpServer> m_pTcpServer = nullptr;         //tcp服务端监听指针
    QTcpSocket *m_pTcpServerSocket = nullptr;                   //tcp服务端的连接指针
};

#endif // MAINWINDOW_H
