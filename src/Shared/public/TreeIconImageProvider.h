#ifndef TREEICONIMAGEPROVIDER_H
#define TREEICONIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QFileIconProvider>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(treeIconImageProvider);

class TreeIconImageProvider : public QQuickImageProvider
{
public:
	TreeIconImageProvider();

	virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override;

private:
	QFileIconProvider m_fileIconProvider;
	QPixmap m_emptyPixmap;
};

#endif // TREEICONIMAGEPROVIDER_H
