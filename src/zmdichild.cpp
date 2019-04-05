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

void ZMdiChild::setContextMenuForTbl(const QStringList &items)
{
	QList<QAction *> contextMnuActions;

	foreach(QString item, items)
	{
		QAction *pAct = new QAction(item, m_tbl->getTable());
		contextMnuActions.append(pAct);
		connect(pAct, SIGNAL(triggered()), this, SLOT(slotCustomActionExec()));
	}
	m_tbl->getTable()->setContextMenuPolicy(Qt::ActionsContextMenu);
	m_tbl->getTable()->addActions(contextMnuActions);
}

void ZMdiChild::slotCustomActionExec()
{
	QAction *pAct = dynamic_cast<QAction*>(sender());
	if(pAct)
		execCustomAction(pAct->text());
}