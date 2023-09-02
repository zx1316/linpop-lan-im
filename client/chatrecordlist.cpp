#include "chatrecordlist.h"
#include "QDebug"

ChatRecordList::ChatRecordList(QTextBrowser* browser)
{
    _browser = browser;
}

/**
 * 函数名称：append()
 * 描述：向记录列表末尾添加记录并显示该条记录
 * 参数：
 * 1.   record:ChatRecord   向记录列表末尾添加的记录
 * 返回值: void
 * 做成时间：2023.08.24
 * 作者：刘文景
 */
void ChatRecordList::append(ChatRecord record){
    _browser->setFontPointSize(9);
    _browser->setTextColor(QColor(0,0,0));
    _browser->setFontWeight(QFont::Normal);
    _browser->setFontItalic(false);
    _browser->setFontUnderline(false);
    _browser->append(record.fromUsername()+" "+record.datetime().toString(Qt::DateFormat::ISODate));

    _browser->setFontPointSize(record.fontsize());
    _browser->setFontWeight(record.board()?QFont::Bold:QFont::Normal);
    _browser->setFontItalic(record.italics());
    _browser->setFontUnderline(record.underline());
    _browser->setTextColor(record.color());
    _browser->append(record.message());
}
