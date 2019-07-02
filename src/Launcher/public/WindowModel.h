#ifndef WINDOWEVENTMODEL_H
#define WINDOWEVENTMODEL_H

#include <QObject>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(windowModel);

#include <Win.h>

class WindowModel : public QObject
{
	Q_OBJECT

public:
	explicit WindowModel(QObject* parent);
	~WindowModel();

	static WindowModel* instance;

signals:
	void startupComplete();

	void windowCreated(HWND window, QString winTitle, QString winClass, QString winProcess, qint32 winStyle);
	void windowRenamed(HWND hwnd, QString winTitle);
	void windowDestroyed(HWND hwnd);

public slots:
	void startup();

	void handleWindowCreated(HWND hwnd);
	void handleWindowRenamed(HWND hwnd);
	void handleWindowDestroyed(HWND hwnd);

protected:
	HWINEVENTHOOK hookEvent(DWORD event, WINEVENTPROC callback);

private:
	QString getWinTitle(HWND hwnd);
	QString getWinClass(HWND hwnd);
	QString getWinProcess(HWND hwnd);
	qint32 getWinStyle(HWND hwnd);

private:
	HWINEVENTHOOK m_createHook;
	HWINEVENTHOOK m_renameHook;
	HWINEVENTHOOK m_destroyHook;

	QMap<HWND, QString> m_windowList;
};

#endif // WINDOWEVENTMODEL_H
