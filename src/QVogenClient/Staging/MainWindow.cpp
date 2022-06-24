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

    // Central Widget
    m_layout = new QGridLayout();
    m_layout->setMargin(0);
    m_layout->setSpacing(0);

    m_widget = new QWidget();
    m_widget->setAttribute(Qt::WA_StyledBackground);
    m_widget->setLayout(m_layout);

    // Widgets
    m_tools = new CentralToolBar();
    m_tools->setObjectName("central-tool-bar");

    m_nav = new CentralNavBar();
    m_tools->setObjectName("central-nav-bar");

    m_tabs = new CentralTabWidget();
    m_tools->setObjectName("central-tabs");

    m_frame = new CCoupleTabFrame();
    m_tools->setObjectName("central-frame");

    m_ctrlPanel = new ControlPanel();
    m_tools->setObjectName("ctrl-panel");

    m_utterPanel = new UtterPanel();
    m_tools->setObjectName("utter-panel");

    // Tab Bar
    auto tabBar = m_tabs->moveOutTabBarWidget();
    m_layout->addWidget(tabBar, 0, 0, 1, 2);

    int row = 1;
    m_layout->addWidget(m_tools, row, 0, 1, 1);
    m_layout->addWidget(m_nav, row, 1, 1, 1);
    m_layout->addWidget(m_frame, row + 1, 0, 1, 2);

    m_frame->setWidget(m_tabs);
    this->setCentralWidget(m_widget);

    // Cards
    m_ctrlCard = new CCoupleTabBarCard();
    m_ctrlCard->setObjectName("ctrl-card");
    m_ctrlCard->setWidget(m_ctrlPanel);

    m_utterCard = new CCoupleTabBarCard();
    m_utterCard->setObjectName("utter-card");
    m_utterCard->setWidget(m_utterPanel);

    m_frame->rightBar()->firstBar()->addCard(m_utterCard);
    m_frame->bottomBar()->firstBar()->addCard(m_ctrlCard);

    // Command Palette
    m_cp = new QCommandPalette(this);
    m_cp->setObjectName("command-palette");
    m_cp->hide();

    // Managers
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

    m_ctrlCard->setText(tr("Control"));
    m_utterCard->setText(tr("Utterances"));
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

int MainWindow::showCommands(QCommandPalette::CommandType type) {
    int result = -1;
    QEventLoop loop;

    // auto slot1 = [&previewFuction](int index) mutable {
    //     if (previewFuction) {
    //         previewFuction(index);
    //     }
    // };

    auto slot2 = [&loop]() mutable { loop.quit(); };
    auto slot3 = [&result, &loop](QListWidgetItem *item) mutable {
        Q_UNUSED(item);
        result = 0;
        loop.quit();
    };

    // auto conn1 = connect(selector, &ComboSelector::currentIndexChanged, this, slot1);
    auto conn2 = connect(m_cp, &QCommandPalette::abandoned, this, slot2);
    auto conn3 = connect(m_cp, &QCommandPalette::activated, this, slot3);

    m_cp->showCommands(type);

    adjustSelector();

    loop.exec();

    // Don't use obj->disconnect() casually because it's really dangerous!
    // disconnect(conn1);
    disconnect(conn2);
    disconnect(conn3);

    m_cp->finish();

    return result;
}

int MainWindow::showLineEdit(const QString &hint, void previewer(const QString &)) {
    int result = -1;
    QEventLoop loop;

    auto slot1 = [&previewer](const QString &text) mutable {
        if (previewer) {
            previewer(text);
        }
    };

    auto slot2 = [&loop]() mutable { loop.quit(); };
    auto slot3 = [&result, &loop](QListWidgetItem *item) mutable {
        Q_UNUSED(item);
        result = 0;
        loop.quit();
    };

    auto conn1 = connect(m_cp, &QCommandPalette::textChanged, this, slot1);
    auto conn2 = connect(m_cp, &QCommandPalette::abandoned, this, slot2);
    auto conn3 = connect(m_cp, &QCommandPalette::activated, this, slot3);

    m_cp->showLineEdit(hint);

    adjustSelector();

    loop.exec();

    // Don't use obj->disconnect() casually because it's really dangerous!
    disconnect(conn1);
    disconnect(conn2);
    disconnect(conn3);

    m_cp->finish();

    return result;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    BasicWindow::resizeEvent(event);

    adjustSelector();
}

void MainWindow::adjustSelector() {
    if (m_cp->isVisible()) {
        auto w = centralWidget();
        m_cp->adjustSize();
        m_cp->resize(w->width() / 2, w->height() / 2);
        m_cp->move((width() - m_cp->width()) / 2, w->y());
    }
}
