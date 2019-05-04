#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QColor>

class Settings : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString wallpaperUrl MEMBER m_wallpaperUrl NOTIFY wallpaperUrlChanged)
	Q_PROPERTY(int wallpaperFillMode MEMBER m_wallpaperFillMode NOTIFY wallpaperFillModeChanged)

	Q_PROPERTY(qreal headerSize MEMBER m_headerSize NOTIFY headerSizeChanged)
	Q_PROPERTY(qreal itemMargin MEMBER m_itemMargin NOTIFY itemMarginChanged)

	Q_PROPERTY(QColor colorInactiveHeader MEMBER m_colorInactiveHeader NOTIFY colorInactiveHeaderChanged)
	Q_PROPERTY(QColor colorActiveHeader MEMBER m_colorActiveHeader NOTIFY colorActiveHeaderChanged)
	Q_PROPERTY(QColor colorContainerPlaceholder MEMBER m_colorContainerPlaceholder NOTIFY colorContainerPlaceholderChanged)

public:
	explicit Settings(QObject* parent = nullptr);
	~Settings();

	static Settings& instance()
	{
		static Settings inst;
		return inst;
	}

	qreal getHeaderSize() const { return m_headerSize; }
	qreal getItemMargin() const { return m_itemMargin; }

signals:
	void wallpaperUrlChanged();
	void wallpaperFillModeChanged();

	void headerSizeChanged();
	void itemMarginChanged();

	void colorInactiveHeaderChanged();
	void colorActiveHeaderChanged();
	void colorContainerPlaceholderChanged();

private:
	QString m_wallpaperUrl;
	int m_wallpaperFillMode;

	qreal m_headerSize;
	qreal m_itemMargin;

	QColor m_colorInactiveHeader;
	QColor m_colorActiveHeader;
	QColor m_colorContainerPlaceholder;
};

#endif // SETTINGS_H