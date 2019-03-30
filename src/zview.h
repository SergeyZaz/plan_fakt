#ifndef ZVIEW_H
#define ZVIEW_H

#include <QtGui/QWidget>
#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QPointer>
#include "ui_zview.h"
#include "stdio.h"
#include "zeditbaseform.h"

#define SORT_ROLE	Qt::UserRole+99

class ZTableModel :  public QSqlRelationalTableModel
{
public:
	ZTableModel(QObject * parent = 0, QSqlDatabase db = QSqlDatabase());
	~ZTableModel();
	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	Qt::ItemFlags flags ( const QModelIndex & index ) const;
	int m_PhotoColumn;
	int m_HexColumn;
	int m_HighlightColumn;
	QList<int> *pHighlightItems;
	QMap< int , QMap<int, QString>* > relMaps;
	bool fEdit;
};

class ZSortFilterProxyModel : public QSortFilterProxyModel
{
	bool lessThan ( const QModelIndex & left, const QModelIndex & right ) const;
public:
	ZSortFilterProxyModel(){};
	~ZSortFilterProxyModel(){};
};

class ZView : public QWidget
{
	Q_OBJECT

	QSqlDatabase			m_DB;
	ZSortFilterProxyModel	sortModel;
	QSqlQueryModel			*model;
	QPointer<ZEditAbstractForm>		pEditForm;
	QString					mTable;

public:
	ZView(QWidget *parent = 0, Qt::WFlags flags = 0);
	ZView(QSqlDatabase &DB, QWidget *parent = 0, Qt::WFlags flags = 0);
	~ZView();
	void init();
	void setDatabase(QSqlDatabase &DB);
	void setColorHighligthIfColumnContain(int col, QList<int> *plist);
	int setTable(const QString &tbl, QStringList &headers, QList<int> &removeColumns);
	void setTable(const QString &tbl) { mTable=tbl;}
	int setQuery(const QString &query, QStringList &headers, bool fRemoveOldModel = true);
	int init(QList<int> &hideColumns, int sortCol = 1);
	void setRelation(int column, const QString &tbl, const QString &attId, const QString &attValue);
	void setRelation(int column, QMap<int, QString> *relMap);
	void updateAndShow(bool fMaximized = false);
	void moveSection(int from, int to);
	void setPhotoColumn(int col);
	void setHexColumn(int col);
	void setFilter(const QString & filter);
	void setCustomEditor(ZEditAbstractForm *pD);
	void setReadOnly(bool fEdit);
	void setVisiblePrint(bool fTrue);
	QItemSelectionModel *selectionModel();
	QSqlQueryModel *getModel() { return model;}
	//ZSortFilterProxyModel *getSortModel() { return &sortModel;}
	QTableView *getTable() { return ui.tbl;}
	//bool loadView(int sortCol);

private:
	Ui::ZViewClass ui;
	bool eventFilter(QObject *obj, QEvent *event);
	int openEditor(int id=-1);
	void update();
public slots:
	void add();
	void del();
	void edit();
	void changeFilter(const QString &);
	void changeFilter(int);
	void clickedTbl(const QModelIndex &);
	void doubleClickedTbl(const QModelIndex &);
	void PrintCards();
	void reload();
	void applyEditor();
signals:	
	void errorQuery(const QDateTime &, long , const QString &);
	void setCurrentElem( QEvent::Type, int );
	void needUpdate();
};

#endif // ZVIEW_H
