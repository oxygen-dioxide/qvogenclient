#include "CommandPaletteListWidget.h"

#include "QMarginsImpl.h"

#include <private/qlistwidget_p.h>

QCommandPaletteListWidget::QCommandPaletteListWidget(QWidget *parent) : QListWidget(parent) {
    init();
}

QCommandPaletteListWidget::~QCommandPaletteListWidget() {
}

void QCommandPaletteListWidget::init() {
    m_delegate = new QCommandPaletteItemDelegate(this);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    setItemDelegate(m_delegate);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

QTypeList QCommandPaletteListWidget::styleData() const {
    QVariant var0;
    var0.setValue(m_delegate->m_focusType);
    QVariant var1;
    var1.setValue(m_delegate->m_selectType);
    QVariant var2;
    var2.setValue(m_delegate->m_underline);
    QVariant var3;
    var3.setValue(m_delegate->m_fileType);
    QVariant var4;
    var4.setValue(m_delegate->m_locType);
    QVariant var5;
    var5.setValue(m_delegate->m_dateType);
    QVariant var6;
    var6.setValue(m_delegate->m_fileMargins);
    QVariant var7;
    var7.setValue(m_delegate->m_locMargins);
    QVariant var8;
    var8.setValue(m_delegate->m_dateMargins);
    QVariant var9;
    var9.setValue(m_delegate->m_iconMargins);
    return {var0, var1, var2, var3, var4, var5, var6, var7, var8, var9};
}

void QCommandPaletteListWidget::setStyleData(const QTypeList &list) {
    if (list.size() >= 10) {
        QVariant var0 = list.at(0);
        QVariant var1 = list.at(1);
        QVariant var2 = list.at(2);
        QVariant var3 = list.at(3);
        QVariant var4 = list.at(4);
        QVariant var5 = list.at(5);
        QVariant var6 = list.at(6);
        QVariant var7 = list.at(7);
        QVariant var8 = list.at(8);
        QVariant var9 = list.at(9);
        if (var0.convert(qMetaTypeId<QTypeFace>())) {
            m_delegate->m_focusType = var0.value<QTypeFace>();
        }
        if (var1.convert(qMetaTypeId<QTypeFace>())) {
            m_delegate->m_selectType = var1.value<QTypeFace>();
        }
        if (var2.convert(qMetaTypeId<QTypeFace>())) {
            m_delegate->m_underline = var2.value<QTypeFace>();
        }
        if (var3.convert(qMetaTypeId<QTypeFace>())) {
            m_delegate->m_fileType = var3.value<QTypeFace>();
        }
        if (var4.convert(qMetaTypeId<QTypeFace>())) {
            m_delegate->m_locType = var4.value<QTypeFace>();
        }
        if (var5.convert(qMetaTypeId<QTypeFace>())) {
            m_delegate->m_dateType = var5.value<QTypeFace>();
        }
        if (var6.convert(qMetaTypeId<QMargins>())) {
            m_delegate->m_fileMargins = var6.value<QMargins>();
        }
        if (var7.convert(qMetaTypeId<QMargins>())) {
            m_delegate->m_locMargins = var7.value<QMargins>();
        }
        if (var8.convert(qMetaTypeId<QMargins>())) {
            m_delegate->m_dateMargins = var8.value<QMargins>();
        }
        if (var9.convert(qMetaTypeId<QMargins>())) {
            m_delegate->m_iconMargins = var9.value<QMargins>();
        }
    }
    update();
    emit styleDataChanged();
}

QCommandPaletteItemDelegate *QCommandPaletteListWidget::delegate() const {
    return m_delegate;
}

QSize QCommandPaletteListWidget::contentsSize() const {
    auto d = static_cast<QListWidgetPrivate *>(d_ptr.data());
    return d->contentsSize();
}

void QCommandPaletteListWidget::updateGeometries() {
    QListWidget::updateGeometries();

    QSize size = contentsSize();
    if (oldContentsSize != size) {
        oldContentsSize = size;
        emit contentsSizeUpdated(size);
    }
}

void QCommandPaletteListWidget::mouseReleaseEvent(QMouseEvent *event) {
    QListWidget::mouseReleaseEvent(event);
    clearSelection();
}
