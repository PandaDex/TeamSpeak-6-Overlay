#ifndef OVERLAYPOSITION_H
#define OVERLAYPOSITION_H

#include <QString>
#include <Qt>

namespace OverlayPosition {
    enum Preset {
        TopLeft = 0,
        TopRight = 1,
        BottomLeft = 2,
        BottomRight = 3,
        CenterLeft = 4,
        CenterRight = 5,
        Custom = 6
    };

    inline int fromString(const QString &value)
    {
        bool ok = false;
        int v = value.toInt(&ok);
        if (!ok || v < TopLeft || v > Custom) return TopLeft;
        return v;
    }

    inline bool isRight(int position)
    {
        return position == TopRight || position == BottomRight || position == CenterRight;
    }

    inline bool isBottom(int position)
    {
        return position == BottomLeft || position == BottomRight;
    }

    inline bool isVerticalCenter(int position)
    {
        return position == CenterLeft || position == CenterRight;
    }

    inline Qt::Alignment layoutAlignment(int position)
    {
        Qt::Alignment vertical = Qt::AlignTop;
        if (isBottom(position)) vertical = Qt::AlignBottom;
        else if (isVerticalCenter(position)) vertical = Qt::AlignVCenter;

        return isRight(position) ? (vertical | Qt::AlignRight) : vertical;
    }
}

namespace DisplayMode {
    enum Mode {
        SpeakingOnly = 0,
        WholeChannel = 1
    };

    inline int fromString(const QString &value)
    {
        return value.toInt() == WholeChannel ? WholeChannel : SpeakingOnly;
    }
}

#endif // OVERLAYPOSITION_H
