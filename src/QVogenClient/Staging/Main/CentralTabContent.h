#ifndef CENTRALTABCONTENT_H
#define CENTRALTABCONTENT_H

#include <QWidget>

// Used as a inner widget of CentralTab
class CentralTabContent : public QWidget {
    Q_OBJECT
public:
    explicit CentralTabContent(QWidget *parent = nullptr);
    ~CentralTabContent();

signals:
};

#endif // CENTRALTABCONTENT_H
