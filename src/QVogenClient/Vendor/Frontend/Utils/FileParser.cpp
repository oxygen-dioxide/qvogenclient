#include "FileParser.h"
#include "SystemHelper.h"

#include "CommonScore.h"

const char mid_suffix[] = "mid";
const char ust_suffix[] = "ust";
const char svp_suffix[] = "svp";
const char s5p_suffix[] = "s5p";
const char vsqx_suffix[] = "vsqx";
const char vsq_suffix[] = "vsq";
const char ustx_suffix[] = "ustx";

FileParser::FileParser(QWidget *parent) : QObject(parent) {
}

FileParser::~FileParser() {
}

bool FileParser::parseFile(const QString &filename, CommonScore &notes) {
    QString suffix = Sys::PathFindSuffix(filename).toLower();
    CommonScore temp;
    if (suffix == mid_suffix) {
        if (parseMidiFile(filename, temp)) {
            notes = temp;
            return true;
        }
    } else if (suffix == ust_suffix) {
        if (parseUTAUFile(filename, temp)) {
            notes = temp;
            return true;
        }
    } else if (suffix == svp_suffix) {
        if (parseSynthVFile(filename, temp)) {
            notes = temp;
            return true;
        }
    } else if (suffix == s5p_suffix) {
        if (parseSynthVOldFile(filename, temp)) {
            notes = temp;
            return true;
        }
    } else if (suffix == vsqx_suffix) {
        if (parseVocaloidFile(filename, temp)) {
            notes = temp;
            return true;
        }
    } else if (suffix == vsq_suffix) {
        if (parseVocaloidOldFile(filename, temp)) {
            notes = temp;
            return true;
        }
    } else if (suffix == ustx_suffix) {
        if (parseOpenUTAUFile(filename, temp)) {
            notes = temp;
            return true;
        }
    }
    return false;
}

bool FileParser::isSuffixParsable(const QString &suffix) {
    QStringList formats{mid_suffix,  ust_suffix, svp_suffix, s5p_suffix,
                        vsqx_suffix, vsq_suffix, ustx_suffix};
    return formats.contains(suffix.toLower());
}
