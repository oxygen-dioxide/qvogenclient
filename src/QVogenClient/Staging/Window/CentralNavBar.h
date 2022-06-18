#ifndef CENTRALNAVBAR_H
#define CENTRALNAVBAR_H

#include <QWidget>

class CentralNavBarPrivate;

class CentralNavBar : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CentralNavBar)
public:
    explicit CentralNavBar(QWidget *parent = nullptr);
    ~CentralNavBar();

protected:
    CentralNavBar(CentralNavBarPrivate &d, QWidget *parent = nullptr);

    QScopedPointer<CentralNavBarPrivate> d_ptr;
};

#endif // CENTRALNAVBAR_H
