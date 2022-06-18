#include "CMenu.h"
#include "CApplication.h"

#include <private/qkeysequence_p.h>
#include <private/qmenu_p.h>

#include <QDebug>
#include <QFontMetrics>

CMenu::CMenu(QWidget *parent) : QMenu(parent) {
    // Initialize Font
#ifdef QS_NO_TAB_FOCUS
    QS_REMOVE_TAB_FOCUS(this)
#endif
    setFont(qApp->font());

    m_subIconMargins = QSize(0, 0);
}

CMenu::CMenu(const QString &title, QWidget *parent) : CMenu(parent) {
    auto d = reinterpret_cast<QMenuPrivate *>(d_ptr.data());
    d->menuAction->setText(title);
}

CMenu::~CMenu() {
}

QSvgUri CMenu::subIcon() const {
    return m_subIcon;
}

void CMenu::setSubIcon(const QSvgUri &icon) {
    m_subIcon = icon;
    update();
    emit styleChanged();
}

QSvgUri CMenu::subIconActive() const {
    return m_subIconActive;
}

void CMenu::setSubIconActive(const QSvgUri &icon) {
    m_subIconActive = icon;
    update();
    emit styleChanged();
}

QSvgUri CMenu::subIconDisabled() const {
    return m_subIconDisabled;
}

void CMenu::setSubIconDisabled(const QSvgUri &subIconDisabled) {
    m_subIconDisabled = subIconDisabled;
}

QSize CMenu::subIconMargins() const {
    return m_subIconMargins;
}

void CMenu::setSubIconMargins(const QSize &margins) {
    m_subIconMargins = margins;
    update();
    emit styleChanged();
}

void CMenu::paintEvent(QPaintEvent *event) {
    auto d = reinterpret_cast<QMenuPrivate *>(d_ptr.data());

    d->updateActionRects();
    QPainter p(this);
    QRegion emptyArea = QRegion(rect());

    QStyleOptionMenuItem menuOpt;
    menuOpt.initFrom(this);
    menuOpt.state = QStyle::State_None;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.maxIconWidth = 0;
    menuOpt.tabWidth = 0;

    style()->drawPrimitive(QStyle::PE_PanelMenu, &menuOpt, &p, this);

    // calculate the scroll up / down rect
    const int fw = style()->pixelMetric(QStyle::PM_MenuPanelWidth, nullptr, this);
    const int hmargin = style()->pixelMetric(QStyle::PM_MenuHMargin, nullptr, this);
    const int vmargin = style()->pixelMetric(QStyle::PM_MenuVMargin, nullptr, this);

    QRect scrollUpRect, scrollDownRect;
    const int leftmargin = fw + hmargin + d->leftmargin;
    const int topmargin = fw + vmargin + d->topmargin;
    const int bottommargin = fw + vmargin + d->bottommargin;
    const int contentWidth = width() - (fw + hmargin) * 2 - d->leftmargin - d->rightmargin;
    if (d->scroll) {
        if (d->scroll->scrollFlags & QMenuPrivate::QMenuScroller::ScrollUp)
            scrollUpRect.setRect(leftmargin, topmargin, contentWidth, d->scrollerHeight());

        if (d->scroll->scrollFlags & QMenuPrivate::QMenuScroller::ScrollDown)
            scrollDownRect.setRect(leftmargin, height() - d->scrollerHeight() - bottommargin,
                                   contentWidth, d->scrollerHeight());
    }

    // calculate the tear off rect
    QRect tearOffRect;
    if (d->tearoff) {
        tearOffRect.setRect(leftmargin, topmargin, contentWidth,
                            style()->pixelMetric(QStyle::PM_MenuTearoffHeight, nullptr, this));
        if (d->scroll && d->scroll->scrollFlags & QMenuPrivate::QMenuScroller::ScrollUp)
            tearOffRect.translate(0, d->scrollerHeight());
    }

    // draw the items that need updating..
    QRect scrollUpTearOffRect = scrollUpRect.united(tearOffRect);
    for (int i = 0; i < d->actions.count(); ++i) {
        QAction *action = d->actions.at(i);
        QRect actionRect = d->actionRects.at(i);
        if (!event->rect().intersects(actionRect) || d->widgetItems.value(action))
            continue;
        // set the clip region to be extra safe (and adjust for the scrollers)
        emptyArea -= QRegion(actionRect);

        QRect adjustedActionRect = actionRect;
        if (!scrollUpTearOffRect.isEmpty() &&
            adjustedActionRect.bottom() <= scrollUpTearOffRect.top())
            continue;

        if (!scrollDownRect.isEmpty() && adjustedActionRect.top() >= scrollDownRect.bottom())
            continue;

        if (adjustedActionRect.intersects(scrollUpTearOffRect)) {
            if (adjustedActionRect.bottom() <= scrollUpTearOffRect.bottom())
                continue;
            else
                adjustedActionRect.setTop(scrollUpTearOffRect.bottom() + 1);
        }

        if (adjustedActionRect.intersects(scrollDownRect)) {
            if (adjustedActionRect.top() >= scrollDownRect.top())
                continue;
            else
                adjustedActionRect.setBottom(scrollDownRect.top() - 1);
        }

        QRegion adjustedActionReg(adjustedActionRect);
        p.setClipRegion(adjustedActionReg);

        QStyleOptionMenuItem opt;
        initStyleOption(&opt, action);
        opt.rect = actionRect;
        style()->drawControl(QStyle::CE_MenuItem, &opt, &p, this);

        // p.setPen(Qt::red);
        // p.setBrush(Qt::transparent);
        // p.drawRect(opt.rect);

        // Draw Right Arrow
        if (action->menu()) {
            QIcon subIcon = (opt.state & QStyle::State_Enabled)
                                ? ((opt.state & QStyle::State_Selected) ? m_subIconActive.toIcon()
                                                                        : m_subIcon.toIcon())
                                : m_subIconDisabled.toIcon();
            if (!subIcon.isNull()) {
                int a = actionRect.height();
                QRect iconRegion(actionRect.right() - a, actionRect.top(), a, a);
                QRect iconRect =
                    iconRegion.adjusted(m_subIconMargins.width(), m_subIconMargins.height(),
                                        -m_subIconMargins.width(), -m_subIconMargins.height());
                p.drawPixmap(iconRect, subIcon.pixmap(iconRect.size()));
            }
        }
    }

    emptyArea -= QRegion(scrollUpTearOffRect);
    emptyArea -= QRegion(scrollDownRect);

    if (d->scrollUpTearOffItem || d->scrollDownItem) {
        if (d->scrollUpTearOffItem)
            d->scrollUpTearOffItem->updateScrollerRects(scrollUpTearOffRect);
        if (d->scrollDownItem)
            d->scrollDownItem->updateScrollerRects(scrollDownRect);
    } else {
        // paint scroll up /down
        d->drawScroller(&p, QMenuPrivate::ScrollerTearOffItem::ScrollUp, scrollUpRect);
        d->drawScroller(&p, QMenuPrivate::ScrollerTearOffItem::ScrollDown, scrollDownRect);
        // paint the tear off..
        d->drawTearOff(&p, tearOffRect);
    }

    // draw border
    if (fw) {
        QRegion borderReg;
        borderReg += QRect(0, 0, fw, height());            // left
        borderReg += QRect(width() - fw, 0, fw, height()); // right
        borderReg += QRect(0, 0, width(), fw);             // top
        borderReg += QRect(0, height() - fw, width(), fw); // bottom
        p.setClipRegion(borderReg);
        emptyArea -= borderReg;
        QStyleOptionFrame frame;
        frame.rect = rect();
        frame.palette = palette();
        frame.state = QStyle::State_None;
        frame.lineWidth = style()->pixelMetric(QStyle::PM_MenuPanelWidth, &frame);
        frame.midLineWidth = 0;
        style()->drawPrimitive(QStyle::PE_FrameMenu, &frame, &p, this);
    }

    // finally the rest of the spaces
    p.setClipRegion(emptyArea);
    menuOpt.state = QStyle::State_None;
    menuOpt.menuItemType = QStyleOptionMenuItem::EmptyArea;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.rect = rect();
    menuOpt.menuRect = rect();

    style()->drawControl(QStyle::CE_MenuEmptyArea, &menuOpt, &p, this);
}

