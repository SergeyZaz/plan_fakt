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
#include "zprotokol.h"

#define	CFG_FILE "config.ini"

ZMainWindow::ZMainWindow()
{
	ui.setupUi(this);

	connect(ui.actAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));
	connect(ui.actAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(ui.actOpenFile, SIGNAL(triggered()), this,	SLOT(slotOpenFile()));
	connect(ui.actAccounts, SIGNAL(triggered()), this,	SLOT(slotOpenAccountsDialog()));
	connect(ui.actPersons, SIGNAL(triggered()), this,	SLOT(slotOpenPersonsDialog()));
	connect(ui.actSections, SIGNAL(triggered()), this,	SLOT(slotOpenSectionsDialog()));
	connect(ui.actPartners, SIGNAL(triggered()), this,	SLOT(slotOpenPartnersDialog()));
	connect(ui.actProjects, SIGNAL(triggered()), this,	SLOT(slotOpenProjectsDialog()));
	connect(ui.actProtokol, SIGNAL(triggered()), this,	SLOT(slotOpenProtokolDialog()));
	connect(ui.actOperations, SIGNAL(triggered()), this,SLOT(slotOpenOperationsDialog()));

	readSettings();
	readIniFile();
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
	QMessageBox::about(this, tr("О программе"),
		tr("Программа для ведения учета денежных средств.<p>Версия 1.0.1. Автор: <a href=\"mailto:zaz@29.ru\">Zaz</a>"));
}

QString key, key_id;
#define GET_KEY(key) QCryptographicHash::hash("Zaz" + key.toLocal8Bit(), QCryptographicHash::Md5).toHex()
#define TRIAL_NUM_DAYS	30
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
		QMessageBox::critical(this, tr("Внимание"), tr("Закончился срок тестовой эксплуатации! \nЗарегистрируйте Вашу копию!"));
		exit(0);
	}

	if(lastModified.daysTo(QDate::currentDate()) > TRIAL_NUM_DAYS)
	{
		QMessageBox::critical(this, tr("Внимание"), tr("Закончился срок тестовой эксплуатации!"));
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
        QMessageBox::critical(0, tr("Ошибка"),
                              tr("Отсутствует конфигурационный файл: %1").arg(QDir::currentPath() + "/" + CFG_FILE));
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
        QMessageBox::critical(this, tr("Ошибка"), msg);
        return 0;
    }

	f();
    return 1;
}
	
void ZMainWindow::slotUpdate()
{
	//обновление открытых окон
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
	QString fileName = QFileDialog::getOpenFileName(this);
	if (fileName.isEmpty()) 
		return;
	ZParseFile pFile;
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

