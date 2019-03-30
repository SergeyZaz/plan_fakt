#ifndef ZPROTOKOL_H
#define ZPROTOKOL_H

#include <QDialog>
#include "ui_zprotokol.h"

class ZProtokol : public QDialog
{
	Q_OBJECT
	
	void loadItemsToListBox( QListWidget *listBox, const QString &tableName);
	int saveToFile(const QString &fileName);

public:
	ZProtokol(QWidget *parent = 0);
	~ZProtokol();

public slots:
	void buildProtokol();
	void saveProtokol();

private:
	Ui::ZProtokol ui;
};

#endif // ZPROTOKOL_H
