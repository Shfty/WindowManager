#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QColor>

class SettingsContainer : public QObject
{
	Q_OBJECT

	Q_PROPERTY(qreal headerSize MEMBER m_headerSize NOTIFY headerSizeChanged)
	Q_PROPERTY(qreal itemMargin MEMBER m_itemMargin NOTIFY itemMarginChanged)
	Q_PROPERTY(qreal itemBorder MEMBER m_itemBorder NOTIFY itemBorderChanged)
	Q_PROPERTY(qreal itemRadius MEMBER m_itemRadius NOTIFY itemRadiusChanged)

	Q_PROPERTY(QColor colorInactiveHeader MEMBER m_colorInactiveHeader NOTIFY colorInactiveHeaderChanged)
	Q_PROPERTY(QColor colorActiveHeader MEMBER m_colorActiveHeader NOTIFY colorActiveHeaderChanged)
	Q_PROPERTY(QColor colorContainerPlaceholder MEMBER m_colorContainerPlaceholder NOTIFY colorContainerPlaceholderChanged)
	Q_PROPERTY(QColor colorContainerBorder MEMBER m_colorContainerBorder NOTIFY colorContainerBorderChanged)

public:
	explicit SettingsContainer(QObject* parent = nullptr);

	qreal getHeaderSize() const { return m_headerSize; }
	qreal getItemMargin() const { return m_itemMargin; }

signals:
	void headerSizeChanged();
	void itemMarginChanged();
	void itemBorderChanged();
	void itemRadiusChanged();

	void colorInactiveHeaderChanged();
	void colorActiveHeaderChanged();
	void colorContainerPlaceholderChanged();
	void colorContainerBorderChanged();

private:
	qreal m_headerSize;
	qreal m_itemMargin;
	qreal m_itemBorder;
	qreal m_itemRadius;

	QColor m_colorInactiveHeader;
	QColor m_colorActiveHeader;
	QColor m_colorContainerPlaceholder;
	QColor m_colorContainerBorder;
};

#endif // SETTINGS_H