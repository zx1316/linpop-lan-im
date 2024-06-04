#include <QColorDialog>
#include "fontselectorwindow.h"
#include "ui_fontselectorwindow.h"

FontSelectorWindow::FontSelectorWindow(const QFont& font, const QColor& color, QWidget *parent) : QWidget(parent), ui(new Ui::FontSelectorWindow), color(color) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->colorLabel->setStyleSheet(QString("QLabel{background:rgb(%1, %2, %3);}").arg(color.red()).arg(color.green()).arg(color.blue()));
    ui->sizeComboBox->addItems({"6", "7", "8", "9", "10", "11", "12", "14", "16", "18", "20", "22", "24", "26", "28", "36", "48", "72"});
    ui->sizeComboBox->setCurrentText(QString::number(font.pointSize()));
    ui->fontComboBox->setCurrentText(font.family());
    ui->boldButton->setChecked(font.bold());
    ui->italicButton->setChecked(font.italic());
    ui->underlineButton->setChecked(font.underline());
    connect(ui->colorButton, &QPushButton::clicked, this, &FontSelectorWindow::onColorButtonClicked);
    connect(ui->okButton, &QPushButton::clicked, this, &FontSelectorWindow::onOkButtonClicked);
}

FontSelectorWindow::~FontSelectorWindow() {
    emit windowClosed();
    delete ui;
}

void FontSelectorWindow::onColorButtonClicked() {
    QColorDialog dialog(Qt::black, this);
    dialog.exec();
    QColor color1 = dialog.currentColor();
    if (color1.isValid()) {
        color = color1;
        ui->colorLabel->setStyleSheet(QString("QLabel{background:rgb(%1, %2, %3);}").arg(color1.red()).arg(color1.green()).arg(color1.blue()));
    }
}

void FontSelectorWindow::onOkButtonClicked() {
    QFont font;
    font.setFamily(ui->fontComboBox->currentText());
    font.setPointSize(ui->sizeComboBox->currentText().toInt());
    font.setBold(ui->boldButton->isChecked());
    font.setItalic(ui->italicButton->isChecked());
    font.setUnderline(ui->underlineButton->isChecked());
    emit updateFont(font, color);
    close();
}
