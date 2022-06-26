#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "BasicWindow.h"

#include "CCoupleTabFrame.h"
#include "CentralTabWidget.h"

#include "Modules/CentralNavBar.h"
#include "Modules/CentralToolBar.h"
#include "Modules/QCommandPalette.h"

#include "Panels/ControlPanel.h"
#include "Panels/UtterPanel.h"

#include <QEventLoop>
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

    int showCommands(QCommandPalette::CommandType type);

    template <class P>
    int showLineEdit(const QString &hint, P previewer, const QString &placeholder = QString(),
                     QString *res = nullptr, bool hold = false);

protected:
    // UI
    QWidget *m_widget;
    QGridLayout *m_layout;

    CentralToolBar *m_tools;
    CentralNavBar *m_nav;

    CentralTabWidget *m_tabs;

    CCoupleTabFrame *m_frame;
    ControlPanel *m_ctrlPanel;
    UtterPanel *m_utterPanel;

    CCoupleTabBarCard *m_ctrlCard;
    CCoupleTabBarCard *m_utterCard;

    QCommandPalette *m_cp;
    QSet<QEventLoop *> m_loops; // Cached event loops

    // Managers
    TabManager *m_tabMgr;
    EventManager *m_eventMgr;
    ActionManager *m_actionMgr;

    void resizeEvent(QResizeEvent *event) override;
    void customEvent(QEvent *event) override;

private:
    void adjustSelector();
};

template <class P>
int MainWindow::showLineEdit(const QString &hint, P previewer, const QString &placeholder,
                             QString *res, bool hold) {
    int result = -1;

    QEventLoop loop;

    auto slot1 = [&previewer](const QString &text) mutable { previewer(text); };

    auto slot2 = [&loop]() mutable { loop.quit(); };
    auto slot3 = [&result, &loop](QListWidgetItem *item) mutable {
        Q_UNUSED(item);
        result = 0;
        loop.quit();
    };

    auto conn1 = connect(m_cp, &QCommandPalette::textChanged, this, slot1);
    auto conn2 = connect(m_cp, &QCommandPalette::abandoned, this, slot2);
    auto conn3 = connect(m_cp, &QCommandPalette::activated, this, slot3);

    m_cp->showLineEdit(hint, placeholder, hold);

    adjustSelector();

    m_loops.insert(&loop);
    loop.exec();
    m_loops.remove(&loop);

    // Don't use obj->disconnect() casually because it's really dangerous!
    disconnect(conn1);
    disconnect(conn2);
    disconnect(conn3);

    if (res) {
        *res = m_cp->text();
    }

    m_cp->finish();

    return result;
}

#endif // MAINWINDOW_H
