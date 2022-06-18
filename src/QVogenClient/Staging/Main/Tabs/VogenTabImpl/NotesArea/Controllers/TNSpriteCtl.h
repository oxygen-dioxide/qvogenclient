#ifndef TNSPRITECTL_H
#define TNSPRITECTL_H

#include "../Elements/TNEdgeSprite.h"
#include "TNController.h"

class TNSpriteCtl : public TNController {
    Q_OBJECT
public:
    explicit TNSpriteCtl(TNotesArea *parent = nullptr);
    ~TNSpriteCtl();

public:
    void install() override;

    bool spriteVisible() const;
    void setSpriteVisible(bool spriteVisible);

    Qt::Corner spritePosition() const;
    void setSpritePosition(const Qt::Corner &spritePosition);

    double spriteAlpha() const;
    void setSpriteAlpha(double spriteAlpha);

    void loadSprite(const QString &path);

protected:
    TNEdgeSprite *m_sprite;

    bool m_spriteVisible;
    Qt::Corner m_spritePosition;

    void updateSprite();

    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
};

#endif // TNSPRITECTL_H
