#ifndef APPCORE_H
#define APPCORE_H

#include <QApplication>

class TrayIcon;
class TrayIconWindow;
class TaskBarWindow;
class QMLApplication;
class TreeItem;

class AppCore : public QObject
{
	Q_OBJECT

  public:
	explicit AppCore(QObject* parent = nullptr);
	~AppCore();

  public slots:
	void handleTrayIconClicked();
	void showTrayIconWindow(QPointF position);
	void handleSave();
	void handleQuit();
	void windowManagerReady();
	void handleAboutToQuit();

  private:
	TreeItem* loadModel(QString filename);
	void saveModel(QString filename, const TreeItem* nestedModel);
	void saveDefaultModel(QString filename);

	TrayIcon* m_trayIcon;

	TrayIconWindow* m_trayIconWindow;
	TaskBarWindow* m_taskBarWindow;

	QMLApplication* m_qmlApp;
	TreeItem* m_rootItem;
};

#endif // APPCORE_H