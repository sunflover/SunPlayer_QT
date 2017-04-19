#include "center_widget.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QDebug>
#include "main_widget.h"
extern MainWidget *g_mainWidget;

CenterWidget::CenterWidget(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(33,34,40));
    setPalette(palette);

    //可获取鼠标跟踪效果
    setMouseTracking(true);
}

void CenterWidget::paintEvent(QPaintEvent*)
{
    if (g_mainWidget->hplayer == nullptr)
    {
        QPainter painter(this);
        QPixmap pix;
		pix.load(QString(":/player/player"));
        int width = pix.width();
        int height = pix.height();

        painter.drawPixmap(size().width()/2 - width/2,size().height()/2 - height/2,width,height,pix);
    }
}

