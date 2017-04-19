#include "main_widget.h"
#include <QFileDialog>
#include <QSettings>
#include <QIcon>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopWidget>
#include "SunFun.h"
#include "ffplayer.h"
#include <string>
using std::string;

#define DEFAULT_WIDTH   800
#define DEFAULT_HEIGHT  600
#define MIN_WIDTH       500
#define MIN_HEIGHT      200

#define XIFU            10

MainWidget *g_mainWidget = nullptr;
__int64 g_pos = 0;

const QEvent::Type CustomEvent_Progress = (QEvent::Type)5001;//建议用5000以上唯一的标识
const QEvent::Type CustomEvent_Complete = (QEvent::Type)5002;//建议用5000以上唯一的标识


MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , isLeftPressDown(false)
    , dir(NONE)
    , hplayer(nullptr)
    , bVol(true)
    , m_duration(0)
    , m_status(0)
    , m_bGDI(true)
{
    g_mainWidget = this;
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    setMouseTracking(true);
    //允许拖放文件
    setAcceptDrops(true);
    loadSettings();
    location = this->geometry();    
    setWindowIcon(QIcon(":/img/player.ico"));
    title_widget = new TitleWidget(this);
    central_widget = new CenterWidget(this);
    ctrl_widget = new CtrlWidget(this);
    about = new About(this);

    QVBoxLayout *center_layout = new QVBoxLayout();
    center_layout->addWidget(central_widget);
    center_layout->setSpacing(0);
    center_layout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->addWidget(title_widget);
    main_layout->addLayout(center_layout);
    main_layout->addWidget(ctrl_widget);
    main_layout->setSpacing(0);
    main_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout);

    connect(title_widget, SIGNAL(sigMenu()), this, SLOT(slotMenu()));
    connect(title_widget, SIGNAL(sigPin()), this, SLOT(slotPin()));
    connect(title_widget, SIGNAL(sigUnPin()), this, SLOT(slotUnPin()));
    connect(title_widget, SIGNAL(sigMin()), this, SLOT(showMinimized()));
    connect(title_widget, SIGNAL(sigMax()), this, SLOT(slotMax()));
    connect(title_widget, SIGNAL(sigRestoreMax()), this, SLOT(slotRestoreMax()));
    connect(title_widget, SIGNAL(sigFull()), this, SLOT(slotFullScreen()));
    connect(title_widget, SIGNAL(sigRestoreFull()), this, SLOT(slotRestoreFullScreen()));
    connect(title_widget, SIGNAL(sigClose()), this, SLOT(close()));

    connect(ctrl_widget, SIGNAL(sigBackward()), this, SLOT(slotBackward()));
    connect(ctrl_widget, SIGNAL(sigForward()), this, SLOT(slotForward()));
    connect(ctrl_widget, SIGNAL(sigOpen()), this, SLOT(slotOpen()));
    connect(ctrl_widget, SIGNAL(sigStop()), this, SLOT(Stop()));
    connect(ctrl_widget, SIGNAL(sigPlay()), this, SLOT(Play()));
    connect(ctrl_widget, SIGNAL(sigPause()), this, SLOT(Pause()));
    connect(ctrl_widget, SIGNAL(sigSeek(int)), this, SLOT(slotSeek(int)));
    connect(ctrl_widget, SIGNAL(sigVol()), this, SLOT(slotVol()));
    connect(ctrl_widget, SIGNAL(sigVolSeek(int)), this, SLOT(slotVolSeek(int)));
    connect(ctrl_widget, SIGNAL(sigFullScreen()), this, SLOT(slotFullScreen()));
    connect(ctrl_widget, SIGNAL(sigRestoreFullScreen()), this, SLOT(slotRestoreFullScreen()));


    //弹出菜单
    popMenu = new QMenu(this);
    actOpenFile = new QAction(QString::fromLocal8Bit("打开文件\tF3"), this);
    actCloseFile = new QAction(QString::fromLocal8Bit("关闭文件\tF4"), this);
    menuVideo = new QMenu(popMenu);
    menuVideo->setTitle(QString::fromLocal8Bit("视频渲染模式"));
    actVideoGDI = new QAction(QString::fromLocal8Bit("GDI"), this);
    actVideoD3D = new QAction(QString::fromLocal8Bit("D3D"), this);
    actVideoGDI->setCheckable(true);
    actVideoD3D->setCheckable(true);
    actVideoGDI->setChecked(m_bGDI);
    actVideoD3D->setChecked(!m_bGDI);
    menuPlayCtrl = new QMenu(popMenu);
    menuPlayCtrl->setTitle(QString::fromLocal8Bit("播放控制"));
    actPlayPause = new QAction(QString::fromLocal8Bit("播放|暂停\t空格键"), this);
    actPrevious = new QAction(QString::fromLocal8Bit("上一文件\tPgUp"), this);
    actNext = new QAction(QString::fromLocal8Bit("下一文件\tPgDn"), this);

    menuSeek = new QMenu(menuPlayCtrl);
    menuSeek->setTitle(QString::fromLocal8Bit("定位"));
    actForward = new QAction(QString::fromLocal8Bit("快进10秒\t→"), this);
    actBackward = new QAction(QString::fromLocal8Bit("快退10秒\t←"), this);

    menuPlaySpeed = new QMenu(menuPlayCtrl);
    menuPlaySpeed->setTitle(QString::fromLocal8Bit("播放速度"));
    actSpeedNormal = new QAction(QString::fromLocal8Bit("正常\tZ"), this);//正常
    actSpeedUp = new QAction(QString::fromLocal8Bit("加速+\tC"), this);
    actSpeedDown = new QAction(QString::fromLocal8Bit("减速-\tX"), this);;//减速

    actSpeedNormal->setCheckable(true);
    actSpeedNormal->setChecked(true);
    actSpeedUp->setCheckable(true);
    actSpeedDown->setCheckable(true);

    menuVolCtrl = new QMenu(popMenu);
    menuVolCtrl->setTitle(QString::fromLocal8Bit("声音控制"));
    actVolUp = new QAction(QString::fromLocal8Bit("音量+\t↑"), this);
    actVolDown = new QAction(QString::fromLocal8Bit("音量-\t↓"), this);
    actVolOff = new QAction(QString::fromLocal8Bit("静音\tM"), this);
    actVolOff->setCheckable(true);

    menuVolSync = new QMenu(menuVolCtrl);
    menuVolSync->setTitle(QString::fromLocal8Bit("声音同步"));
    actVolSyncAfter = new QAction(QString::fromLocal8Bit("滞后0.1秒\tShift+<"), this);
    actVolSyncBefore = new QAction(QString::fromLocal8Bit("超前0.1秒\tShift+>"), this);
    actVolSyncReset = new QAction(QString::fromLocal8Bit("复位\tShift+?"), this);
    actVolSyncAfter->setCheckable(true);
    actVolSyncBefore->setCheckable(true);
    actVolSyncReset->setCheckable(true);
    actVolSyncReset->setChecked(true);

    actFullScreen = new QAction(QString::fromLocal8Bit("全屏\tEnter"), this);
    actFullScreen->setCheckable(true);
    actFullScreen1 = new QAction(QString::fromLocal8Bit("全屏+\tCtrl+Enter"), this);
    actFullScreen1->setCheckable(true);
    actAbout = new QAction(QString::fromLocal8Bit("关于\tF1"), this);
    actExit = new QAction(QString::fromLocal8Bit("退出\tAlt+F4"), this);

    popMenu->addAction(actOpenFile);
    popMenu->addAction(actCloseFile);
    popMenu->addSeparator();
    popMenu->addMenu(menuVideo);
    menuVideo->addAction(actVideoGDI);
    menuVideo->addAction(actVideoD3D);
    popMenu->addMenu(menuPlayCtrl);
    menuPlayCtrl->addAction(actPlayPause);
    menuPlayCtrl->addSeparator();
    menuPlayCtrl->addAction(actPrevious);
    menuPlayCtrl->addAction(actNext);
    menuPlayCtrl->addSeparator();
    menuPlayCtrl->addMenu(menuSeek);
    menuSeek->addAction(actForward);
    menuSeek->addAction(actBackward);
    menuPlayCtrl->addMenu(menuPlaySpeed);
    menuPlaySpeed->addAction(actSpeedNormal);
    menuPlaySpeed->addAction(actSpeedUp);
    menuPlaySpeed->addAction(actSpeedDown);

    popMenu->addMenu(menuVolCtrl);
    menuVolCtrl->addAction(actVolUp);
    menuVolCtrl->addAction(actVolDown);
    menuVolCtrl->addAction(actVolOff);
    menuVolCtrl->addSeparator();
    menuVolCtrl->addMenu(menuVolSync);
    menuVolSync->addAction(actVolSyncAfter);
    menuVolSync->addAction(actVolSyncBefore);
    menuVolSync->addAction(actVolSyncReset);

    popMenu->addAction(actFullScreen);
    popMenu->addAction(actFullScreen1);
    popMenu->addSeparator();
    popMenu->addAction(actAbout);
    popMenu->addAction(actExit);

    connect(actOpenFile, SIGNAL(triggered()), this, SLOT(slotOpen()));
    connect(actCloseFile, SIGNAL(triggered()), this, SLOT(Stop()));
    connect(actVideoGDI, SIGNAL(triggered()), this, SLOT(slotVideoGDI()));
    connect(actVideoD3D, SIGNAL(triggered()), this, SLOT(slotVideoD3D()));
    connect(actPlayPause, SIGNAL(triggered()), this, SLOT(Play()));
    connect(actPrevious, SIGNAL(triggered()), this, SLOT(Previous()));
    connect(actNext, SIGNAL(triggered()), this, SLOT(Next()));
    connect(actSpeedNormal, SIGNAL(triggered()), this, SLOT(slotSpeedNormal()));
    connect(actSpeedUp, SIGNAL(triggered()), this, SLOT(slotSpeedUp()));
    connect(actSpeedDown, SIGNAL(triggered()), this, SLOT(slotSpeedDown()));
    connect(actForward, SIGNAL(triggered()), this, SLOT(slotForward()));
    connect(actBackward, SIGNAL(triggered()), this, SLOT(slotBackward()));

    connect(actVolDown, SIGNAL(triggered()), this, SLOT(slotVolDown()));
    connect(actVolUp, SIGNAL(triggered()), this, SLOT(slotVolUp()));
    connect(actVolOff, SIGNAL(triggered()), this, SLOT(slotVol()));
    connect(actVolSyncAfter, SIGNAL(triggered()), this, SLOT(slotVolSyncAfter()));
    connect(actVolSyncBefore, SIGNAL(triggered()), this, SLOT(slotVolSyncBefore()));
    connect(actVolSyncReset, SIGNAL(triggered()), this, SLOT(slotVolSyncReset()));
    connect(actFullScreen, SIGNAL(triggered()), this, SLOT(slotFullScreen()));
    connect(actFullScreen1, SIGNAL(triggered()), this, SLOT(slotFullScreen()));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));
    connect(actExit, SIGNAL(triggered()), this, SLOT(close()));
}


