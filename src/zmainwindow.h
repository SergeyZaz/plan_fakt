#ifndef ZMAINWINDOW_H
#define ZMAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include "ui_zmainwindow.h"


class ZMainWindow : public QMainWindow
{
	Q_OBJECT

	QSqlDatabase	db;
	Ui::ZMainWindow	ui;

public:
	ZMainWindow();
	~ZMainWindow();

protected:
	void	closeEvent(QCloseEvent *event);
	int		readIniFile();

private slots:
	void slotAbout();
	void slotOpenFile();
	void slotOpenCSVFile();
	void slotOpenAccountsDialog();
	void slotOpenPersonsDialog();
	void slotOpenSectionsDialog();
	void slotOpenPartnersDialog();
	void slotOpenProjectsDialog();
	void slotOpenProtokolDialog();
	void slotOpenOperationsDialog();
	void slotUpdate();

private:
	void readSettings();
	void writeSettings();
	void f();
};

unsigned long fZ();

#endif
