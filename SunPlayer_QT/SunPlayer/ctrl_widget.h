#ifndef CTRLWIDGET_H
#define CTRLWIDGET_H

#include <QWidget>
#include "push_button.h"
#include <QLabel>
#include <QSlider>
#include <QStyleOptionSlider>
#include <QEvent>
#include <QApplication>


#define MyCustomEventTypeA (QEvent::User + 1)
#define MyCustomEventTypeB (QEvent::User + 2)

class ClickableSlider : public QSlider
{
public:
    ClickableSlider(QWidget *parent = 0) : QSlider(parent) {}

protected:
    void ClickableSlider::mousePressEvent(QMouseEvent *event)
    {
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

        if (event->button() == Qt::LeftButton && !sr.contains(event->pos()))
        {
            int newVal;
            if (orientation() == Qt::Vertical)
            {
                double halfHandleHeight = (0.5 * sr.height()) + 0.5;
                int adaptedPosY = height() - event->y();
                if ( adaptedPosY < halfHandleHeight )
                    adaptedPosY = halfHandleHeight;
                if ( adaptedPosY > height() - halfHandleHeight )
                    adaptedPosY = height() - halfHandleHeight;
                double newHeight = (height() - halfHandleHeight) - halfHandleHeight;
                double normalizedPosition = (adaptedPosY - halfHandleHeight)  / newHeight ;

                newVal = minimum() + (maximum() - minimum()) * normalizedPosition;
            }
            else
            {
                double halfHandleWidth = (0.5 * sr.width()) + 0.5;
                int adaptedPosX = event->x();
                if ( adaptedPosX < halfHandleWidth )
                    adaptedPosX = halfHandleWidth;
                if ( adaptedPosX > width() - halfHandleWidth )
                    adaptedPosX = width() - halfHandleWidth;
                double newWidth = (width() - halfHandleWidth) - halfHandleWidth;
                double normalizedPosition = (adaptedPosX - halfHandleWidth)  / newWidth ;

                newVal = minimum() + ((maximum() - minimum()) * normalizedPosition);
            }

            if (invertedAppearance())
                setValue( maximum() - newVal );
            else
                setValue(newVal);


            //向父窗口发送自定义事件event type，这样就可以在父窗口中捕获这个事件进行处理
            if(this->objectName() == QString::fromLocal8Bit("progress_slider"))
            {
                QEvent evEvent(static_cast<QEvent::Type>(MyCustomEventTypeA));
                QCoreApplication::sendEvent(parentWidget(), &evEvent);
            }
            else if(this->objectName() == QString::fromLocal8Bit("vol_slider"))
            {
                QEvent evEvent(static_cast<QEvent::Type>(MyCustomEventTypeB));
                QCoreApplication::sendEvent(parentWidget(), &evEvent);
            }

            event->accept();
        }
        else
        {
            QSlider::mousePressEvent(event);
        }
    }
};



class CtrlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CtrlWidget(QWidget *parent = 0);
    bool event(QEvent *event);

signals:
    void sigBackward();
    void sigForward();
    void sigOpen();
    void sigStop();
    void sigPlay();
    void sigPause();
    void sigVol();
    void sigVolSeek(int value);
    void sigFullScreen();
    void sigRestoreFullScreen();
    void sigSeek(int value);


public:
    PushButton *fast_backward;//快退
    ClickableSlider *progress_slider;//进度滑块
    PushButton *fast_forward;//快进
    QLabel	   *time_lable; //标题
    PushButton *open_button;//打开
    PushButton *stop_button;//停止
    PushButton *previous_button;//前一个
    PushButton *play_button;//播放
    PushButton *pause_button;//暂停
    PushButton *next_button;//下一个
    PushButton *vol_button;//音量
    ClickableSlider *vol_slider;//音量滑块
    PushButton *full_button;//全屏
    PushButton *full_restore_button;//退出全屏
    PushButton *sizebox;
};



#endif // CTRLWIDGET_H

