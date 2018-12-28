#include "mainwindow.h"
#include "init.h"

#include <QApplication>
#include <QMessageBox>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (Init::getInstance().appInit() != 0) {
        QMessageBox::critical(nullptr, "Error", "Init error!");
        return -1;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
