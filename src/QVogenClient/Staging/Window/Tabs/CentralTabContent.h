#ifndef CENTRALTABCONTENT_H
#define CENTRALTABCONTENT_H

#include <QWidget>

class CentralTabContentPrivate;

class CentralTabContent : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CentralTabContent)
public:
    explicit CentralTabContent(QWidget *parent = nullptr);
    ~CentralTabContent();

protected:
    CentralTabContent(CentralTabContentPrivate &d, QWidget *parent = nullptr);

    QScopedPointer<CentralTabContentPrivate> d_ptr;
};

#endif // CENTRALTABCONTENT_H
