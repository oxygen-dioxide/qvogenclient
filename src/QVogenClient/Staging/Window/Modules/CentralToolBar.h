#ifndef CENTRALTOOLBAR_H
#define CENTRALTOOLBAR_H

#include <QWidget>

class MainWindow;
class CentralToolBarPrivate;

class CentralToolBar : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CentralToolBar)
public:
    CentralToolBar(QWidget *parent = nullptr);
    ~CentralToolBar();

public:
    enum CursorModes {
        NoCursor = -1,
        Select = 0,
        Sketch,
        Free,
    };

    CursorModes cursorMode() const;
    void setCursorMode(CursorModes mode);

signals:
    void cursorModeChanged(int mode);

protected:
    CentralToolBar(CentralToolBarPrivate &d, QWidget *parent = nullptr);

    QScopedPointer<CentralToolBarPrivate> d_ptr;

private:
    void _q_buttonClicked();
};

#endif // CENTRALTOOLBAR_H