MainWidget::~MainWidget()
{
    player_close(hplayer);
}


void MainWidget::centerWindow(void)
{
    move((qApp->desktop()->availableGeometry().width() - width()) / 2 + qApp->desktop()->availableGeometry().x(),
         (qApp->desktop()->availableGeometry().height() - height()) / 2 + qApp->desktop()->availableGeometry().y());
}

void MainWidget::slotMenu(void)
{
    popMenu->exec(central_widget->mapToGlobal(QPoint(0, 0)));
}

void MainWidget::slotPin(void)
{
    title_widget->pin_button->hide();
    title_widget->unpin_button->show();
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    show();
}

void MainWidget::slotUnPin(void)
{
    title_widget->unpin_button->hide();
    title_widget->pin_button->show();
    this->setWindowFlags(this->windowFlags() &~ Qt::WindowStaysOnTopHint);
    show();
}


void MainWidget::slotMax(void)
{
    title_widget->max_button->hide();
    title_widget->max_restore_button->show();
    //获取当前界面的位置
    if(!this->isFullScreen())
    {
        location = this->geometry();
    }
    this->setGeometry(SingleApplication::desktop()->availableGeometry());

    title_widget->show();
    ctrl_widget->show();
    actFullScreen->setChecked(false);
    actFullScreen1->setChecked(false);
}

