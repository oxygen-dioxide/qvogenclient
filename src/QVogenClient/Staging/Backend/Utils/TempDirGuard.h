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

protected:
    QString m_path;
};

#endif // TEMPDIRGUARD_H
