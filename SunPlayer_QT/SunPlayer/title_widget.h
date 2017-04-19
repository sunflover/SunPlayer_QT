#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QtWidgets>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QSignalMapper>
#include "push_button.h"

class TitleWidget : public QWidget
{
	Q_OBJECT
public:
    explicit TitleWidget(QWidget *parent = 0);

signals:
    void sigMenu();
    void sigPin();
    void sigUnPin();
    void sigMin();
    void sigMax();
    void sigRestoreMax();
    void sigFull();
    void sigRestoreFull();
    void sigClose();

public:
    PushButton *menu_button; //菜单
    QLabel	   *title; //标题
    PushButton *pin_button; //总在最前
    PushButton *unpin_button; //不在最前
    PushButton *min_button; //最小化
    PushButton *max_button; //最大化
    PushButton *max_restore_button; //恢复
    PushButton *full_button; //全屏
    PushButton *full_restore_button; //退出全屏
    PushButton *close_button; //关闭
};

#endif // TITLEWIDGET_H
