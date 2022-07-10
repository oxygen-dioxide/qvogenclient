#ifndef MACMAINWINDOW_H
#define MACMAINWINDOW_H

#include <QMainWindow>

class MacWindowCtl;

class MacMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MacMainWindow(QWidget *parent = nullptr);
    ~MacMainWindow();

    void setTitleBarHeight(int height);

    int titleBarHeight() const;

protected:
    QScopedPointer<MacWindowCtl> m_macCtl;

signals:
};

#endif // MACMAINWINDOW_H
