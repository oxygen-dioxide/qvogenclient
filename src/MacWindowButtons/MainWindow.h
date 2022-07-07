#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "BasicMainWindow.h"

class MainWindow : public BasicMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
