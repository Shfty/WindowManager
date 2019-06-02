#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QThread>

class WinShellController;
class WindowController;
class WindowView;
class TreeItem;
class SettingsContainer;
class QmlController;
class QQuickWindow;
class QQuickItem;
class QWindow;

class AppCore : public QObject
{
	Q_OBJECT

	Q_PROPERTY(WinShellController* winShellController MEMBER m_winShellController NOTIFY winShellControllerChanged)
	Q_PROPERTY(WindowController* windowController MEMBER m_windowController NOTIFY windowControllerChanged)
	Q_PROPERTY(WindowView* windowView MEMBER m_windowView NOTIFY windowViewChanged)
	Q_PROPERTY(SettingsContainer* settingsContainer MEMBER m_settingsContainer NOTIFY settingsContainerChanged)
	Q_PROPERTY(QmlController* qmlController MEMBER m_qmlController NOTIFY qmlControllerChanged)
	Q_PROPERTY(TreeItem* treeModel MEMBER m_rootItem NOTIFY treeModelChanged)

	Q_PROPERTY(QQuickItem* configOverlay READ getConfigOverlay() NOTIFY configOverlayChanged)
	Q_PROPERTY(QQuickItem* windowListOverlay READ getWindowListOverlay() NOTIFY windowListOverlayChanged)
	Q_PROPERTY(QQuickItem* powerMenuOverlay READ getPowerMenuOverlay() NOTIFY powerMenuOverlayChanged)
	Q_PROPERTY(QQuickItem* itemSettingsOverlay READ getItemSettingsOverlay() NOTIFY itemSettingsOverlayChanged)

public:
	explicit AppCore(QObject* parent = nullptr);

	QQuickItem* getConfigOverlay();
	QQuickItem* getWindowListOverlay();
	QQuickItem* getPowerMenuOverlay();
	QQuickItem* getItemSettingsOverlay();

signals:

	void winShellControllerChanged();
	void windowControllerChanged();
	void windowViewChanged();
	void settingsContainerChanged();
	void qmlControllerChanged();
	void treeModelChanged();

	void configOverlayChanged();
	void windowListOverlayChanged();
	void powerMenuOverlayChanged();
	void itemSettingsOverlayChanged();

public slots:
	void windowManagerReady();

	void save();

	void shutdown();
	void restart();
	void sleep();

private:
	void elevatePrivileges();

	TreeItem* loadModel(QString filename);
	void saveModel(QString filename, const TreeItem* nestedModel);
	void saveDefaultModel(QString filename);

	QThread m_windowControllerThread;

	WindowController* m_windowController;
	WindowView* m_windowView;
	WinShellController* m_winShellController;
	SettingsContainer* m_settingsContainer;
	QmlController* m_qmlController;
	TreeItem* m_rootItem;

	QQuickWindow* m_configOverlay;
	QQuickWindow* m_windowListOverlay;
	QQuickWindow* m_powerMenuOverlay;
	QQuickWindow* m_itemSettingsOverlay;
};

#endif // APPCORE_H