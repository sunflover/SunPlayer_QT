#include "main_widget.h"
#include "SingleApplication.h"


int main(int argc, char *argv[])
{
    QApplication::addLibraryPath("./plugins");

    //获取系统编码，否则移植会出现乱码
    QTextCodec *codec = QTextCodec::codecForName("System");
    //设置和对本地文件系统读写时候的默认编码格式
    QTextCodec::setCodecForLocale(codec);

    //单实例进程，或者说防止程序多开
    SingleApplication app(argc, argv);
    //fix ui bug
    app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    if (!app.isRunning())
    {
		MainWidget main_widget;
        //传入一个要激活程序的窗口，当多开时会激活已有进程的窗口，且多开失败
		app.mainWindow = &main_widget;
        app.installEventFilter(&main_widget);
        main_widget.show();

        return app.exec();
    }
    return 0;
}
