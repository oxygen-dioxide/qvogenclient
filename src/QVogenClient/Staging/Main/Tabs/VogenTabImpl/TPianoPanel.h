#ifndef TPIANOPANEL_H
#define TPIANOPANEL_H

#include "VogenPanel.h"

#include "NotesArea/TNotesArea.h"
#include "NotesArea/TNotesScroll.h"
#include "PianoArea/TPianoArea.h"
#include "PianoArea/TPianoScroll.h"
#include "SectionsArea/TSectionsArea.h"

#include "QTypeList.h"

#include <QGridLayout>

class TPianoPanel : public VogenPanel {
    Q_OBJECT
    Q_PROPERTY(QTypeList styleDataN READ styleDataN WRITE setStyleDataN NOTIFY styleDataChanged)
    Q_PROPERTY(QTypeList styleDataS READ styleDataS WRITE setStyleDataS NOTIFY styleDataChanged)
public:
    explicit TPianoPanel(VogenTab *tab, QWidget *parent = nullptr);
    ~TPianoPanel();

    friend class TuningScope;

private:
    void init();

public:
    int sectionCount() const;
    void setSectionCount(int sectionCount);

    int currentWidth() const;
    void setCurrentWidth(int currentWidth);

    int currentHeight() const;
    void setCurrentHeight(int currentHeight);

    int currentQuantize() const;
    void setCurrentQuantize(int currentQuantize);

    void setCurrentSizes(int w, int h, int q);

    int blankSections() const;
    void setBlankSections(int blankSections);

    // Proxy style
    QTypeList styleDataN() const;
    void setStyleDataN(const QTypeList &list);

    QTypeList styleDataS() const;
    void setStyleDataS(const QTypeList &list);

protected:
    TNotesScroll *m_notesScroll;
    TNotesArea *m_notesArea;

    TPianoScroll *m_pianoScroll;
    TPianoArea *m_pianoArea;

    TSectionsArea *m_sectionsArea;

    QGridLayout *m_layout;

    void wheelEvent(QWheelEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

public:
    TNotesArea *notesArea() const;
    TPianoArea *pianoArea() const;
    TSectionsArea *sectionsArea() const;

private:
    void handleNotesAreaMoved(int dx, int dy);
    void handleNotesAreaZoomRequested(Qt::Orientation orient, int offset, const QPoint &anchor);

signals:
    void styleDataChanged();
};

#endif // TPIANOPANEL_H
