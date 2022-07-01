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

    enum PlayButton {
        Play,
        Stop,
        Synth,
    };

    CursorModes cursorMode() const;
    void setCursorMode(CursorModes mode);

    void setPlayFlags(int flags);

signals:
    void cursorModeChanged(int mode);
    void playButtonClicked(int id);

protected:
    CentralToolBar(CentralToolBarPrivate &d, QWidget *parent = nullptr);

    QScopedPointer<CentralToolBarPrivate> d_ptr;

private:
    void _q_buttonClicked();
};

#endif // CENTRALTOOLBAR_H
