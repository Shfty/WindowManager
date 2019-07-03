#ifndef WINDOWINFO_H
#define WINDOWINFO_H

#include <QObject>
#include <QString>
#include "Win.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(windowInfo);

struct WindowInfo {
	WindowInfo(HWND inHwnd = nullptr,
			   QString inTitle = QString(),
			   QString inClass = QString(),
			   QString inProcess = QString(),
			   qint32 inStyle = 0)
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

Q_DECLARE_METATYPE(WindowInfo)

QDataStream& operator <<(QDataStream&, const WindowInfo&);
QDataStream& operator >>(QDataStream&, WindowInfo&);
QTextStream& operator <<(QTextStream&, const WindowInfo&);
QDebug operator <<(QDebug, const WindowInfo&);
bool operator ==(const WindowInfo&, const WindowInfo&);
bool operator <(const WindowInfo&, const WindowInfo&);

class WindowObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(HWND hwnd READ getHwnd WRITE setHwnd NOTIFY hwndChanged)
	Q_PROPERTY(QString winTitle READ getWinTitle WRITE setWinTitle NOTIFY winTitleChanged)
	Q_PROPERTY(QString winClass READ getWinClass WRITE setWinClass NOTIFY winClassChanged)
	Q_PROPERTY(QString winProcess READ getWinProcess WRITE setWinProcess NOTIFY winProcessChanged)
	Q_PROPERTY(qint32 winStyle READ getWinStyle WRITE setWinStyle NOTIFY winStyleChanged)

public:
	explicit WindowObject(QObject* parent = nullptr);
	explicit WindowObject(WindowInfo wi, QObject* parent = nullptr);

	HWND getHwnd() const { return m_windowInfo.hwnd; }
	QString getWinTitle() const { return m_windowInfo.winTitle; }
	QString getWinClass() const { return m_windowInfo.winClass; }
	QString getWinProcess() const { return m_windowInfo.winProcess; }
	qint32 getWinStyle() const { return m_windowInfo.winStyle; }
	WindowInfo getWindowInfo() const { return m_windowInfo; }

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
	WindowInfo m_windowInfo;
};

Q_DECLARE_METATYPE(WindowObject*)
Q_DECLARE_METATYPE(const WindowObject*)

#endif // WINDOWINFO_H
