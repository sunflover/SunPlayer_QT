#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "SingleApplication.h"
#include "title_widget.h"
#include "center_widget.h"
#include "ctrl_widget.h"
#include "about.h"

#include <QMouseEvent>
#include <QPoint>
#include <QCursor>
#include <QRect>
#define PADDING 2

enum Direction{
    UP = 0,
    DOWN=1,
    LEFT,
    RIGHT,
    LEFTTOP,
    LEFTBOTTOM,
    RIGHTBOTTOM,
    RIGHTTOP,
    NONE
};

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
    int64_t getVideoDuration();
    int64_t getVideoPosition();

public:
    TitleWidget *title_widget; //标题栏
    CenterWidget *central_widget; //播放窗口
    CtrlWidget *ctrl_widget; //标题栏
    About *about;
    QMenu *popMenu;    
    QMenu *menuVideo;
    QMenu *menuPlayCtrl;
    QMenu *menuVolCtrl;
    QMenu *menuSeek;
    QMenu *menuPlaySpeed;
    QMenu *menuVolSync;
    QAction *actOpenFile;
    QAction *actCloseFile;
    QAction *actVideoGDI;
    QAction *actVideoD3D;
    QAction *actPlayPause;
    QAction *actNext;
    QAction *actPrevious;
    QAction *actBackward;//快退10s
    QAction *actForward;//快进10s
    QAction *actSpeedNormal;//正常
    QAction *actSpeedUp;//加速
    QAction *actSpeedDown;//减速
    QAction *actVolUp;
    QAction *actVolDown;
    QAction *actVolOff;
    QAction *actVolSyncBefore;
    QAction *actVolSyncAfter;
    QAction *actVolSyncReset;
    QAction *actFullScreen;
    QAction *actFullScreen1;
    QAction *actAbout;
    QAction *actExit;



public slots:
    void slotMenu(void);
    void slotPin(void);
    void slotUnPin(void);
    void slotMax(void);
    void slotRestoreMax(void);
    void slotFullScreen(void);
    void slotRestoreFullScreen(void);

    void slotSpeedNormal(void);
    void slotSpeedUp(void);
    void slotSpeedDown(void);
    void slotSpeedChange(int speed);
    void slotForward(void);
    void slotBackward(void);
    void slotOpen(void);
    void Play(const QString &strFilePath);
    void Play(void);
    void Pause(void);
    void Stop(void);
    void Previous(void);
    void Next(void);
    void slotVol(void);
    void slotVolUp(void);
    void slotVolDown(void);
    void slotVolSyncAfter(void);
    void slotVolSyncBefore(void);
    void slotVolSyncReset(void);
    void slotVolSync(int value);
    void slotVolSeek(int val);
    void slotSeek(int value);
    void slotAbout(void);
    void slotVideoGDI(void);
    void slotVideoD3D(void);




protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    bool eventFilter(QObject *target, QEvent *event);

    void contextMenuEvent(QContextMenuEvent *event);
    void customEvent(QEvent *event); //该函数是父类QWidget的虚函数

    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

    void showFullScreen();
    void showNormal();

    void centerWindow(void);

private:
    QRect location;
    bool bVol;
    bool isLeftPressDown;
    QPoint dragPosition;
    Direction dir;

    int getVideoWidth();
    int getVideoHeight();
    int getVolume();
    void setVolume(int val);
    void loadSettings();
    void saveSettings();
    QString readPath();
    void writePath(QString strPath);
    void region(const QPoint &cursorPoint);

public:
    void *hplayer;
    int64_t m_duration;
    // 是否处于播放状态
    int m_status;//播放器状态,1：播放状态，2：暂停状态，3：停止状态
    // 正在播放的文件在播放列表中的索引值
    int m_nIndex;
    // 播放列表
    QVector<QString> m_VecFileNames;
    bool m_bGDI;
};


#endif // MAINWIDGET_H


