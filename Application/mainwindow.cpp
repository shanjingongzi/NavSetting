#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "navcontroller.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  controller = new NavController(this);
  controller->Initialize();
  setCentralWidget(controller->View());
}

MainWindow::~MainWindow() { delete ui; }
