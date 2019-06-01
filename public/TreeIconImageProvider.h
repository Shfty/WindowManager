#ifndef TREEICONIMAGEPROVIDER_H
#define TREEICONIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QFileIconProvider>

class TreeIconImageProvider : public QQuickImageProvider
{
public:
	TreeIconImageProvider();

	virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override;

private:
	QFileIconProvider m_fileIconProvider;
};

#endif // TREEICONIMAGEPROVIDER_H
