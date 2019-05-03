#ifndef WINDOWINFO_H
#define WINDOWINFO_H

#include "Win.h"
#include <QString>

struct WindowInfo
{
public:
	WindowInfo(HWND newHwnd = nullptr, QString newTitle = QString(), QString newClass = QString())
		: hwnd(newHwnd)
		, winTitle(newTitle)
		, winClass(newClass)
	{
	}

	HWND hwnd;
	QString winTitle;
	QString winClass;
};

Q_DECLARE_METATYPE(WindowInfo)

#endif // WINDOWINFO_H
