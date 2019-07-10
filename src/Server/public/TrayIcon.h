#ifndef TRAYICON_H
#define TRAYICON_H

#include <QObject>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(trayIcon)

class QSystemTrayIcon;

class TrayIcon : public QObject
{
	Q_OBJECT

public:
	explicit TrayIcon(QObject* parent = nullptr);

public slots:
	void startup();

private:
	QSystemTrayIcon* m_trayIcon;
};

#endif // TRAYICON_H
