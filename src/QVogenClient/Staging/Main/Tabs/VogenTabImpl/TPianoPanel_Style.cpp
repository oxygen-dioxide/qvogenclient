#include "TPianoPanel.h"

QTypeList TPianoPanel::styleDataN() const {
    const auto &data = m_notesArea->styleData();
    QVariant colors;
    colors.setValue(QColorList({data.backDark, data.backLight, data.levelLine, data.pitchLine,
                                data.sectionLine, data.quarterLine, data.timeLine}));
    return {data.lineWidth, colors};
}

void TPianoPanel::setStyleDataN(const QTypeList &list) {
    TNotesArea::StyleData data;
    if (list.size() == 2) {
        QVariant var0 = list.front();
        QVariant var1 = list.back();
        if (var0.type() == QVariant::Double) {
            data.lineWidth = var0.toDouble();
        }
        if (var1.convert(qMetaTypeId<QColorList>())) {
            QColorList colors = var1.value<QColorList>();
            if (colors.size() >= 7) {
                data.backDark = colors.at(0);
                data.backLight = colors.at(1);
                data.levelLine = colors.at(2);
                data.pitchLine = colors.at(3);
                data.sectionLine = colors.at(4);
                data.quarterLine = colors.at(5);
                data.timeLine = colors.at(6);
            }
        }
    }
    m_notesArea->setStyleData(data);
    emit styleDataChanged();
}

QTypeList TPianoPanel::styleDataS() const {
    const auto &data = m_sectionsArea->styleData();
    QVariant colors;
    colors.setValue(QColorList({data.line, data.number, data.beat, data.globalTempo, data.tempo}));
    return {data.lineWidth, data.shortRatio, data.numberRatio, colors};
}

void TPianoPanel::setStyleDataS(const QTypeList &list) {
    TSectionsArea::Data data;
    if (list.size() == 4) {
        QVariant var0 = list.at(0);
        QVariant var1 = list.at(1);
        QVariant var2 = list.at(2);
        QVariant var3 = list.at(3);
        if (var0.type() == QVariant::Double) {
            data.lineWidth = var0.toDouble();
        }
        if (var1.type() == QVariant::Double) {
            data.shortRatio = var1.toDouble();
        }
        if (var2.type() == QVariant::Double) {
            data.numberRatio = var2.toDouble();
        }
        if (var3.convert(qMetaTypeId<QColorList>())) {
            QColorList colors = var3.value<QColorList>();
            if (colors.size() >= 5) {
                data.line = colors.at(0);
                data.number = colors.at(1);
                data.beat = colors.at(2);
                data.globalTempo = colors.at(3);
                data.tempo = colors.at(4);
            }
        }
    }
    m_sectionsArea->setStyleData(data);
    emit styleDataChanged();
}
