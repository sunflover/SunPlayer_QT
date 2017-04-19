#include "title_widget.h"


TitleWidget::TitleWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    menu_button = new PushButton(this);
    title = new QLabel(this);
    pin_button = new PushButton(this);
    unpin_button = new PushButton(this);
    min_button = new PushButton(this);
    max_button = new PushButton(this);
    max_restore_button = new PushButton(this);
    full_button = new PushButton(this);
    full_restore_button = new PushButton(this);
    close_button = new PushButton(this);

		
    menu_button->setPicName(QString(":/title/menu"));
    title->setStyleSheet("color:white;");
    pin_button->setPicName(QString(":/title/pin"));
    unpin_button->setPicName(QString(":/title/unpin"));
    min_button->setPicName(QString(":/title/min"));
    max_button->setPicName(QString(":/title/max"));
    max_restore_button->setPicName(QString(":/title/restore"));
    full_button->setPicName(QString(":/title/full"));
    full_restore_button->setPicName(QString(":/title/full_restore"));
    close_button->setPicName(QString(":/title/close"));

    menu_button->setToolTip(QString::fromLocal8Bit("菜单"));
    pin_button->setToolTip(QString::fromLocal8Bit("总在最前"));
    unpin_button->setToolTip(QString::fromLocal8Bit("取消最前"));
    min_button->setToolTip(QString::fromLocal8Bit("最小化"));
    max_button->setToolTip(QString::fromLocal8Bit("最大化"));
    max_restore_button->setToolTip(QString::fromLocal8Bit("恢复尺寸"));
    full_button->setToolTip(QString::fromLocal8Bit("全屏"));
    full_restore_button->setToolTip(QString::fromLocal8Bit("退出全屏"));
    close_button->setToolTip(QString::fromLocal8Bit("关闭"));

    connect(menu_button, SIGNAL(clicked()), this, SIGNAL(sigMenu()));
    connect(pin_button, SIGNAL(clicked()), this, SIGNAL(sigPin()));
    connect(unpin_button, SIGNAL(clicked()), this, SIGNAL(sigUnPin()));
    connect(min_button, SIGNAL(clicked()), this, SIGNAL(sigMin()));
    connect(max_button, SIGNAL(clicked()), this, SIGNAL(sigMax()));
    connect(max_restore_button, SIGNAL(clicked()), this, SIGNAL(sigRestoreMax()));
    connect(full_button, SIGNAL(clicked()), this, SIGNAL(sigFull()));
    connect(full_restore_button, SIGNAL(clicked()), this, SIGNAL(sigRestoreFull()));
    connect(close_button, SIGNAL(clicked()), this, SIGNAL(sigClose()));

    QHBoxLayout *main_layout = new QHBoxLayout(this);
    main_layout->addWidget(menu_button, 0, Qt::AlignTop);
    main_layout->addSpacing(5);
    main_layout->addWidget(title, 0, Qt::AlignLeft);
    main_layout->addStretch();
    main_layout->addWidget(pin_button, 0, Qt::AlignTop);
    unpin_button->hide();
    main_layout->addWidget(unpin_button, 0, Qt::AlignTop);
    main_layout->addWidget(min_button, 0, Qt::AlignTop);
    main_layout->addWidget(max_button, 0, Qt::AlignTop);
    max_restore_button->hide();
    main_layout->addWidget(max_restore_button, 0, Qt::AlignTop);
    main_layout->addWidget(full_button, 0, Qt::AlignTop);
    full_restore_button->hide();
    main_layout->addWidget(full_restore_button, 0, Qt::AlignTop);
    main_layout->addWidget(close_button, 0, Qt::AlignTop);
    main_layout->setSpacing(0);
    main_layout->setContentsMargins(2, 2, 3, 2);
    setFixedHeight(20);
    setLayout(main_layout);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(56,58,72));
    setPalette(palette);

    //可获取鼠标跟踪效果
    setMouseTracking(true);
}
