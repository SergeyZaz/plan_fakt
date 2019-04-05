#include <QApplication>
#include <QTextCodec>
#include "zmainwindow.h"
#ifdef _WIN32
	#include "windows.h"
#endif

unsigned long fZ()
{
	WCHAR dd[4];
	dd[1] = ':'; 
	dd[2] = '\\'; 
	dd[3] = 0;
	DWORD dr = GetLogicalDrives();
	for( int i = 0; i < 26; i++ )
	{
		if(((dr>>i)&0x00000001)) 
		{
			dd[0] = char(65+i); 
			if(GetDriveType(dd) == DRIVE_FIXED)
			{
				unsigned long n;
				if (GetVolumeInformation(dd, 0, 0, &n, 0, 0, 0, 0))
				{
					return n;
				}
			}
		}
	}
	return -1;
}

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
