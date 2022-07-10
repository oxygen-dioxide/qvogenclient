#ifndef MACWINDOWCTL_H
#define MACWINDOWCTL_H

#include <QWidget>

class MacWindowCtl : public QObject {
    Q_OBJECT
public:
    explicit MacWindowCtl(QWidget *w);
    virtual ~MacWindowCtl();

    bool setup();

public:
    QWidget *widget() const;

    void setCloseBtnEnabled(bool enabled);
    void setMinBtnEnabled(bool enabled);
    void setZoomBtnEnabled(bool enabled);
    void setTitlebarVisible(bool visible);

    void setTitleBarHeight(int height);
    int titleBarHeight() const {
        return m_titleBarHeight;
    }

    bool isCloseBtnEnabled() const {
        return m_bIsCloseBtnEnabled;
    }
    bool isMinBtnEnabled() const {
        return m_bIsMinBtnEnabled;
    }
    bool isZoomBtnEnabled() const {
        return m_bIsZoomBtnEnabled;
    }

    bool titleBarVisible() const {
        return m_bTitleBarVisible;
    }

    QWidgetList titleBarObjects;

protected:
    QWidget *w;
    void *innerPointer;

    int m_titleBarHeight;
    bool m_bWinMoving;
    bool m_bMousePressed;
    QPoint m_MousePos;
    QPoint m_WindowPos;
    bool m_bIsCloseBtnEnabled, m_bIsMinBtnEnabled, m_bIsZoomBtnEnabled;
    bool m_bTitleBarVisible;

    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // MACWINDOWCTL_H
