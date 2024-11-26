#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "navcontroller.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    controller = new NavController(this);
    controller->Initialize();
    setCentralWidget(controller->View());
    connect(controller,&NavController::MessageChanged,[this](const QString &msg){
        this->statusBar()->showMessage(msg);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
