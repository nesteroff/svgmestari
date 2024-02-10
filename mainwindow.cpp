#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QSvgRenderer>
#include <QPainter>
#include <QColorDialog>
#include <QDomDocument>
#include <QMessageBox>
#include "imageviewer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{    
    ui->setupUi(this);
    ui->splitter->setSizes({this->height() / 2, this->height() / 2});
    imageViewer = new ImageViewer(this);
    ui->verticalLayout_2->replaceWidget(ui->imageLabel, imageViewer);
    delete ui->imageLabel;
    imageViewer->setMinimumHeight(50);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_colorPushButton_clicked()
{
    QColor color = QColorDialog::getColor(selectedColor, nullptr, tr("select color"), QColorDialog::DontUseNativeDialog);
    if (color.isValid())
    {
        selectedColor = color;

        QDomDocument doc;
        doc.setContent(ui->svgTextEdit->toPlainText());
        QDomElement root = doc.documentElement();
        QDomNodeList elements = root.elementsByTagName("path");
        for (int i = 0; i < elements.count(); ++i)
        {
            QDomElement element = elements.at(i).toElement();
            element.setAttribute("fill", color.name());
        }
        setTextWithUndo(doc.toString(defaultIdent));
    }
}

void MainWindow::on_svgTextEdit_textChanged()
{
    imageViewer->renderSvg(ui->svgTextEdit->toPlainText().toUtf8(), ui->fitCheckBox->isChecked(), ui->trimCheckBox->isChecked());
}

void MainWindow::on_fitCheckBox_stateChanged(int arg1)
{
    imageViewer->renderSvg(ui->svgTextEdit->toPlainText().toUtf8(), ui->fitCheckBox->isChecked(), ui->trimCheckBox->isChecked());
}

void MainWindow::on_trimCheckBox_stateChanged(int arg1)
{
    imageViewer->renderSvg(ui->svgTextEdit->toPlainText().toUtf8(), ui->fitCheckBox->isChecked(), ui->trimCheckBox->isChecked());
}

void MainWindow::setTextWithUndo(QString text)
{
    QTextDocument *doc = ui->svgTextEdit->document();
    QTextCursor curs(doc);
    curs.select(QTextCursor::Document);
    curs.insertText(text);
}

QString MainWindow::formatXML(QString text, int ident)
{
    QDomDocument doc;
    doc.setContent(text);
    return doc.toString(ident);
}

void MainWindow::enableUI(bool enable)
{
    ui->fitCheckBox->setEnabled(enable);
    ui->colorPushButton->setEnabled(enable);
    ui->svgSavePushButton->setEnabled(enable);
    ui->pngLineEdit->setEnabled(enable);
    ui->pngSavePushButton->setEnabled(enable);
    ui->pngWidthSpinBox->setEnabled(enable);
    ui->pngHeightSpinBox->setEnabled(enable);
}

void MainWindow::on_svgOpenPushButton_clicked()
{
    auto initialPath = ui->svgLineEdit->text();
    if (initialPath.isEmpty())
        initialPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    auto svgPath = QFileDialog::getOpenFileName(this, tr("Open SVG"), initialPath, tr("SVG Files (*.svg)"));
    if (svgPath.isEmpty())
        return;

    ui->svgLineEdit->setText(QDir::toNativeSeparators(svgPath));
    QFileInfo fileInfo(svgPath);
    QString pngPath = fileInfo.path() + "/" + fileInfo.baseName () + ".png";
    ui->pngLineEdit->setText(QDir::toNativeSeparators(pngPath));

    QFile file(svgPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Failed to open"), file.errorString());
        return;
    }

    QByteArray svgContent = file.readAll();
    QSvgRenderer svgRenderer(svgContent);
    if (!svgRenderer.isValid())
    {
        QMessageBox::critical(this, tr("Failed to load"), tr("Invalid SVG"));
        return;
    }

    ui->svgTextEdit->setPlainText(formatXML(svgContent));
}

void MainWindow::on_svgSavePushButton_clicked()
{
    auto initialPath = ui->svgLineEdit->text();
    if (initialPath.isEmpty())
        initialPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    auto svgPath = QFileDialog::getSaveFileName(this, tr("Save SVG"), initialPath, tr("SVG Files (*.svg)"));
    if (svgPath.isEmpty())
        return;

    QFile file(svgPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Failed to open"), file.errorString());
        return;
    }

    auto svgData = formatXML(ui->svgTextEdit->toPlainText(), -1);
    if (file.write(svgData.toUtf8()) == -1)
    {
        QMessageBox::critical(this, tr("Failed to write"), file.errorString());
        return;
    }

    ui->svgLineEdit->setText(QDir::toNativeSeparators(svgPath));
}

void MainWindow::on_pngSavePushButton_clicked()
{
    auto initialPath = ui->pngLineEdit->text();
    if (initialPath.isEmpty())
        initialPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    auto pngPath = QFileDialog::getSaveFileName(this, tr("Export to PNG"), initialPath, tr("PNG Files (*.png)"));
    if (pngPath.isEmpty())
        return;

    auto svgData = ui->svgTextEdit->toPlainText().toUtf8();
    QSvgRenderer svgRenderer(svgData);
    if (!svgRenderer.isValid())
    {
        QMessageBox::critical(this, tr("Failed to save"), tr("Invalid SVG"));
        return;
    }

    QSize pngSize(ui->pngWidthSpinBox->value(), ui->pngHeightSpinBox->value());
    QImage image(pngSize, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    svgRenderer.setAspectRatioMode(Qt::KeepAspectRatio);
    if (ui->trimCheckBox->isChecked())
        svgRenderer.setViewBox(svgRenderer.boundsOnElement("svg"));

    svgRenderer.render(&painter);
    if (!image.save(pngPath))
        QMessageBox::critical(this, tr("Failed to save"), tr("Invalid SVG"));
    else
        ui->pngLineEdit->setText(QDir::toNativeSeparators(pngPath));
}


