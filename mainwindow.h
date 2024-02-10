#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ImageViewer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_colorPushButton_clicked();

    void on_svgTextEdit_textChanged();

    void on_fitCheckBox_stateChanged(int arg1);

    void on_trimCheckBox_stateChanged(int arg1);

    void on_svgOpenPushButton_clicked();

    void on_svgSavePushButton_clicked();

    void on_pngSavePushButton_clicked();

private:
    Ui::MainWindow *ui;
    ImageViewer *imageViewer;
    QColor selectedColor;
    static const int defaultIdent = 2;

    void setTextWithUndo(QString text);
    QString formatXML(QString text, int ident = defaultIdent);
    void enableUI(bool enable);
};
#endif // MAINWINDOW_H
