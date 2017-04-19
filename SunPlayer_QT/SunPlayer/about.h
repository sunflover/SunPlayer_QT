#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QLabel>
#include "push_button.h"

class About : public QDialog
{
    Q_OBJECT
public:
    explicit About(QWidget *parent = 0);

private:
    QLabel *title_label; //标题
    QLabel *title_icon_label; //标题图标
    QLabel *title_info_label; //标题文字
    PushButton *close_button; //关闭按钮

    QLabel *version_label; //SunPlayer v1.0
    QLabel *info_label; //纯净 支持全格式音视频播放

    QLabel *compiler_label; //Based on Qt 5.5.1 (MSVC 2013, 32 bit)
    QLabel *build_label; //Built on Oct 13 2015 07:38:22
    QLabel *copyright_label; //Copyright(c) xxoo.com All Right Reserved
    QLabel *icon_label; //图标

    QPushButton *ok_button; //确定按钮

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QPoint move_point; //移动的距离
    bool mouse_press; //按下鼠标左键

signals:

public slots:


};

#endif // ABOUT_H
