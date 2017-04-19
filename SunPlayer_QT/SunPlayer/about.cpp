#include "about.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

About::About(QWidget *parent) : QDialog(parent)
  ,mouse_press(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    setFixedSize(520, 290);

    title_label = new QLabel();
    title_icon_label = new QLabel();
    title_info_label = new QLabel();
    close_button = new PushButton();
    info_label = new QLabel();
    version_label = new QLabel();
    compiler_label = new QLabel();
    build_label = new QLabel();
    copyright_label = new QLabel();
    icon_label = new QLabel();
    ok_button = new QPushButton();

    QPixmap title_pixmap(QString(":/ico/player"));
    title_icon_label->setPixmap(title_pixmap);
    title_icon_label->setFixedSize(16, 16);
    title_icon_label->setScaledContents(true);

    close_button->setPicName(QString(":/title/close"));


    title_label->setFixedHeight(19);
    ok_button->setFixedSize(75, 25);
    QPixmap pixmap(":/player/player");
    icon_label->setPixmap(pixmap);
    icon_label->setFixedSize(pixmap.size());

    title_label->setStyleSheet("color:white;");
    version_label->setStyleSheet("color:white;");
    compiler_label->setStyleSheet("color:white;");
    build_label->setStyleSheet("color:white;");
    copyright_label->setStyleSheet("color:gray;");
    title_info_label->setStyleSheet("color:white;");
    info_label->setStyleSheet("color:white;");
    ok_button->setStyleSheet("QPushButton{border:1px solid lightgray;background:rgb(230,230,230);}QPushButton:hover{border-color:green;background:transparent;}");

    QFont title_info_font(QString::fromLocal8Bit("微软雅黑"), 14, QFont::Bold, false);
    title_info_label->setFont(title_info_font);

    QFont info_font = info_label->font();
    info_font.setBold(true);
    info_label->setFont(info_font);

    QHBoxLayout *title_layout = new QHBoxLayout();
    title_layout->addWidget(title_icon_label, 0, Qt::AlignVCenter);
    title_layout->addWidget(title_label, 0, Qt::AlignVCenter);
    title_layout->addStretch();
    title_layout->addWidget(close_button, 0, Qt::AlignTop);
    title_layout->setSpacing(5);
    title_layout->setContentsMargins(5, 0, 0, 0);

    QVBoxLayout *v_layout = new QVBoxLayout();
    v_layout->addWidget(title_info_label);
    v_layout->addWidget(info_label);
    v_layout->addWidget(version_label);
    v_layout->addWidget(compiler_label);
    v_layout->addWidget(build_label);
    v_layout->addWidget(copyright_label);
    v_layout->addStretch();
    v_layout->setSpacing(5);
    v_layout->setContentsMargins(0, 15, 0, 0);
    info_label->setContentsMargins(0, 0, 0, 40);

    QHBoxLayout *bottom_layout = new QHBoxLayout();
    bottom_layout->addStretch();
    bottom_layout->addWidget(ok_button);
    bottom_layout->setSpacing(0);
    bottom_layout->setContentsMargins(0, 0, 30, 20);

    QHBoxLayout *h_layout = new QHBoxLayout();
    h_layout->addLayout(v_layout);
    h_layout->addWidget(icon_label);
    h_layout->setSpacing(0);
    h_layout->setContentsMargins(40, 0, 40, 0);

    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->addLayout(title_layout);
    main_layout->addStretch();
    main_layout->addLayout(h_layout);
    main_layout->addLayout(bottom_layout);
    main_layout->setSpacing(0);
    main_layout->setContentsMargins(5, 5, 5, 5);

    connect(ok_button, SIGNAL(clicked()), this, SLOT(hide()));
    connect(close_button, SIGNAL(clicked()), this, SLOT(hide()));

    this->setLayout(main_layout);


    title_label->setText(QString::fromLocal8Bit("关于SunPlayer"));
    title_info_label->setText(QString::fromLocal8Bit("SunPlayer"));
    info_label->setText(QString::fromLocal8Bit("简洁\t纯净\t好用"));
    version_label->setText(QString::fromLocal8Bit("主程序版本：v1.0.0.2"));
    compiler_label->setText(QString::fromLocal8Bit("Based on Qt 5.7.0 (MSVC 2013, 32 bit)"));
    build_label->setText(QString("Built on %1 %2").arg(__DATE__).arg(__TIME__));
    copyright_label->setText(QString::fromLocal8Bit("Copyright(c) xxoo.com Inc. All Rights Reserved."));
    close_button->setToolTip(QString::fromLocal8Bit("关闭"));
    ok_button->setText(QString::fromLocal8Bit("关闭"));

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(56,58,72));
    setPalette(palette);
}


void About::mousePressEvent(QMouseEvent *event)
{
    //只能是鼠标左键移动和改变大小
    if(event->button() == Qt::LeftButton)
    {
        mouse_press = true;
    }

    //窗口移动距离
    move_point = event->globalPos() - pos();
}

void About::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void About::mouseMoveEvent(QMouseEvent *event)
{
    //移动窗口
    if(mouse_press)
    {
        QPoint move_pos = event->globalPos();
        move(move_pos - move_point);
    }
}

