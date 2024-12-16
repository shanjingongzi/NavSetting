#include "mainwindow.h"

#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <qdatetime.h>
#include <Windows.h>

int main(int argc, char* argv[])
{
    QDateTime currentDate = QDateTime::currentDateTime();
    auto msg              = currentDate.toString();
    auto limitDate        = QDateTime::fromString("Tue Dec 10 16:25:27 2024");
    auto days             = currentDate.daysTo(limitDate);
    if (days>10 ||days<-10) {
        return -1;
    }
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
