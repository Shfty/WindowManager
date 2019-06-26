#ifndef WIN_H
#define WIN_H

#include <QMetaType>
#include <QVariant>

#include "Windows.h"
#include "WinBase.h"
#include "processthreadsapi.h"
#include "powrprof.h"
#include "dwmapi.h"
#include "Psapi.h"

#undef HWND_TOP
#undef HWND_BOTTOM
#undef HWND_TOPMOST
#undef HWND_NOTOPMOST

#define HWND_TOP reinterpret_cast<HWND>(0)
#define HWND_BOTTOM reinterpret_cast<HWND>(1)
#define HWND_TOPMOST reinterpret_cast<HWND>(-1)
#define HWND_NOTOPMOST reinterpret_cast<HWND>(-2)

QDataStream& operator <<(QDataStream&, const HWND&);
QDataStream& operator >>(QDataStream&, HWND&);
QDebug operator <<(QDebug dbg, const HWND& hwnd);

Q_DECLARE_METATYPE(HWND)

#endif // WIN_H
