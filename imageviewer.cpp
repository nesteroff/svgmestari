#include "imageviewer.h"
#include <QPaintEvent>
#include <QPainter>
#include <QSvgRenderer>

ImageViewer::ImageViewer(QWidget *parent)
    : QWidget{parent}
{}

void ImageViewer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.fillRect(rect(), QColor(255, 255, 255));
    int h = 10;
    bool shifted = false;
    for (int y = 0; y < height(); y += h)
    {
        for (int x = 0; x < this->width(); x += h * 2)
        {
            QRect rect(shifted ? x : x + h, y, h, h);
            painter.fillRect(rect, QColor(190, 190, 190));
        }
        shifted = !shifted;
    }

    QFont imageFont("Segoe UI", 12, QFont::Bold);
    painter.setFont(imageFont);
    painter.setPen(QColor(0, 0, 0));
    if (data.size())
    {
        QSvgRenderer svgRenderer(data);
        svgRenderer.setAspectRatioMode(Qt::KeepAspectRatio);
        if (trim)
            svgRenderer.setViewBox(svgRenderer.boundsOnElement("svg"));
        if (svgRenderer.isValid())
        {
            QRect imageRect = rect();
            if (!stretch)
            {
                auto defaultSize = svgRenderer.defaultSize();
                imageRect = QRect((width() - defaultSize.width()) / 2, (height() - defaultSize.height()) / 2, defaultSize.width(), defaultSize.height());
            }
            svgRenderer.render(&painter, imageRect);

            if (drawBorder)
            {
                QBrush borderBrush(QColor(5, 170, 224));
                int borderWidth = 2;
                painter.fillRect(imageRect.x(), imageRect.y(), imageRect.width(), borderWidth, borderBrush);
                painter.fillRect(imageRect.x(), imageRect.y(), borderWidth, imageRect.height(), borderBrush);
                painter.fillRect(imageRect.right() - borderWidth + 1, imageRect.y(), borderWidth, imageRect.height(), borderBrush);
                painter.fillRect(imageRect.x(), imageRect.bottom() - borderWidth + 1, imageRect.width(), borderWidth, borderBrush);
            }
        }

        QRect sizeRect(2, rect().bottom() - 30, 100, 30);
        auto svgSize = svgRenderer.defaultSize();
        painter.drawText(sizeRect, Qt::TextSingleLine | Qt::AlignBottom | Qt::AlignLeft, QString("%1 x %2").arg(svgSize.width()).arg(svgSize.height()));
    }

    QRect sizeRect(rect().right() - 100, rect().bottom() - 30, 100, 30);
    painter.drawText(sizeRect, Qt::TextSingleLine | Qt::AlignBottom | Qt::AlignRight, QString("%1 x %2").arg(rect().width()).arg(rect().height()));
}

void ImageViewer::renderSvg(QByteArray _data, bool _stretch, bool _trim)
{
    data = _data;
    stretch = _stretch;
    trim = _trim;
    repaint();
}
