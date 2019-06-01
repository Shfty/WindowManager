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

		std::sort(availableSizes.begin(), availableSizes.end(), [=](const QSize& lhs, const QSize& rhs) -> bool {
			qreal minLhs = qMin(lhs.width(), lhs.height());
			qreal minRhs = qMin(rhs.width(), rhs.height());

			qreal deltaLhs = qAbs(minReq - minLhs);
			qreal deltaRhs = qAbs(minReq - minRhs);

			return deltaLhs > deltaRhs;
		});
	}

	outSize = availableSizes.last();

	size->setWidth(outSize.width());
	size->setHeight(outSize.height());

	return icon.pixmap(outSize);
}
