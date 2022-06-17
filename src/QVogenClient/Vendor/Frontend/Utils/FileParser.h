#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <QMessageBox>
#include <QWidget>

class CommonScore;

class FileParser : public QObject {
    Q_OBJECT
public:
    explicit FileParser(QWidget *parent = nullptr);
    ~FileParser();

    bool parseFile(const QString &filename, CommonScore &notes);

protected:
    bool parseMidiFile(const QString &filename, CommonScore &notes);        // *.mid
    bool parseUTAUFile(const QString &filename, CommonScore &notes);        // *.ust
                                                                            //
    bool parseSynthVFile(const QString &filename, CommonScore &notes);      // *.svp
    bool parseSynthVOldFile(const QString &filename, CommonScore &notes);   // *.s5p
                                                                            //
    bool parseVocaloidFile(const QString &filename, CommonScore &notes);    // *.vsqx
    bool parseVocaloidOldFile(const QString &filename, CommonScore &notes); // *.vsq
                                                                            //
    bool parseOpenUTAUFile(const QString &filename, CommonScore &notes);    // *.ustx

public:
    static bool isSuffixParsable(const QString &suffix);

signals:
};

#endif // FILEPARSER_H
