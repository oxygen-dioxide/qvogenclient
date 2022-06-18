#include "MainWindow.h"
#include "CMenu.h"

#include "DataManager.h"

#include "ActionManager.h"
#include "EventManager.h"
#include "TabManager.h"

#include "Types/Events.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent) : BasicWindow(parent) {
    setAcceptDrops(true);

    m_layout = new QGridLayout();
    m_layout->setMargin(0);
    m_layout->setSpacing(0);

    m_widget = new QWidget();
    m_widget->setAttribute(Qt::WA_StyledBackground);
    m_widget->setLayout(m_layout);

    m_tools = new CentralToolBar();
    m_nav = new CentralNavBar();
    m_tabs = new CentralTabWidget();
    m_frame = new CCoupleTabFrame();

    int row = 1;
    auto tabBar = m_tabs->moveOutTabBarWidget();

    m_layout->addWidget(tabBar, 0, 0, 1, 2);
    m_layout->addWidget(m_tools, row, 0, 1, 1);
    m_layout->addWidget(m_nav, row, 1, 1, 1);
    m_layout->addWidget(m_frame, row + 1, 0, 1, 2);

    m_frame->setWidget(m_tabs);
    this->setCentralWidget(m_widget);

    m_tabMgr = new TabManager(this);
    m_eventMgr = new EventManager(this);
    m_actionMgr = new ActionManager(this);

    m_tabMgr->load();
    m_eventMgr->load();
    m_actionMgr->load();

    Q_TR_NOTIFY(MainWindow)

    // Initialize
    {
        // Update All Menus
        QEventImpl::MenuUpdateRequestEvent e(ActionImpl::StateMask);
        QApplication::sendEvent(this, &e);
    }
}

MainWindow::~MainWindow() {
}

void MainWindow::reloadStrings() {
    m_titleBar->reloadStrings();
    m_actionMgr->reloadStrings();
}

CentralToolBar *MainWindow::toolBar() const {
    return m_tools;
}

CentralNavBar *MainWindow::navBar() const {
    return m_nav;
}

CentralTabWidget *MainWindow::tabWidget() const {
    return m_tabs;
}

CCoupleTabFrame *MainWindow::frame() const {
    return m_frame;
}

TabManager *MainWindow::tabMgr() const {
    return m_tabMgr;
}

EventManager *MainWindow::eventMgr() const {
    return m_eventMgr;
}

ActionManager *MainWindow::actionMgr() const {
    return m_actionMgr;
}
