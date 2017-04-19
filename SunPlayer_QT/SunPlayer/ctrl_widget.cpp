#include "ctrl_widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include "SunFun.h"
#include "main_widget.h"
extern MainWidget *g_mainWidget;

CtrlWidget::CtrlWidget(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    fast_backward = new PushButton(this);//快退
    progress_slider = new ClickableSlider(this);//进度条
    progress_slider->setObjectName(QString::fromLocal8Bit("progress_slider"));
    progress_slider->setOrientation(Qt::Horizontal);
    progress_slider->setRange(0,100);
    progress_slider->setValue(0);
    fast_forward = new PushButton(this);//快进
    time_lable = new QLabel(this); //时间
    open_button = new PushButton(this);//打开
    stop_button = new PushButton(this);//停止
    previous_button = new PushButton(this);//前一个
    previous_button->setObjectName("Backward");
    play_button = new PushButton(this);//播放
    pause_button = new PushButton(this);//暂停
    next_button = new PushButton(this);//下一个
    next_button->setObjectName("Forward");
    vol_button = new PushButton(this);//音量
    vol_slider = new ClickableSlider(this);//音量滑块
    vol_slider->setObjectName(QString::fromLocal8Bit("vol_slider"));
    vol_slider->setOrientation(Qt::Horizontal);
    vol_slider->setRange(0,100);
    vol_slider->setValue(100);
    full_button = new PushButton(this); //全屏
    full_restore_button = new PushButton(this); //退出全屏
    sizebox = new PushButton(this);
    sizebox->setEnabled(false);

    fast_backward->setPicName(QString(":/ctrl/fast_backward"));
    QFile file(":/qss/QSlider");
    file.open(QFile::ReadOnly);
    QByteArray byte_array = file.readAll();
    file.close();
    progress_slider->setStyleSheet(byte_array);
    progress_slider->setFixedHeight(11);
    vol_slider->setStyleSheet(byte_array);

    fast_forward->setPicName(QString(":/ctrl/fast_forward"));
    time_lable->setStyleSheet("color:white;");
    time_lable->setText(QString("00:00:00/00:00:00"));
    open_button->setPicName(QString(":/ctrl/open"));    
    stop_button->setPicName(QString(":/ctrl/stop"));
    previous_button->setPicName(QString(":/ctrl/previous"));
    play_button->setPicName(QString(":/ctrl/play"));
    pause_button->setPicName(QString(":/ctrl/pause"));
    next_button->setPicName(QString(":/ctrl/next"));
    vol_button->setPicName(QString(":/ctrl/vol"));
    full_button->setPicName(QString(":/ctrl/full"));
    full_restore_button->setPicName(QString(":/ctrl/full_restore"));
    sizebox->setPicName(QString(":/ctrl/sizebox"));

    fast_backward->setToolTip(QString::fromLocal8Bit("快退10秒"));
    fast_forward->setToolTip(QString::fromLocal8Bit("快进10秒"));
    open_button->setToolTip(QString::fromLocal8Bit("打开"));
    stop_button->setToolTip(QString::fromLocal8Bit("停止"));
    previous_button->setToolTip(QString::fromLocal8Bit("左键：后退，右键：上一个"));
    play_button->setToolTip(QString::fromLocal8Bit("播放"));
    pause_button->setToolTip(QString::fromLocal8Bit("暂停"));
    next_button->setToolTip(QString::fromLocal8Bit("左键：前进，右键：下一个"));
    vol_button->setToolTip(QString::fromLocal8Bit("静音"));
    full_button->setToolTip(QString::fromLocal8Bit("全屏"));
    full_restore_button->setToolTip(QString::fromLocal8Bit("退出全屏"));

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->setContentsMargins(0,2,0,0);
    layout1->setSpacing(0);
    layout1->addWidget(fast_backward,0,Qt::AlignTop);
    layout1->addWidget(progress_slider,0,Qt::AlignTop);
    layout1->addWidget(fast_forward,0,Qt::AlignTop);

    QHBoxLayout *layout2left = new QHBoxLayout();
    layout2left->setContentsMargins(10,0,0,8);
    layout2left->setSpacing(5);
    layout2left->addWidget(time_lable, 0, Qt::AlignVCenter);
    layout2left->addWidget(open_button, 0, Qt::AlignVCenter);
    layout2left->addStretch();
    layout2left->addWidget(stop_button, 0,Qt::AlignVCenter);

    QHBoxLayout *layout2mid = new QHBoxLayout();
    layout2mid->setContentsMargins(0,0,0,4);
    layout2mid->setSpacing(5);
    layout2mid->addWidget(previous_button, 0,Qt::AlignVCenter);
    layout2mid->addWidget(play_button, 0, Qt::AlignCenter);
    pause_button->hide();
    layout2mid->addWidget(pause_button, 0, Qt::AlignCenter);
    layout2mid->addWidget(next_button, 0,Qt::AlignVCenter);

    QHBoxLayout *layout2right = new QHBoxLayout();
    layout2right->setContentsMargins(0,0,2,2);
    layout2right->setSpacing(5);
    layout2right->addSpacing(5);
    layout2right->addWidget(vol_button, 0,Qt::AlignVCenter);
    layout2right->addWidget(vol_slider, 0,Qt::AlignLeft);
    layout2right->addStretch();
    layout2right->addWidget(full_button, 0, Qt::AlignVCenter);
    full_restore_button->hide();
    layout2right->addWidget(full_restore_button, 0, Qt::AlignVCenter);
    layout2right->addWidget(sizebox, 0, Qt::AlignBottom | Qt::AlignRight);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setContentsMargins(0,0,0,0);
    layout2->addLayout(layout2left);
    layout2->addLayout(layout2mid);
    layout2->addLayout(layout2right);

    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->setContentsMargins(0,0,0,0);
    main_layout->addLayout(layout1);
    main_layout->addLayout(layout2);    
    setLayout(main_layout);
    this->setFixedHeight(80);

    //信号
    connect(fast_backward,SIGNAL(clicked()),this,SIGNAL(sigBackward()));
    connect(fast_forward,SIGNAL(clicked()),this,SIGNAL(sigForward()));
    connect(previous_button,SIGNAL(clicked()),this,SIGNAL(sigBackward()));
    connect(next_button,SIGNAL(clicked()),this,SIGNAL(sigForward()));
    connect(open_button,SIGNAL(clicked()),this,SIGNAL(sigOpen()));
    connect(stop_button,SIGNAL(clicked()),this,SIGNAL(sigStop()));    
    connect(play_button,SIGNAL(clicked()),this,SIGNAL(sigPlay()));
    connect(pause_button,SIGNAL(clicked()),this,SIGNAL(sigPause()));    
    connect(vol_button,SIGNAL(clicked()),this,SIGNAL(sigVol()));
    connect(vol_slider,SIGNAL(sliderMoved(int)),this,SIGNAL(sigVolSeek(int)));
    connect(progress_slider,SIGNAL(sliderMoved(int)),this,SIGNAL(sigSeek(int)));
    connect(full_button, SIGNAL(clicked()), this, SIGNAL(sigFullScreen()));
    connect(full_restore_button, SIGNAL(clicked()), this, SIGNAL(sigRestoreFullScreen()));

    open_button->setDefault(true);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(56,58,72));
    setPalette(palette);

    //可获取鼠标跟踪效果
    setMouseTracking(true);
}


bool CtrlWidget::event(QEvent *event)
{
    if (event->type() == MyCustomEventTypeA)
    {
        // processing...
        emit sigSeek(progress_slider->value());
        return true;
    }
    else if (event->type() == MyCustomEventTypeB)
    {
        // processing...
        emit sigVolSeek(vol_slider->value());
        return true;
    }

    return QWidget::event(event);
}
