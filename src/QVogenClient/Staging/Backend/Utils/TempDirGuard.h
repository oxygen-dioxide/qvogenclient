#ifndef TEMPDIRGUARD_H
#define TEMPDIRGUARD_H

#include <QString>

class TempDirGuard {
public:
    TempDirGuard();
    ~TempDirGuard();

public:
    void create();
    void destroy();

    QString path() const;

    bool autoDestroy() const;
    void setAutoDestroy(bool autoDestroy);

protected:
    QString m_path;
    bool m_autoDestroy;
};

#endif // TEMPDIRGUARD_H
