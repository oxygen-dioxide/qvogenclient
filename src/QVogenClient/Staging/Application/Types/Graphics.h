#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QGraphicsItem>

namespace GraphicsImpl {

    enum ItemTypes {
        NoteItem = QGraphicsItem::UserType + 1,
        GroupHintItem,
    };

};

#endif // GRAPHICS_H
