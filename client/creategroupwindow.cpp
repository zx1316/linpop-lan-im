#include "creategroupwindow.h"
#include "ui_creategroupwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>

CreateGroupWindow::CreateGroupWindow(const QList<QString> &friendList, const QString &selfName, MiHoYoLauncher *launcher, QWidget *parent) : QWidget(parent), ui(new Ui::CreateGroupWindow), launcher(launcher), selfName(selfName) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->unselectedList->addItems(friendList);

    //连接按钮
    connect(ui->create_group_button, &QPushButton::clicked, this, &CreateGroupWindow::onCreateGroupButtonClicked);
    connect(ui->selectImgButton, SIGNAL(clicked()), this, SLOT(onSelectImgButtonClicked()));
    connect(ui->addButton, &QPushButton::clicked, this, &CreateGroupWindow::onAddButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked, this, &CreateGroupWindow::onRemoveButtonClicked);
}

void CreateGroupWindow::onCreateGroupSuccessSignal() {
    QMessageBox::information(this,"创建成功","创建聊天室成功");
}

void CreateGroupWindow::onCreateGroupFailSignal() {
    QMessageBox::critical(this,"创建失败","该聊天室已存在");
}

void CreateGroupWindow::onAddButtonClicked() {
    launcher->gachaLaunch();
    auto current = ui->unselectedList->currentRow();
    if (current >= 0) {
        auto item = ui->unselectedList->takeItem(current);
        ui->selectedList->addItem(item);
    }
}

void CreateGroupWindow::onRemoveButtonClicked() {
    launcher->gachaLaunch();
    auto current = ui->selectedList->currentRow();
    if (current > 0) {
        auto item = ui->selectedList->takeItem(current);
        ui->unselectedList->addItem(item);
    }
}

void CreateGroupWindow::onCreateGroupButtonClicked() {
    launcher->gachaLaunch();
    QString group_name = ui->group_name->text();
    int spaceCnt = 0;
    for (auto ch : group_name) {
        if (ch.isSpace()) {
            spaceCnt++;
        }
    }
    if (group_name.startsWith('_')) {
        QMessageBox::critical(this, "创建失败", "聊天室名称不能以下划线开头");
    } else if (group_name.toUtf8().size() > 30) {
        QMessageBox::critical(this, "创建失败", "聊天室名称过长");
    } else if (group_name.isEmpty()) {
        QMessageBox::critical(this, "创建失败", "聊天室名称不能为空");
    }  else if (spaceCnt == group_name.length()) {
        QMessageBox::critical(this, "创建失败", "聊天室名称不能全为空格");
    } else if (ui->imageLabel->pixmap() == nullptr) {
        QMessageBox::critical(this, "创建失败", "请设置头像");
    } else {
        QList<QString> list1;
        list1.append(selfName);
        auto len = ui->selectedList->count();
        for (auto i = 0; i < len; i++) {
            list1.append(ui->selectedList->item(i)->text());
        }
        auto image = ui->imageLabel->pixmap()->toImage();
        QByteArray array;
        QBuffer buffer(&array);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG");
        QString imgName = QCryptographicHash::hash(array, QCryptographicHash::Md5).toHex() + ".png";
        QFile file(QCoreApplication::applicationDirPath() + "/cached_images/" + imgName);
        if (!file.exists()) {
            image.save(QCoreApplication::applicationDirPath() + "/cached_images/" + imgName, "PNG");
        }
        emit createGroupRequestSignal("_" + group_name, imgName, list1);
    }
}

void CreateGroupWindow::onSelectImgButtonClicked() {
    launcher->gachaLaunch();
    auto path = QFileDialog::getOpenFileName(this, "选择一张png图片", "../", "Images (*.png)");
    if (path != "") {
        QImage originalImage(path);
        // 确定裁剪区域以获取正方形部分
        int size = qMin(originalImage.width(), originalImage.height());
        QRect squareRect((originalImage.width() - size) / 2, (originalImage.height() - size) / 2, size, size);
        QImage squareImage = originalImage.copy(squareRect);
        // 将裁剪后的图像调整为128x128像素
        QSize newSize(128, 128);
        squareImage = squareImage.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->imageLabel->setPixmap(QPixmap::fromImage(squareImage));
    }
}

CreateGroupWindow::~CreateGroupWindow() {
    emit windowClosed();
    delete ui;
}
