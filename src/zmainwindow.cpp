#include <QMessageBox>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDir>
#include <QTextCodec>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMdiSubWindow>
#include <QFileInfo>
#include <QCryptographicHash>
#include "zmainwindow.h"
#include "zurpersons.h"
#include "zsections.h"
#include "zprojects.h"
#include "zpartners.h"
#include "zaccounts.h"
#include "zoperations.h"
#include "zparsefile.h"
#include "zparsecsvfile.h"
#include "zprotokol.h"

#define	CFG_FILE "config.ini"

ZMainWindow::ZMainWindow()
{
	ui.setupUi(this);

	connect(ui.actAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));
	connect(ui.actAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(ui.actOpenFile, SIGNAL(triggered()), this,	SLOT(slotOpenFile()));
	connect(ui.actOpenFileCSV, SIGNAL(triggered()), this,	SLOT(slotOpenCSVFile()));
	connect(ui.actAccounts, SIGNAL(triggered()), this,	SLOT(slotOpenAccountsDialog()));
	connect(ui.actPersons, SIGNAL(triggered()), this,	SLOT(slotOpenPersonsDialog()));
	connect(ui.actSections, SIGNAL(triggered()), this,	SLOT(slotOpenSectionsDialog()));
	connect(ui.actPartners, SIGNAL(triggered()), this,	SLOT(slotOpenPartnersDialog()));
	connect(ui.actProjects, SIGNAL(triggered()), this,	SLOT(slotOpenProjectsDialog()));
	connect(ui.actProtokol, SIGNAL(triggered()), this,	SLOT(slotOpenProtokolDialog()));
	connect(ui.actOperations, SIGNAL(triggered()), this,SLOT(slotOpenOperationsDialog()));

	readSettings();
	if(readIniFile()==0)
		exit(0);
}

ZMainWindow::~ZMainWindow()
{
}

void ZMainWindow::closeEvent(QCloseEvent *event)
{
	ui.mdiArea->closeAllSubWindows();
	if (ui.mdiArea->currentSubWindow()) 
	{
		event->ignore();
	} 
	else 
	{
		writeSettings();
		event->accept();
	}
}

void ZMainWindow::slotAbout()
{
	QMessageBox::about(this, tr("� ���������"),
		tr("��������� ��� ������� ����� �������� �������.<p>������ 2.0.1. (������: %1 %2) �����: <a href=\"mailto:zaz@29.ru\">Zaz</a>")
		.arg( __DATE__ ).arg( __TIME__ ));
}

QString key, key_id;
#define GET_KEY(key) QCryptographicHash::hash("Zaz" + key.toLocal8Bit(), QCryptographicHash::Md5).toHex()
#define TRIAL_NUM_DAYS	180
void ZMainWindow::f()
{
	if(key_id == GET_KEY(key))
		return;

	QSqlQuery query;
	QDate lastDate = QDate::currentDate();
	if (query.exec(QString("SELECT date FROM operations ORDER BY date DESC LIMIT 1")))
	{
		while (query.next()) 
		{
			lastDate = query.value(0).toDate();
			break;
		}
	}	

	QFileInfo file(QApplication::arguments().at(0));
	QDate lastModified = file.lastModified().date();

	if(lastModified.daysTo(lastDate) > TRIAL_NUM_DAYS)
	{
		QMessageBox::critical(this, tr("��������"), tr("���������� ���� �������� ������������! \n��������������� ���� �����!"));
		exit(0);
	}

	if(lastModified.daysTo(QDate::currentDate()) > TRIAL_NUM_DAYS)
	{
		QMessageBox::critical(this, tr("��������"), tr("���������� ���� �������� ������������!"));
		exit(0);
	}
}

void ZMainWindow::readSettings()
{
	QSettings settings("Zaz", "PlanFact");
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(640, 480)).toSize();

	key = QString::number(fZ());
	if(settings.value("key", "").toString() != key)
	{
		settings.setValue("key", key);
	}
	key_id = settings.value("id", "").toString();

	move(pos);
	resize(size);
}

void ZMainWindow::writeSettings()
{
	QSettings settings("Zaz", "PlanFact");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
}

int ZMainWindow::readIniFile()
{
    if (!QFile::exists(CFG_FILE)) 
    {
        QMessageBox::critical(0, tr("������"),
                              tr("����������� ���������������� ����: %1").arg(QDir::currentPath() + "/" + CFG_FILE));
        return 0;
    }

    QSettings settings(CFG_FILE, QSettings::IniFormat);

    db = QSqlDatabase::addDatabase(settings.value("Database/driver").toString());
    db.setDatabaseName(settings.value("Database/dbname").toString());
    db.setUserName(settings.value("Database/user").toString());
    db.setPassword(settings.value("Database/pwd").toString());
    db.setHostName(settings.value("Database/host").toString());
    db.setPort(settings.value("Database/port").toInt());

    db.setConnectOptions("connect_timeout=10"); 
    if (!db.open()) 
    {
		QString msg = db.lastError().text();
		msg = QTextCodec::codecForName("UTF-8")->toUnicode(QTextCodec::codecForName("windows-1251")->fromUnicode(msg));
        QMessageBox::critical(this, tr("������"), msg);
        return 0;
    }

	f();
    return 1;
}
	
void ZMainWindow::slotUpdate()
{
	//���������� �������� ����
	foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) 
	{
		ZMdiChild *pChild = dynamic_cast<ZMdiChild *>(window->widget());
		if (pChild)
		{
			pChild->reload();
		}
	}
}

void ZMainWindow::slotOpenFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, "����� ����� ��� �������", "", "��������� ����� (*.txt);;��� ����� (*.*)");
	if (fileName.isEmpty()) 
		return;
	ZParseFile pFile;
	if(pFile.loadFile(fileName))
	{
		slotUpdate();
	}
}

