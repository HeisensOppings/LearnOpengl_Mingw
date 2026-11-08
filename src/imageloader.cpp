#include "imageloader.h"

#include <QImage>
#include <QString>
#include <cstring>

bool LoadImageFile(const std::string &path, ImageData &out, bool flipVertically)
{
    out = ImageData();
    QImage image(QString::fromStdString(path));
    if (image.isNull())
        return false;

    QImage converted;
    if (image.isGrayscale() && !image.hasAlphaChannel())
    {
        converted = image.convertToFormat(QImage::Format_Grayscale8);
        out.channels = 1;
    }
    else if (image.hasAlphaChannel())
    {
        converted = image.convertToFormat(QImage::Format_RGBA8888);
        out.channels = 4;
    }
    else
    {
        converted = image.convertToFormat(QImage::Format_RGB888);
        out.channels = 3;
    }
    if (flipVertically)
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
        converted = converted.flipped(Qt::Vertical);
#else
        converted = converted.mirrored(false, true);
#endif
    }

    out.width = converted.width();
    out.height = converted.height();
    const size_t rowBytes = static_cast<size_t>(out.width) * out.channels;
    out.pixels.resize(rowBytes * out.height);
    // QImage scanlines are 32-bit aligned; copy row by row into a tightly packed buffer
    for (int y = 0; y < out.height; ++y)
        std::memcpy(out.pixels.data() + rowBytes * y, converted.constScanLine(y), rowBytes);
    return true;
}
