#ifndef WINDOWINFO_H
#define WINDOWINFO_H

#include <QObject>
#include <QString>
#include "Win.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(windowInfo);

struct WindowInfoInternal {
	WindowInfoInternal(HWND inHwnd, QString inTitle, QString inClass, QString inProcess, qint32 inStyle)
	{
		hwnd = inHwnd;
		winTitle = inTitle;
		winClass = inClass;
		winProcess = inProcess;
		winStyle = inStyle;
	}

	HWND hwnd;
	QString winTitle;
	QString winClass;
	QString winProcess;
	qint32 winStyle;
};

class WindowInfo : public QObject
{
	Q_OBJECT

	Q_PROPERTY(HWND hwnd READ getHwnd WRITE setHwnd NOTIFY hwndChanged)
	Q_PROPERTY(QString winTitle READ getWinTitle WRITE setWinTitle NOTIFY winTitleChanged)
	Q_PROPERTY(QString winClass READ getWinClass WRITE setWinClass NOTIFY winClassChanged)
	Q_PROPERTY(QString winProcess READ getWinProcess WRITE setWinProcess NOTIFY winProcessChanged)
	Q_PROPERTY(qint32 winStyle READ getWinStyle WRITE setWinStyle NOTIFY winStyleChanged)

public:
	explicit WindowInfo(QObject* parent = nullptr);

	HWND getHwnd() const { return m_hwnd; }
	QString getWinTitle() const { return m_winTitle; }
	QString getWinClass() const { return m_winClass; }
	QString getWinProcess() const { return m_winProcess; }
	qint32 getWinStyle() const { return m_winStyle; }

	void setHwnd(HWND newHwnd);
	void setWinTitle(QString newTitle);
	void setWinClass(QString newClass);
	void setWinProcess(QString newProcess);
	void setWinStyle(qint32 newStyle);

signals:
	void hwndChanged();
	void winTitleChanged();
	void winClassChanged();
	void winProcessChanged();
	void winStyleChanged();

	void windowClosed();

private:
	HWND m_hwnd;
	QString m_winTitle;
	QString m_winClass;
	QString m_winProcess;
	qint32 m_winStyle;
};

Q_DECLARE_METATYPE(WindowInfo*)
Q_DECLARE_METATYPE(const WindowInfo*)

#endif // WINDOWINFO_H
