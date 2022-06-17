#ifndef SEQUENCETEXTFILE_H
#define SEQUENCETEXTFILE_H

#include <QFile>
#include <QTextCodec>

#include <QLinkNote.h>
#include <SectionSettings.h>
#include <SectionVersion.h>
#include <UtaProjectText.h>

using SectionNotes = QList<QLinkNote>;

class SequenceTextFile {
public:
    SequenceTextFile();
    ~SequenceTextFile();

    SectionVersion sectionVersion;
    SectionSettings sectionSettings;
    QList<QLinkNote> sectionNotes;

public:
    bool load(const QString &filename);
    bool save(const QString &filename);

    void reset();

protected:
    bool parseSectionName(const QString &str, QString &name);
    bool parseSectionNote(const QStringList &sectionList, QLinkNote &note);
    bool parseSectionVersion(const QStringList &sectionList, SectionVersion &version);
    bool parseSectionSettings(const QStringList &sectionList, SectionSettings &setting);

    void writeSectionName(const int &name, QTextStream &out);
    void writeSectionName(const QString &name, QTextStream &out);

    void writeSectionNote(int num, const QLinkNote &note, QTextStream &out);
    void writeSectionVersion(QTextStream &out);
    void writeSectionSettings(QTextStream &out);
};

#endif // SEQUENCETEXTFILE_H
