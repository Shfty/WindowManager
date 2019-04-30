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
		void onQuitTriggered();

	private slots:
		void onActivated(QSystemTrayIcon::ActivationReason reason);

	private:
		QMenu* m_menu;
};

#endif // TRAYICON_H