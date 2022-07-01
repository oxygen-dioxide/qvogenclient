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
    int showLineEdit(QCommandPalette::Hint *hint);
    int showList(const QStringList &list, int current, const QString &placeholder);

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

#endif // MAINWINDOW_H
