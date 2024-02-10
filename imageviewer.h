#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>

class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewer(QWidget *parent = nullptr);

    void renderSvg(QByteArray _data, bool _stretch, bool _trim);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:

private:
    QByteArray data;
    bool stretch;
    bool trim;
    static const bool drawBorder = false;
};

#endif // IMAGEVIEWER_H
