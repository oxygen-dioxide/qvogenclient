#include "MathHelper.h"

#include <QDebug>
#include <QSet>
#include <QtMath>

#include <cstdio>

#include <QtGui/private/qcssparser_p.h>

QList<int> Math::toIntList(const QStringList &list) {
    QList<int> res;
    for (auto it = list.begin(); it != list.end(); ++it) {
        bool isNum;
        int num = it->toInt(&isNum);
        if (!isNum) {
            return {};
        }
        res.append(num);
    }
    return res;
}

QList<double> Math::toDoubleList(const QStringList &list) {
    QList<double> res;
    for (auto it = list.begin(); it != list.end(); ++it) {
        bool isNum;
        int num = it->toDouble(&isNum);
        if (!isNum) {
            return {};
        }
        res.append(num);
    }
    return res;
}

bool Math::isNumber(const QString &s, bool considerDot, bool considerNeg) {
    bool flag = true;

    for (int i = 0; i < s.size(); ++i) {
        QChar ch = s.at(i);
        if ((ch >= '0' && ch <= '9') || (considerDot && ch == '.') || (considerNeg && ch == '-')) {
            // is Number
        } else {
            flag = false;
            break;
        }
    }

    return flag;
}

QStringList Math::extractFunctionToStringList(const QString &str, bool *ok) {
    int leftParen = str.indexOf('(');
    int rightParen = str.lastIndexOf(')');
    QStringList res;
    if (leftParen > 0 && rightParen > leftParen) {
        if (ok) {
            *ok = true;
        }
        res = QStringList(
            {str.mid(0, leftParen), str.mid(leftParen + 1, rightParen - leftParen - 1)});
    } else {
        if (ok) {
            *ok = false;
        }
    }
    return res;
}

QColor Math::CssStringToColor(const QString &str) {
    if (str.isEmpty()) {
        return QColor();
    }

    QCss::Declaration dec;
    QCss::Value val;

    bool ok;
    QStringList list = extractFunctionToStringList(str, &ok);
    if (ok) {
        val.type = QCss::Value::Function;
        val.variant = list;
    } else {
        val.type = QCss::Value::String;
        val.variant = str;
    }

    dec.d->values.push_back(val);
    return dec.colorValue();
}

double Math::euclideanDistance(const QPoint &p1, const QPoint &p2) {
    return qSqrt(qPow(p1.x() - p2.x(), 2) + qPow(p1.y() - p2.y(), 2));
}

QStringList Math::splitAll(const QString &str, const QChar &delim) {
    QStringList res;
    QString buf;
    for (int i = 0; i < str.size(); ++i) {
        auto cur = str.at(i);
        if (cur == delim) {
            if (!buf.isEmpty()) {
                res.append(buf);
                buf.clear();
            }
            continue;
        }
        buf += cur;
    }
    if (!buf.isEmpty()) {
        res.append(buf);
    }
    return res;
}

QString Math::adjustRepeatedName(const QSet<QString> &set, const QString &name) {
    QString body;
    int num;

    bool failed = false;

    // Check if there's a formatted suffix
    int index = name.lastIndexOf(" (");
    if (index >= 0) {
        QString suffix = name.mid(index);

        // Get suffix index
        const char fmt[] = " (%d)";
        int n = ::sscanf(suffix.toUtf8().data(), fmt, &num);
        if (n != 1) {
            failed = true;
        } else {
            body = name.left(index);
        }
    } else {
        failed = true;
    }

    if (failed) {
        body = name;
        num = 0;
    }

    QString newName;
    while (true) {
        num++;
        newName = body + QString(" (%1)").arg(QString::number(num));
        if (!set.contains(newName)) {
            break;
        }
    };

    return newName;
}
