#ifndef APPCORE_H
#define APPCORE_H

#include <QApplication>

class TrayIcon;
class TrayIconWindow;
class TaskBarWindow;
class QMLApplication;
class TreeItem;
class QQmlEngine;
class QQuickItem;
class QQuickWindow;

class AppCore : public QObject
{
	Q_OBJECT

public:
	explicit AppCore(QObject* parent = nullptr);
	~AppCore();

public slots:
	void showTrayIconWindow(QPointF position);
	void shutdown();
	void restart();
	void sleep();

	void handleSave();
	void handleQuit();

	void windowManagerReady();
	void handleAboutToQuit();

private:
	TreeItem* loadModel(QString filename);
	void saveModel(QString filename, const TreeItem* nestedModel);
	void saveDefaultModel(QString filename);

	void elevatePrivileges();

	TrayIcon* m_trayIcon;

	TrayIconWindow* m_trayIconWindow;
	TaskBarWindow* m_taskBarWindow;
	
	TreeItem* m_rootItem;
};

#endif // APPCORE_H