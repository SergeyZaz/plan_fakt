#ifndef ZEDITBASEFORM_H
#define ZEDITBASEFORM_H

#include <QDialog>
#include <QSqlDatabase>
#include <QDateTime>
#include <QComboBox>
#include <QSqlQuery>
#include "ui_zeditbaseform.h"

#define ADD_UNIC_CODE	-99

class ZEditAbstractForm : public QDialog
{
	Q_OBJECT
public:
	int						curEditId;
	QString					m_tbl;
	QSqlDatabase			m_DB;

	ZEditAbstractForm(QWidget *parent = 0, Qt::WFlags flags = 0):QDialog(parent, flags)
	{
		setModal(true);
	}
	virtual int init(QSqlDatabase &db, const QString &tbl, int id)
	{
		m_DB = db;
		m_tbl = tbl;
		curEditId = id;
		return 1;
	}
	virtual ~ZEditAbstractForm(){};
	virtual void setSectionsType(int){};
	void loadItemsToCombobox( QComboBox *comboBox, const QString &tableName, const QString &filter = "")
	{
		QString strQuery, txt = comboBox->currentText();

		// create query
		QSqlQuery query(m_DB);

		// clear box
		comboBox->clear();

		if(tableName == "sections")
			strQuery = QString("SELECT id, name, parent FROM %1").arg(tableName);
		else
			strQuery = QString("SELECT id, name FROM %1").arg(tableName);

		if(!filter.isEmpty())
			strQuery += QString(" WHERE %1").arg(filter);

		strQuery += " ORDER BY name";

		int result = query.exec(strQuery);
	
		if (result)
		{
			while (query.next()) 
			{
				strQuery = query.value(1).toString().simplified();
				if(tableName == "sections")	//0-������������ �����, 1-�������������� �����, 2-���������� �����
				{
					switch(query.value(2).toInt())
					{
					case 1:
						strQuery += tr(" (�������������� �����)");
						break;
					case 2:
						strQuery += tr(" (���������� �����)");
						break;
					default:
						strQuery += tr(" (������������ �����)");
						break;
					}
				}
				comboBox->addItem(strQuery, query.value(0).toInt());
			}
		}

		if(!txt.isEmpty())
			comboBox->setCurrentIndex(comboBox->findText(txt));
	}

signals:	
	void errorQuery(const QDateTime &, long , const QString &);
	void needUpdateVal(int);
};

class ZEditBaseForm : public ZEditAbstractForm
{
	Q_OBJECT
	Ui::EditBaseForm ui;
	int fNeedComment;
	void closeEvent(QCloseEvent *event);
public:
	int init(QSqlDatabase &db, const QString &tbl, int id);
	ZEditBaseForm(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ZEditBaseForm();
public slots:
	void applySlot();
	void textChangedSlot(const QString &);
};


#endif // ZEDITBASEFORM_H