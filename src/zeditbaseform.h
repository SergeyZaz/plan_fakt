#ifndef ZEDITBASEFORM_H
#define ZEDITBASEFORM_H

#include <QDialog>
#include <QSqlDatabase>
#include <QDateTime>
#include <QComboBox>
#include <QSqlQuery>
#include "ui_zeditbaseform.h"

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
	void loadItemsToCombobox( QComboBox *comboBox, const QString &tableName)
	{
		QString txt = comboBox->currentText();

		// create query
		QSqlQuery query(m_DB);

		// clear box
		comboBox->clear();
		auto result = query.exec(QString("SELECT id, name FROM %1 ORDER BY name").arg(tableName));
		if (result)
		{
			while (query.next()) 
			{
				comboBox->addItem(query.value(1).toString().simplified(), query.value(0).toInt());
			}
		}

		if(!txt.isEmpty())
			comboBox->setCurrentIndex(comboBox->findText(txt));
	}

signals:	
	void errorQuery(const QDateTime &, long , const QString &);
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