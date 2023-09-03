#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>

/**
 * @author 朱炫曦
 * @date 2023/09/02
 * @brief 扫描电脑特定文件夹和文件以快速获得原神或崩铁本体的路径，并立刻启动，如果本程序运行时获得了管理员权限，则启动时Windows不会有弹窗提示。如果两者都没安装，则自动打开浏览器下载原神启动器。
 * @param parent QWidget parent.
 */
void autoStart(QWidget *parent) {
    QString userPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QDir qDir(userPath);
    QString giPath = "";
    QString srPath = "";
    if (qDir.cd("AppData/LocalLow/miHoYo/原神")) {
        QByteArray array;
/*      QFile qFile(userPath + "/AppData/LocalLow/miHoYo/原神/UidInfo.txt");   // 这里可以获取uid
        qFile.open(QIODevice::ReadOnly);
        array = qFile.readLine();
        qFile.close();
        QString uid(array);*/
        QFile qFile1(userPath + "/AppData/LocalLow/miHoYo/原神/output_log.txt");
        qFile1.open(QIODevice::ReadOnly);
        while ((array = qFile1.readLine()).length() != 0) {
            QString line(array);
//          读到类似"[2023-08-27 18:02:48.611] Warmup file D:/Genshin Impact/Genshin Impact Game/YuanShen_Data/StreamingAssets\AssetBundles/blocks/00/00867649.blk 2359296";
//          感谢sunxiaomeng提供的文件夹
            if (line.contains("Genshin Impact Game/YuanShen_Data/StreamingAssets")) {
                int start = line.indexOf("Warmup file") + 12;
                int end = line.indexOf("YuanShen_Data");
                line.truncate(end);
                line.remove(0, start);
                line.append("YuanShen.exe");
                qDebug() << line;
                giPath = line;
                break;
            }
        }
    }
    if (qDir.cd("AppData/LocalLow/miHoYo/崩坏：星穹铁道")) {
        QFile qFile(userPath + "/AppData/LocalLow/miHoYo/崩坏：星穹铁道/Player.log");
        qFile.open(QIODevice::ReadOnly);
        QByteArray array;
        while ((array = qFile.readLine()).length() != 0) {
            QString line(array);
            if (line.contains("Loading player data from")) {
                int start = line.indexOf("Loading player data from") + 25;
                int end = line.indexOf("StarRail_Data");
                line.truncate(end);
                line.remove(0, start);
                line.append("StarRail.exe");
                qDebug() << line;
                srPath = line;
                break;
            }
        }
    }
    if (giPath == "" && srPath == "") {
//        QMessageBox::question(parent, "悲报", "你的电脑中没有安装原神或崩铁，即将自动下载原神", "立刻下载", "或者下载");
        // 这个地方想必是不能自动获取最新的下载链接，过几个星期不知道还能不能下，再说吧
        QDesktopServices::openUrl(QUrl("https://ys-api.mihoyo.com/event/download_porter/link/ys_cn/official/pc_backup205;"));
    } else {
        QString path;
        if (srPath != "" && giPath != "") {
            if (QRandomGenerator::global()->bounded(1.0) < 0.5) {
                path = giPath;
            } else {
                path = srPath;
            }
        } else if (srPath != "") {
            path = srPath;
        } else {
            path = giPath;
        }
        QProcess process(parent);
        QStringList arguments;
        process.startDetached("\""+ path + "\"", arguments);
    }
}

/**
 * @author 朱炫曦
 * @date 2023/09/02
 * @brief 0.6%概率出货，90抽必出货。
 * @param parent QWidget parent.
 */
void gachaAutoStart(QWidget *parent) {
    static int counter = 0;
    counter++;
    if (counter == 90 || QRandomGenerator::global()->generateDouble() <= 0.006) {
        autoStart(parent);
        counter = 0;
    }
}