void MainWidget::slotRestoreMax(void)
{
    title_widget->max_restore_button->hide();
    title_widget->max_button->show();
    this->setGeometry(location);

    title_widget->show();
    ctrl_widget->show();
    actFullScreen->setChecked(false);
    actFullScreen1->setChecked(false);
}

void MainWidget::loadSettings()
{
    QSettings settings("BestDRM", "SunPlayer");

    QRect rc(settings.value(QString::fromLocal8Bit("geometry")).toPoint(),QSize(DEFAULT_WIDTH,DEFAULT_HEIGHT));
    setGeometry(rc);

    QVariant var = settings.value(QString::fromLocal8Bit("GDI"));
    if(var.isNull())
        m_bGDI = true;
    else
        m_bGDI = settings.value(QString::fromLocal8Bit("GDI")).toBool();

    setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
}

void MainWidget::saveSettings()
{
    QSettings settings("BestDRM", "SunPlayer");
    QPoint point(title_widget->mapToGlobal(QPoint(0, 0)));//获取控件在窗体中的坐标
    settings.setValue("geometry", point);
    settings.setValue("GDI", m_bGDI);
}

QString MainWidget::readPath()
{
    QSettings settings("BestDRM", "SunPlayer");
    return settings.value(QString::fromLocal8Bit("path")).toString();
}

void MainWidget::writePath(QString strPath)
{
    QSettings settings("BestDRM", "SunPlayer");
    settings.setValue("path", strPath);
}

void MainWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
}

void MainWidget::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
    {
        return;
    }

    QString fileName = urls.first().toLocalFile();
    if (fileName.isEmpty())
    {
        return;
    }
    else
    {
        Play(fileName);
    }
}

void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isLeftPressDown = false;
        if(dir != NONE)
        {
            this->releaseMouse();
            this->setCursor(QCursor(Qt::ArrowCursor));
        }

        // 顶部吸附功能
        QDesktopWidget *desktopWidget = QApplication::desktop();
        //获取可用桌面大小
        QRect deskRect = desktopWidget->availableGeometry();
        QRect rc = this->geometry();
        if (rc.top() < deskRect.top())
        {
            this->setGeometry(rc.left(), deskRect.top(), rc.right() - rc.left(), rc.bottom() - rc.top());
        }
    }
    else if(event->button() == Qt::RightButton)
    {
        QPoint point =this->mapToGlobal(event->pos());
        //获取控件在窗体中的坐标
        QPoint BackwardPoint(ctrl_widget->previous_button->mapToGlobal(QPoint(0,0)));
        QRect rcBackward(BackwardPoint,ctrl_widget->previous_button->size());
        if(rcBackward.contains(point))
        {
           Previous();
           return;
        }

        QPoint ForwardPoint(ctrl_widget->next_button->mapToGlobal(QPoint(0,0)));
        QRect rcForward(ForwardPoint,ctrl_widget->next_button->size());
        if(rcForward.contains(point))
        {
           Next();
           return;
        }
    }
}

