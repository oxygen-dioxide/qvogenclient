#include "MainWindow.h"

#include <QDebug>
#include <QLabel>
#include <QLayout>

MainWindow::MainWindow(QWidget *parent) : BasicMainWindow(parent) {
    auto label = new QLabel();
    label->setText("Frameless Window");
    label->setAlignment(Qt::AlignCenter);
    setCentralWidget(label);
}

MainWindow::~MainWindow() {
}
