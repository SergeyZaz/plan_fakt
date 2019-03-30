#include <QApplication>
#include <QTextCodec>
#include "zmainwindow.h"
#ifdef _WIN32
	#include "windows.h"
#endif

int main(int argc, char *argv[])
{
	QTextCodec::setCodecForTr(QTextCodec::codecForName("windows-1251"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("windows-1251"));

#ifdef _WIN32
#ifdef _DEBUG
				AllocConsole();
				freopen("CONOUT$","wt", stdout);
				freopen("CONOUT$","wt", stderr);
				freopen("CONIN$","rt", stdin);
				GetStdHandle(STD_OUTPUT_HANDLE);
#endif
#endif
				
    QApplication app(argc, argv);

    ZMainWindow mainWin;
    mainWin.show();

    return app.exec();
}
