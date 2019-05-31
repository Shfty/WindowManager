#ifndef WINDOWINFO_H
#define WINDOWINFO_H

#include <QObject>
#include <QString>
#include "Win.h"

class WindowInfo : public QObject
{
	Q_OBJECT

	Q_PROPERTY(HWND hwnd MEMBER m_hwnd NOTIFY hwndChanged)
	Q_PROPERTY(QString winTitle MEMBER m_winTitle NOTIFY winTitleChanged)
	Q_PROPERTY(QString winClass MEMBER m_winClass NOTIFY winClassChanged)
	Q_PROPERTY(QString winProcess MEMBER m_winProcess NOTIFY winProcessChanged)

public:
	explicit WindowInfo(QObject* parent = nullptr);

	HWND getHwnd() const { return m_hwnd; }
	QString getWinTitle() const { return m_winTitle; }
	QString getWinClass() const { return m_winClass; }
	QString getWinProcess() const { return m_winProcess; }

signals:
	void hwndChanged();
	void winTitleChanged();
	void winClassChanged();
	void winProcessChanged();

private:
	HWND m_hwnd;
	QString m_winTitle;
	QString m_winClass;
	QString m_winProcess;
};

Q_DECLARE_METATYPE(WindowInfo*)
Q_DECLARE_METATYPE(const WindowInfo*)

#endif // WINDOWINFO_H