void ZMainWindow::slotOpenCSVFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, "����� ����� ��� �������", "", "CSV-����� (*.csv);;��� ����� (*.*)");
	if (fileName.isEmpty()) 
		return;
	ZParseCSVFile pFile;
	if(pFile.loadFile(fileName))
	{
		slotUpdate();
	}
}

void ZMainWindow::slotOpenAccountsDialog()
{
	foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) 
	{
		if (dynamic_cast<ZAccounts *>(window->widget()))
		{
			ui.mdiArea->setActiveSubWindow(window);
			return;
		}
	}

	ZMdiChild *child = new ZAccounts;
	connect(child, SIGNAL(needUpdate()), this,SLOT(slotUpdate()));
	ui.mdiArea->addSubWindow(child);
	child->setWindowTitleAndIcon(ui.actAccounts->text(), ui.actAccounts->icon());
	child->initDB(db, "accounts");
	child->show();
}

void ZMainWindow::slotOpenPersonsDialog()
{
	foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) 
	{
		if (dynamic_cast<ZUrPersons *>(window->widget()))
		{
			ui.mdiArea->setActiveSubWindow(window);
			return;
		}
	}

	ZMdiChild *child = new ZUrPersons;
	connect(child, SIGNAL(needUpdate()), this,SLOT(slotUpdate()));
	ui.mdiArea->addSubWindow(child);
	child->setWindowTitleAndIcon(ui.actPersons->text(), ui.actPersons->icon());
	child->initDB(db, "ur_persons");
	child->show();
}

void ZMainWindow::slotOpenSectionsDialog()
{
	foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) 
	{
		if (dynamic_cast<ZSections *>(window->widget()))
		{
			ui.mdiArea->setActiveSubWindow(window);
			return;
		}
	}

	ZMdiChild *child = new ZSections;
	connect(child, SIGNAL(needUpdate()), this,SLOT(slotUpdate()));
	ui.mdiArea->addSubWindow(child);
	child->setWindowTitleAndIcon(ui.actSections->text(), ui.actSections->icon());
	child->initDB(db, "sections");
	child->show();
}

void ZMainWindow::slotOpenPartnersDialog()
{
	foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) 
	{
		if (dynamic_cast<ZPartners *>(window->widget()))
		{
			ui.mdiArea->setActiveSubWindow(window);
			return;
		}
	}

	ZMdiChild *child = new ZPartners;
	connect(child, SIGNAL(needUpdate()), this,SLOT(slotUpdate()));
	ui.mdiArea->addSubWindow(child);
	child->setWindowTitleAndIcon(ui.actPartners->text(), ui.actPartners->icon());
	child->initDB(db, "partners");
	child->show();
}

void ZMainWindow::slotOpenProjectsDialog()
{
	foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) 
	{
		if (dynamic_cast<ZProjects *>(window->widget()))
		{
			ui.mdiArea->setActiveSubWindow(window);
			return;
		}
	}

	ZMdiChild *child = new ZProjects;
	connect(child, SIGNAL(needUpdate()), this,SLOT(slotUpdate()));
	ui.mdiArea->addSubWindow(child);
	child->setWindowTitleAndIcon(ui.actProjects->text(), ui.actProjects->icon());
	child->initDB(db, "projects");
	child->show();
}

void ZMainWindow::slotOpenOperationsDialog()
{
	foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) 
	{
		if (dynamic_cast<ZOperations *>(window->widget()))
		{
			ui.mdiArea->setActiveSubWindow(window);
			return;
		}
	}

	ZMdiChild *child = new ZOperations;
	connect(child, SIGNAL(needUpdate()), this,SLOT(slotUpdate()));
	connect(child, SIGNAL(needUpdateVal(int)), this,SLOT(slotUpdateAccountsVal(int)));
	ui.mdiArea->addSubWindow(child);
	child->setWindowTitleAndIcon(ui.actOperations->text(), ui.actOperations->icon());
	child->initDB(db, "operations");
	child->show();
}

void ZMainWindow::slotOpenProtokolDialog()
{
	ZProtokol *pD = new ZProtokol(this);
	pD->show();
}

void ZMainWindow::slotUpdateAccountsVal(int account_id)
{
	QSqlQuery query, query2;
	int id;
	double val;
	QString stringQuery = "SELECT id FROM accounts";

	if(account_id!=-1)
		stringQuery += QString(" WHERE id=%1").arg(account_id);
	
	if (query.exec(stringQuery))
	{
		while (query.next()) 
		{
			id = query.value(0).toInt();
			val = 0;

			for(int i=0;i<2;i++)
			{
				//stringQuery = QString("SELECT sum(val) FROM operations WHERE type=%1 AND account=%2")
				stringQuery = QString("SELECT sum(val) FROM operations WHERE section IN (SELECT id FROM sections WHERE type=%1) AND account=%2")
					.arg(i) //���: 0-�����������/1-�������/2-�����������
					.arg(id);
				if(query2.exec(stringQuery))
				{
					while (query2.next()) 
					{
						if(i==0)
							val += query2.value(0).toDouble();
						else
							val -= query2.value(0).toDouble();
					}
				}
				else
				{
				}
			}
			
			stringQuery = QString("UPDATE accounts SET val=%1 WHERE id=%2").arg(val, 0, 'f', 2).arg(id);
			if(!query2.exec(stringQuery))
				QMessageBox::critical(this, tr("������"), query2.lastError().text());
		}
	}	
	else
	{
		QMessageBox::critical(this, tr("������"), query.lastError().text());
	}
}