void MainWidget::mousePressEvent(QMouseEvent *event)
{
    switch(event->button())
    {
    case Qt::LeftButton:
        isLeftPressDown = true;
        if(dir != NONE)
        {
            this->mouseGrabber();
        }
        else
        {
            dragPosition = event->globalPos() - this->frameGeometry().topLeft();
        }
        break;


    default:
        QWidget::mousePressEvent(event);
    }
}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint gloPoint = event->globalPos();
    QRect rect = this->rect();
    QPoint tl = mapToGlobal(rect.topLeft());
    QPoint rb = mapToGlobal(rect.bottomRight());
    if(this->isFullScreen())
    {
        //标题栏
        if(event->globalY() <= title_widget->height())
        {
            title_widget->raise();
            title_widget->show();
        }
        else
        {
            title_widget->hide();
        }
        //播放控制
        int screen_height = SingleApplication::desktop()->screenGeometry().height();
        if(event->globalY() + ctrl_widget->height() >= screen_height)
        {
            ctrl_widget->raise();
            ctrl_widget->show();
        }
        else
        {
            ctrl_widget->hide();
        }
    }
    else
    {
        if(!isLeftPressDown)
        {
            // 调整大小
            this->region(gloPoint);
        }
        else
        {
            if(dir != NONE)
            {
                QRect rMove(tl, rb);

                switch(dir)
                {
                case LEFT:
                    if(rb.x() - gloPoint.x() <= this->minimumWidth())
                        rMove.setX(tl.x());
                    else
                        rMove.setX(gloPoint.x());
                    break;
                case RIGHT:
                    rMove.setWidth(gloPoint.x() - tl.x());
                    break;
                case UP:
                    if(rb.y() - gloPoint.y() <= this->minimumHeight())
                        rMove.setY(tl.y());
                    else
                        rMove.setY(gloPoint.y());
                    break;
                case DOWN:
                    rMove.setHeight(gloPoint.y() - tl.y());
                    break;
                case LEFTTOP:
                    if(rb.x() - gloPoint.x() <= this->minimumWidth())
                        rMove.setX(tl.x());
                    else
                        rMove.setX(gloPoint.x());
                    if(rb.y() - gloPoint.y() <= this->minimumHeight())
                        rMove.setY(tl.y());
                    else
                        rMove.setY(gloPoint.y());
                    break;
                case RIGHTTOP:
                    rMove.setWidth(gloPoint.x() - tl.x());
                    rMove.setY(gloPoint.y());
                    break;
                case LEFTBOTTOM:
                    rMove.setX(gloPoint.x());
                    rMove.setHeight(gloPoint.y() - tl.y());
                    break;
                case RIGHTBOTTOM:
                    rMove.setWidth(gloPoint.x() - tl.x());
                    rMove.setHeight(gloPoint.y() - tl.y());
                    break;
                default:

                    break;
                }
                this->setGeometry(rMove);
            }
            else
            {
                // 在这儿实现吸附功能
                move(event->globalPos() - dragPosition);
                event->accept();
            }
        }

    }

    QWidget::mouseMoveEvent(event);
}


