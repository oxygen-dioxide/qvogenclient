#ifndef TBASEOPERATION_H
#define TBASEOPERATION_H

#include <QString>

class TBaseOperation {
public:
    TBaseOperation(int type);
    virtual ~TBaseOperation();

    enum OperateType {
        NoteMove,
        NoteStretch,
    };

public:
    int type() const;

    virtual bool differ() const;

protected:
    int t;
};

#endif // TBASEOPERATION_H
