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

    bool parseFile(const QString &filename, CommonScore &proj);

protected:
    bool parseMidiFile(const QString &filename, CommonScore &proj);        // *.mid
    bool parseUTAUFile(const QString &filename, CommonScore &proj);        // *.ust
                                                                            //
    bool parseSynthVFile(const QString &filename, CommonScore &proj);      // *.svp
    bool parseSynthVOldFile(const QString &filename, CommonScore &proj);   // *.s5p
                                                                            //
    bool parseVocaloidFile(const QString &filename, CommonScore &proj);    // *.vsqx
    bool parseVocaloidOldFile(const QString &filename, CommonScore &proj); // *.vsq
                                                                            //
    bool parseOpenUTAUFile(const QString &filename, CommonScore &proj);    // *.ustx

public:
    static bool isSuffixParsable(const QString &suffix);

signals:
};

#endif // FILEPARSER_H
