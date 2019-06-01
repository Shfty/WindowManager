#include "TreeIconImageProvider.h"

TreeIconImageProvider::TreeIconImageProvider()
	:QQuickImageProvider(QQuickImageProvider::Pixmap)
{

}

#include <QDebug>
QPixmap TreeIconImageProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
	QString url = id;
	url.replace("%5C", "/");

	QFileInfo fileInfo(url);

	QIcon icon = m_fileIconProvider.icon(fileInfo);
	QList<QSize> availableSizes = icon.availableSizes();
	QSize outSize;

	if(requestedSize.isValid())
	{
		qreal minReq = qMin(requestedSize.width(), requestedSize.height());

		// TODO: Find closest available size and assign here
		outSize = availableSizes.last();
	}
	else
	{
		// Use last (largest) size
		outSize = availableSizes.last();
	}

	size->setWidth(outSize.width());
	size->setHeight(outSize.height());

	return icon.pixmap(outSize);
}