void CMenu::initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const {
    if (!option || !action)
        return;

    bool mouseDown;
    // Avoid using QMenuPrivate::mouseDown
    {
        QStyleOptionMenuItem testOption;
        QMenu::initStyleOption(&testOption, action);
        mouseDown = testOption.state & QStyle::State_Sunken;
    }

    auto d = reinterpret_cast<QMenuPrivate *>(d_ptr.data());

    option->initFrom(this);
    option->palette = palette();
    option->state = QStyle::State_None;

    if (window()->isActiveWindow())
        option->state |= QStyle::State_Active;
    if (isEnabled() && action->isEnabled() && (!action->menu() || action->menu()->isEnabled()))
        option->state |= QStyle::State_Enabled;
    else
        option->palette.setCurrentColorGroup(QPalette::Disabled);

    option->font = action->font().resolve(font());
    option->fontMetrics = QFontMetrics(option->font);

    if (d->currentAction && d->currentAction == action && !d->currentAction->isSeparator()) {
        option->state |=
            QStyle::State_Selected | (mouseDown ? QStyle::State_Sunken : QStyle::State_None);
    }

    option->menuHasCheckableItems = d->hasCheckableItems;
    if (!action->isCheckable()) {
        option->checkType = QStyleOptionMenuItem::NotCheckable;
    } else {
        option->checkType = (action->actionGroup() && action->actionGroup()->isExclusive())
                                ? QStyleOptionMenuItem::Exclusive
                                : QStyleOptionMenuItem::NonExclusive;
        option->checked = action->isChecked();
    }
    if (action->menu()) {
        // option->menuItemType = QStyleOptionMenuItem::SubMenu; Ignore Right Arrow
        option->menuItemType = QStyleOptionMenuItem::Normal;
    } else if (action->isSeparator()) {
        option->menuItemType = QStyleOptionMenuItem::Separator;
    } else if (d->defaultAction == action) {
        option->menuItemType = QStyleOptionMenuItem::DefaultItem;
    } else {
        option->menuItemType = QStyleOptionMenuItem::Normal;
    }
    if (action->isIconVisibleInMenu()) {
        option->icon = action->icon();
    }

    QString textAndAccel = action->text();
    int tabWidth = d->tabWidth;

#ifndef QT_NO_SHORTCUT
    if ((action->isShortcutVisibleInContextMenu() || !d->isContextMenu()) &&
        textAndAccel.indexOf(QLatin1Char('\t')) == -1) {
        QKeySequence seq = action->shortcut();
        if (!seq.isEmpty()) {
            QString seqText = seq.toString(QKeySequence::NativeText);
            seqText.replace(", ", " ");
            textAndAccel += QLatin1Char('\t') + seqText;
            tabWidth = QFontMetrics(font()).horizontalAdvance(seqText);
        }
    }
#endif
    option->text = textAndAccel;
    option->tabWidth = tabWidth;
    option->maxIconWidth = d->maxIconWidth;
    option->menuRect = rect();
}
