#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "BasicWindow.h"

#include "CCoupleTabFrame.h"
#include "CentralNavBar.h"
#include "CentralTabWidget.h"
#include "CentralToolBar.h"

#include <QGridLayout>

class TabManager;
class EventManager;
class ActionManager;

class MainWindow : public BasicWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void reloadStrings();

public:
    CentralToolBar *toolBar() const;
    CentralNavBar *navBar() const;
    CentralTabWidget *tabWidget() const;
    CCoupleTabFrame *frame() const;

    TabManager *tabMgr() const;
    EventManager *eventMgr() const;
    ActionManager *actionMgr() const;

protected:
    // UI
    QWidget *m_widget;
    QGridLayout *m_layout;

    CentralToolBar *m_tools;
    CentralNavBar *m_nav;
    CentralTabWidget *m_tabs;

    CCoupleTabFrame *m_frame;

    // Managers
    TabManager *m_tabMgr;
    EventManager *m_eventMgr;
    ActionManager *m_actionMgr;
};
#endif // MAINWINDOW_H
