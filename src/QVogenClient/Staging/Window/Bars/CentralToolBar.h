#ifndef CENTRALTOOLBAR_H
#define CENTRALTOOLBAR_H

#include <QWidget>

class CentralToolBarPrivate;

class CentralToolBar : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CentralToolBar)
public:
    CentralToolBar(QWidget *parent = nullptr);
    ~CentralToolBar();

public:
    enum CursorModes {
        Select,
        Sketch,
        Free,
    };

protected:
    CentralToolBar(CentralToolBarPrivate &d, QWidget *parent = nullptr);

    QScopedPointer<CentralToolBarPrivate> d_ptr;
};

#endif // CENTRALTOOLBAR_H
