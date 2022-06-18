#ifndef TPIANOAREA_H
#define TPIANOAREA_H

#include <QLabel>
#include <QWidget>

#include "TPianoKey.h"

class TPianoArea : public QWidget {
    Q_OBJECT
public:
    explicit TPianoArea(QWidget *parent = nullptr);
    explicit TPianoArea(int currentHeight, QWidget *parent = nullptr);
    ~TPianoArea();

private:
    void init();

public:
    int currentHeight() const;
    void setCurrentHeight(int currentHeight);

protected:
    QList<TPianoKey *> m_whites;
    QList<TPianoKey *> m_blacks;
    QList<QLabel *> m_labels;

    QLabel *m_toneLabel;
    int m_currentNoteNum;
    int m_currentHeight;

    void adjustHeight();
    void handleUpdateToneLabel();

private:
    void timerEvent(QTimerEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

signals:
};

#endif // TPIANOAREA_H
