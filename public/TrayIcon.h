#ifndef TRAYICON_H
#define TRAYICON_H

#include <QSystemTrayIcon>

class TrayIcon : public QSystemTrayIcon
{
	Q_OBJECT

	public:
		explicit TrayIcon(QObject* parent = nullptr);

	signals:
		void onClicked();

	private slots:
		void onActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif // TRAYICON_H