void MainWidget::region(const QPoint &cursorGlobalPoint)
{
    QRect rect = this->rect();
    QPoint tl = mapToGlobal(rect.topLeft());
    QPoint rb = mapToGlobal(rect.bottomRight());
    int x = cursorGlobalPoint.x();
    int y = cursorGlobalPoint.y();

    if(tl.x() + PADDING >= x && tl.x() <= x && tl.y() + PADDING >= y && tl.y() <= y)
    {
        // 左上角
        dir = LEFTTOP;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else if(x >= rb.x() - PADDING && x <= rb.x() && y >= rb.y() - PADDING && y <= rb.y())
    {
        // 右下角
        dir = RIGHTBOTTOM;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else if(x <= tl.x() + PADDING && x >= tl.x() && y >= rb.y() - PADDING && y <= rb.y())
    {
        //左下角
        dir = LEFTBOTTOM;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if(x <= rb.x() && x >= rb.x() - PADDING && y >= tl.y() && y <= tl.y() + PADDING)
    {
        // 右上角
        dir = RIGHTTOP;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if(x <= tl.x() + PADDING && x >= tl.x())
    {
        // 左边
        dir = LEFT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if( x <= rb.x() && x >= rb.x() - PADDING)
    {
        // 右边
        dir = RIGHT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if(y >= tl.y() && y <= tl.y() + PADDING)
    {
        // 上边
        dir = UP;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else if(y <= rb.y() && y >= rb.y() - PADDING)
    {
        // 下边
        dir = DOWN;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else
    {
        // 默认
        dir = NONE;
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}


void MainWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint point = event->pos();

    // 双击标题栏，最大化
    QRect rect = title_widget->geometry();
    if(rect.contains(point))
    {
        if(title_widget->max_button->isVisible())
        {
            slotMax();
        }
        else
        {
            slotRestoreMax();
        }
        return;
    }

    // 双击暂停，播放切换
    if (m_status == 1)
    {
        Pause();
    }
    else
    {
        Play();
    }
}


bool MainWidget::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        int uKey = keyEvent->key();
        //获取修饰键(Ctrl,Alt,Shift)的状态
        Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
        //判断某个修饰键是否被按下
        if(modifiers & Qt::ShiftModifier)
            uKey += Qt::SHIFT;
        if(modifiers & Qt::ControlModifier)
            uKey += Qt::CTRL;
        if(modifiers & Qt::AltModifier)
            uKey += Qt::ALT;
        //QMessageBox::information(this,"msg", QKeySequence(uKey).toString(QKeySequence::NativeText) ,QMessageBox::Ok);
        if(QKeySequence("F3") == QKeySequence(uKey))
        {
            slotOpen();
            return true;
        }
        else if(QKeySequence("F4") == QKeySequence(uKey))
        {
            Stop();
            return true;
        }
        else if(QKeySequence("F1") == QKeySequence(uKey))
        {
            slotAbout();
            return true;
        }
        else if(QKeySequence("ALT+F4") == QKeySequence(uKey))
        {
            close();
            return true;
        }
        else if(QKeySequence(Qt::Key_Space) == QKeySequence(uKey))
        {
            if(m_status == 1)
                Pause();
            else
                Play();
            return true;
        }
        else if(QKeySequence(Qt::Key_PageUp) == QKeySequence(uKey))
        {
            Previous();
            return true;
        }
        else if(QKeySequence(Qt::Key_PageDown) == QKeySequence(uKey))
        {
            Next();
            return true;
        }
        else if(QKeySequence("Shift+>") == QKeySequence(uKey))
        {
            slotVolSyncBefore();
            return true;
        }
        else if(QKeySequence("Shift+<") == QKeySequence(uKey))
        {
            slotVolSyncAfter();
            return true;
        }
        else if(QKeySequence("Shift+?") == QKeySequence(uKey))
        {
            slotVolSyncReset();
            return true;
        }
        else if (QKeySequence(uKey) == QKeySequence(Qt::Key_Escape))
        {
            if (this->isFullScreen())
            {
                slotFullScreen();
            }
            else
            {
                showMinimized();
                Pause();
            }
            return true;
        }
        else if(QKeySequence(uKey) == QKeySequence(Qt::Key_Right))
        {
            slotForward();
            return true;
        }
        else if(QKeySequence(uKey) == QKeySequence(Qt::Key_Left))
        {
            slotBackward();
            return true;
        }
        else if(QKeySequence(uKey) == QKeySequence(Qt::Key_Up))
        {
            slotVolUp();
            return true;
        }
        else if(QKeySequence(uKey) == QKeySequence(Qt::Key_Down))
        {
            slotVolDown();
            return true;
        }
        else if (QKeySequence(uKey) == QKeySequence(Qt::Key_Return) || QKeySequence(uKey) == QKeySequence(Qt::Key_Enter))
        {
            slotFullScreen();
            return true;
        }
        else if(QKeySequence(uKey) == QKeySequence("Ctrl+Return") || QKeySequence(uKey) == QKeySequence("Ctrl+Enter"))
        {
            actFullScreen->setChecked(false);
            actFullScreen1->setChecked(true);
            slotFullScreen();
            return true;
        }
        else if(QKeySequence(uKey) == QKeySequence(Qt::Key_Z))
        {
            slotSpeedNormal();
            return true;
        }
        else if(QKeySequence(uKey) == QKeySequence(Qt::Key_C))
        {
            slotSpeedUp();
            return true;
        }
        else if(QKeySequence(uKey) == QKeySequence(Qt::Key_X))
        {
            slotSpeedDown();
            return true;
        }
        else if(QKeySequence(uKey) == QKeySequence(Qt::Key_M))
        {
            slotVol();
            return true;
        }
    }

    return QWidget::eventFilter(target, event);
}

void MainWidget::contextMenuEvent(QContextMenuEvent *event)
{
    // 在按钮上右击不响应菜单
    QPoint point =this->mapToGlobal(event->pos());
    QObjectList list = title_widget->children();
    list.append(ctrl_widget->children());

    QWidget *b;
    QString strClassName;
    QString btn = QString::fromLocal8Bit("PushButton");
    QString sld = QString::fromLocal8Bit("QSlider");
    foreach (QObject *obj, list)
    {
        b = nullptr;
        strClassName = obj->metaObject()->className();
        if( btn == strClassName || sld == strClassName)
        {
            b = qobject_cast<QWidget*>(obj);
        }
        else
        {
            continue;
        }
        if(b)
        {
            QPoint GlobalPoint(b->mapToGlobal(QPoint(0, 0)));//获取控件在窗体中的坐标
            QRect rc(GlobalPoint,b->size());
            if(rc.contains(point))
            {
               return;
            }
        }
    }

    QCursor cur = this->cursor();
    popMenu->exec(cur.pos());
}


void MainWidget::closeEvent(QCloseEvent *event)
{
    saveSettings();

    QWidget::closeEvent(event);
}


void MainWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    player_setrect(hplayer, 0, 0, 0, central_widget->width(), central_widget->height());
}


void MainWidget::slotAbout(void)
{
    about->exec();
}

void MainWidget::slotSpeedNormal(void)
{
    int speed = 100;
    player_setparam(hplayer, PARAM_PLAY_SPEED, &speed);
    actSpeedNormal->setChecked(true);
    actSpeedUp->setChecked(false);
    actSpeedDown->setChecked(false);
}

void MainWidget::slotSpeedUp(void)
{
    int speed = 0;
    player_getparam(hplayer, PARAM_PLAY_SPEED, &speed);
    if (speed >= 800)
        return;
    else
        speed += 10;

    slotSpeedChange(speed);
}

void MainWidget::slotSpeedDown(void)
{
    int speed = 0;
    player_getparam(hplayer, PARAM_PLAY_SPEED, &speed);
    if (speed <= 20)
        return;
    else
        speed -= 10;

    slotSpeedChange(speed);
}

void MainWidget::slotSpeedChange(int speed)
{
    player_setparam(hplayer, PARAM_PLAY_SPEED, &speed);
    if(speed == 100)
    {
        actSpeedNormal->setChecked(true);
        actSpeedUp->setChecked(false);
        actSpeedDown->setChecked(false);
    }
    else if(speed < 100)
    {
        actSpeedNormal->setChecked(false);
        actSpeedUp->setChecked(false);
        actSpeedDown->setChecked(true);
    }
    else if(speed > 100)
    {
        actSpeedNormal->setChecked(false);
        actSpeedUp->setChecked(true);
        actSpeedDown->setChecked(false);
    }
}

void MainWidget::slotBackward(void)
{
    int value = ctrl_widget->progress_slider->value()*1000 - 10 * 1000;
    value = value < 0 ? 0 : value;
    player_seek(hplayer, value);
}

void MainWidget::slotForward(void)
{
    int value = ctrl_widget->progress_slider->value()*1000 + 10 * 1000;
    value = value > m_duration ? m_duration : value;
    player_seek(hplayer, value);
}

void MainWidget::slotOpen(void)
{
    int nSize = m_VecFileNames.size();
    QString strPath = readPath();
    QStringList fileNameList;
    QFileDialog* fd = new QFileDialog(this,QString::fromLocal8Bit("请选择要播放的文件"),strPath,QString::fromLocal8Bit("音视频文件(*.*)"));//创建对话框
    fd->setFileMode(QFileDialog::ExistingFiles);
    if ( fd->exec() == QDialog::Accepted ) //如果成功的执行
    {
        fileNameList = fd->selectedFiles(); //返回文件列表的名称
        for(int i=0;i<fileNameList.size();i++)
        {
            m_VecFileNames.append(fileNameList[i]);
        }
    }
    else
    {
        fd->close();
        return;
    }

    QFileInfo fi(fileNameList[0]);
    writePath(fi.absolutePath());
    m_nIndex = nSize;
    Stop();
    Play(m_VecFileNames[m_nIndex]);
}

void MainWidget::slotVideoGDI(void)
{
    m_bGDI = true;
    actVideoGDI->setChecked(m_bGDI);
    actVideoD3D->setChecked(!m_bGDI);
    int mode = VDEV_RENDER_TYPE_GDI;
    player_setparam(hplayer, PARAM_VDEV_RENDER_TYPE, &mode);
}

void MainWidget::slotVideoD3D(void)
{
    m_bGDI = false;
    actVideoGDI->setChecked(m_bGDI);
    actVideoD3D->setChecked(!m_bGDI);
    int mode = VDEV_RENDER_TYPE_D3D;
    player_setparam(hplayer, PARAM_VDEV_RENDER_TYPE, &mode);
}

void MainWidget::customEvent(QEvent *event)
{
    if (event->type() == CustomEvent_Progress) //捕获消息
    {
        LONG64 position = g_pos;
        LONG64 duration = m_duration;
        QString strTime = QString::fromLocal8Bit(FormatString("%.2I64u:%.2I64u:%.2I64u/%.2I64u:%.2I64u:%.2I64u", position / 1000 / 60 / 60 % 60, position / 1000 / 60 % 60, position / 1000 % 60, duration / 1000 / 60 / 60 % 60, duration / 1000 / 60 % 60, duration / 1000 % 60).c_str());
        ctrl_widget->time_lable->setText(strTime);
        ctrl_widget->progress_slider->setValue((int)(position / 1000));
    }
    else if(event->type() == CustomEvent_Complete)
    {
        Next();
    }
}

void PlayCallback(__int32 msg, __int64 param)
{
    switch (msg)
    {
    case PLAY_PROGRESS:
        //该函数实现将自定义的消息发送到队列，且new QEvent(CustomEvent_Login))只能动态分配，postEvent会自动删除
        if(g_mainWidget->m_status == 1 || g_mainWidget->m_status == 2)
        {
            g_pos = param;
            QApplication::postEvent(g_mainWidget, new QEvent(CustomEvent_Progress));
        }

        break;
    case PLAY_COMPLETED:
        QApplication::postEvent(g_mainWidget, new QEvent(CustomEvent_Complete));

        break;
    default:
        break;
    }
}

void MainWidget::Play(const QString &strFilePath)
{
    QFileInfo fi(strFilePath);
    if(!fi.isFile())
    {
        return;
    }

    player_close(hplayer);
    if(this->isFullScreen())
        this->showNormal();
    hplayer = player_open((char *)strFilePath.toStdString().c_str(), (void *)central_widget->winId(), ADEV_RENDER_TYPE_WAVEOUT, m_bGDI ? VDEV_RENDER_TYPE_GDI : VDEV_RENDER_TYPE_D3D);
    player_setparam(hplayer, PARAM_PLAYER_CALLBACK, PlayCallback);
    player_setrect(hplayer, 0, 0, 0, central_widget->width(), central_widget->height());
    player_play(hplayer);    
    if (hplayer)
    {
        m_status = 1;
        //将中央窗口的大小设置为视频的宽高，如果视频尺寸太大，最大化显示
        int width = getVideoWidth();
        int height = getVideoHeight();
        QDesktopWidget *desktopWidget = QApplication::desktop();
        //获取可用桌面大小        
        QRect deskRect = desktopWidget->availableGeometry();
        int nWidth = (width <= deskRect.width()) ? width : deskRect.width();
        int nHeight = ((height + title_widget->height() + ctrl_widget->height()) <= deskRect.height()) ? (height + title_widget->height() + ctrl_widget->height()) : deskRect.height();
        int nLeft = deskRect.left();
        if (pos().x() > deskRect.left())
        {
            nLeft = ((pos().x() + nWidth) <= deskRect.right()) ? pos().x() : (deskRect.right() - nWidth + 1);
        }
        int nTop = deskRect.top();
        if (pos().y()> deskRect.top())
        {
            nTop = ((pos().y() + nHeight) <= deskRect.bottom()) ? pos().y() : (deskRect.bottom() - nHeight + 1);
        }
        setGeometry(nLeft, nTop, nWidth , nHeight);
        if(width >= deskRect.width())
        {
            title_widget->max_button->hide();
            title_widget->max_restore_button->show();
        }
        //视频时间显示初始化
        m_duration = getVideoDuration();
        ctrl_widget->progress_slider->setValue(0);
        ctrl_widget->progress_slider->setRange(0, (int)(m_duration / 1000));

        QString strTime = QString::fromLocal8Bit(FormatString("00:00:00/%.2I64u:%.2I64u:%.2I64u", m_duration / 1000 / 60 / 60 % 60, m_duration / 1000 / 60 % 60, m_duration / 1000 % 60).c_str());
        ctrl_widget->time_lable->setText(strTime);

        ctrl_widget->play_button->hide();
        ctrl_widget->pause_button->show();

        QString strTmp = strFilePath;
        title_widget->title->setText(strTmp.mid(strTmp.lastIndexOf("/") + 1));
    }
}


void MainWidget::Play(void)
{
    if(!hplayer)
    {
        slotOpen();
        return;
    }
    player_play(hplayer);
    m_status = 1;
    ctrl_widget->play_button->hide();
    ctrl_widget->pause_button->show();
}


void MainWidget::Pause(void)
{
    if(hplayer)
    {
        if(m_status == 1)
        {
            player_pause(hplayer);
            m_status = 2;
            ctrl_widget->pause_button->hide();
            ctrl_widget->play_button->show();
        }
    }
}


void MainWidget::Stop(void)
{
    if(hplayer)
    {
        player_close(hplayer);
        hplayer = nullptr;
        m_status = 3;
        g_pos = 0;
        m_duration = 0;
        ctrl_widget->progress_slider->setValue(0);
        ctrl_widget->time_lable->setText("00:00:00/00:00:00");
        title_widget->title->setText("");
    }
    central_widget->update();
}

void MainWidget::Previous(void)
{
    Stop();
    if (m_nIndex > 0)
    {
        m_nIndex--;
    }
    Play(m_VecFileNames.at(m_nIndex));
}

void MainWidget::Next(void)
{
    Stop();
    Sleep(200);
    if (m_nIndex < m_VecFileNames.size() - 1)
    {
        m_nIndex++;
    }
    Play(m_VecFileNames.at(m_nIndex));
}

void MainWidget::slotVol(void)
{
    static int volume = 0;
    if(bVol)
    {
        actVolOff->setChecked(true);
        ctrl_widget->vol_button->setPicName(QString(":/ctrl/vol_off"));
        volume = getVolume();
        ctrl_widget->vol_slider->setValue(0);
        setVolume(0);
        ctrl_widget->vol_button->setToolTip(QString::fromLocal8Bit("恢复音量"));
    }
    else
    {
        actVolOff->setChecked(false);
        ctrl_widget->vol_button->setPicName(QString(":/ctrl/vol"));
        ctrl_widget->vol_slider->setValue(volume);
        setVolume(volume);
        ctrl_widget->vol_button->setToolTip(QString::fromLocal8Bit("静音"));
    }
    bVol = !bVol;
}


void MainWidget::slotVolSync(int value)
{
    if(!hplayer)
        return;

    player_setparam(hplayer, PARAM_AVSYNC_TIME_DIFF, &value);
    if(value == 0)
    {
        actVolSyncReset->setChecked(true);
        actVolSyncAfter->setChecked(false);
        actVolSyncBefore->setChecked(false);
    }
    else if(value < 0)
    {
        actVolSyncAfter->setChecked(true);
        actVolSyncReset->setChecked(false);
        actVolSyncBefore->setChecked(false);
    }
    else if(value > 0)
    {
        actVolSyncBefore->setChecked(true);
        actVolSyncReset->setChecked(false);
        actVolSyncAfter->setChecked(false);
    }
}

//音频落后视频
void MainWidget::slotVolSyncAfter(void)
{
    int value = 0;
    player_getparam(hplayer, PARAM_AVSYNC_TIME_DIFF, &value);
    value -= 100;
    slotVolSync(value);
}

//音频超前视频
void MainWidget::slotVolSyncBefore(void)
{
    int value = 0;
    player_getparam(hplayer, PARAM_AVSYNC_TIME_DIFF, &value);
    value += 100;
    slotVolSync(value);
}

void MainWidget::slotVolSyncReset(void)
{
    slotVolSync(0);
}

void MainWidget::slotVolUp(void)
{
    int volume = getVolume();
    if(volume >= 95)
        volume = 100;
    else
        volume += 5;
    slotVolSeek(volume);
}

void MainWidget::slotVolDown(void)
{
    int volume = getVolume();
    if(volume <= 5)
        volume = 0;
    else
        volume -= 5;
    slotVolSeek(volume);
}

void MainWidget::slotVolSeek(int val)
{
    if(val <= 1)
    {
        bVol = false;
        actVolOff->setChecked(true);
        ctrl_widget->vol_button->setPicName(QString(":/ctrl/vol_off"));
        ctrl_widget->vol_button->setToolTip(QString::fromLocal8Bit("恢复音量"));
    }
    else
    {
        bVol = true;
        actVolOff->setChecked(false);
        ctrl_widget->vol_button->setPicName(QString(":/ctrl/vol"));
        ctrl_widget->vol_button->setToolTip(QString::fromLocal8Bit("静音"));
    }
    setVolume(val);
}

void MainWidget::setVolume(int val)
{
    int value = 0;
    if(val <= 100)
    {
        value = -182 + 182 * val / 100;
    }
    else
    {
        value = 73 * (val - 100) / 100;
    }
    player_setparam(hplayer, PARAM_AUDIO_VOLUME, &value);
    if(val >= 0 && val <= 100)
    {
        ctrl_widget->vol_slider->setValue(val);
    }
}


int MainWidget::getVolume()
{
    int value = ctrl_widget->vol_slider->value();
    return value;
}

void MainWidget::slotSeek(int value)
{
    //qDebug()<<"slotSeek";
    player_seek(hplayer, value*1000);
}


void MainWidget::slotFullScreen(void)
{
    if (this->isFullScreen())
    {
        if(actFullScreen->isChecked() && actFullScreen1->isChecked())
        {
            int mode =  -1;
            player_getparam(hplayer,PARAM_VIDEO_MODE,&mode);
            if(mode == VIDEO_MODE_STRETCHED)
            {
                actFullScreen->setChecked(true);
                actFullScreen1->setChecked(false);
                mode = VIDEO_MODE_LETTERBOX;
                player_setparam(hplayer,PARAM_VIDEO_MODE,&mode);
            }
            else
            {
                actFullScreen->setChecked(false);
                actFullScreen1->setChecked(true);
                mode = VIDEO_MODE_STRETCHED;
                player_setparam(hplayer,PARAM_VIDEO_MODE,&mode);
            }
        }
        else
        {
            this->showNormal();
        }
    }
    else
    {
        if(actFullScreen1->isChecked())
        {
            actFullScreen->setChecked(false);
            actFullScreen1->setChecked(true);
            int mode =  VIDEO_MODE_STRETCHED;
            player_setparam(hplayer,PARAM_VIDEO_MODE,&mode);
        }
        else
        {
            actFullScreen->setChecked(true);
            actFullScreen1->setChecked(false);
            int mode =  VIDEO_MODE_LETTERBOX;
            player_setparam(hplayer,PARAM_VIDEO_MODE,&mode);
        }

        this->showFullScreen();
    }
}

void MainWidget::slotRestoreFullScreen(void)
{
    if(actFullScreen->isChecked() && actFullScreen1->isChecked())
    {
        int mode =  -1;
        player_getparam(hplayer, PARAM_VIDEO_MODE, &mode);
        if(mode == VIDEO_MODE_STRETCHED)
        {
            actFullScreen->setChecked(true);
            actFullScreen1->setChecked(false);
            mode = VIDEO_MODE_LETTERBOX;
            player_setparam(hplayer, PARAM_VIDEO_MODE, &mode);
        }
        else
        {
            actFullScreen->setChecked(false);
            actFullScreen1->setChecked(true);
            mode = VIDEO_MODE_STRETCHED;
            player_setparam(hplayer, PARAM_VIDEO_MODE, &mode);
        }
    }
    else
    {
        this->showNormal();
    }
}


void MainWidget::showFullScreen()
{
    title_widget->full_button->hide();
    ctrl_widget->full_button->hide();
    title_widget->full_restore_button->show();
    ctrl_widget->full_restore_button->show();

    title_widget->hide();
    ctrl_widget->hide();

    QWidget::showFullScreen();
}

void MainWidget::showNormal()
{    
    title_widget->full_restore_button->hide();
    ctrl_widget->full_restore_button->hide();
    title_widget->full_button->show();
    ctrl_widget->full_button->show();

    title_widget->show();
    ctrl_widget->show();

    actFullScreen->setChecked(false);
    actFullScreen1->setChecked(false);

    int mode = VIDEO_MODE_LETTERBOX;
    player_setparam(hplayer, PARAM_VIDEO_MODE, &mode);

    QWidget::showNormal();
}

int MainWidget::getVideoWidth()
{
    int vw = 0;
    player_getparam(hplayer, PARAM_VIDEO_WIDTH, &vw);
    return vw;
}

int MainWidget::getVideoHeight()
{
    int vh = 0;
    player_getparam(hplayer, PARAM_VIDEO_HEIGHT, &vh);
    return vh;
}

int64_t MainWidget::getVideoDuration()
{
    int64_t total = 1;
    player_getparam(hplayer, PARAM_MEDIA_DURATION, &total);
    return total;
}

int64_t MainWidget::getVideoPosition()
{
    int64_t pos = 0;
    player_getparam(hplayer, PARAM_MEDIA_DURATION, &pos);
    return pos;
}

