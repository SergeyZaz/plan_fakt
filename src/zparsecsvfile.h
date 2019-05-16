#ifndef PARSECSVFILE_H
#define PARSECSVFILE_H

#include <QMap>

class ZParseCSVFile
{
	QString curSchet;
	bool fParseStart;
	QMap<QString, QString> mapData;
	void message(const QString& txt);
public:
	ZParseCSVFile(){fParseStart = false;}
	~ZParseCSVFile(){};
	int parseData(const QString &data);
	bool loadFile(const QString &fileName);
};

#endif
