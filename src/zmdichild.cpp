#include <QCloseEvent>
#include "zmdichild.h"

ZMdiChild::ZMdiChild()
{
	setAttribute(Qt::WA_DeleteOnClose);

	m_tbl = new ZView(this);
	QGridLayout *pLayput = new QGridLayout(this);
	pLayput->addWidget(m_tbl);
	pLayput->setContentsMargins( 0, 0, 0, 0 );
	setLayout(pLayput); 
	
	connect(m_tbl, SIGNAL(needUpdate()), this, SIGNAL(needUpdate()));
}

void ZMdiChild::closeEvent(QCloseEvent *event)
{
	event->accept();
}

