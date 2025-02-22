// HelloNRX.h: основной файл заголовка для библиотеки DLL HelloNRX
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы

// CHelloNRXApp
// Реализацию этого класса см. в файле HelloNRX.cpp
//

class CHelloNRXApp : public CWinApp
{
public:
	CHelloNRXApp();

// Переопределение
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
