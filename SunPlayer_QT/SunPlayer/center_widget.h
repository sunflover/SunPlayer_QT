#ifndef CENTERWIDGET_H
#define CENTERWIDGET_H

#include <QWidget>

class CenterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CenterWidget(QWidget *parent = 0);
private:
    void paintEvent(QPaintEvent*);

};

#endif // CENTERWIDGET_H
