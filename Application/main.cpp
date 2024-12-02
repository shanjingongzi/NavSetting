#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    QFile file("E:/Code/NavSetting/build/bin/Debug/styles/styles.qss");
    if (file.open(QFile::ReadOnly)) {
        QString stypeSheet = QString::fromUtf8(file.readAll());
        a.setStyleSheet(stypeSheet);
    }
    return a.exec();
}
