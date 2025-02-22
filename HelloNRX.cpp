// HelloNRX.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "HelloNRX.h"

#ifdef _DEBUG
#endif

void helloNrxCmd()
{
	//Запрос на ввод String
	ACHAR str[133];
	//Код результата работы функции
	int ret;
	ret = ncedGetString(1, L"\nВведите строку", str);

	if (ret != RTCAN)
	{
		ncutPrintf(L"\nВы ввели: %s", str);
	}

	//Запрос на ввод Double
	double realNumber = 0.0;
	ret = ncedGetReal(L"\nВведите вещественное значение", &realNumber);

	if (ret != RTCAN)
	{
		ncutPrintf(L"\nВы ввели: %f", realNumber);
	}

	//Запрос на ввод Point (вводить через ,)
	ncedInitGet(RSG_NONULL, L"Первое вТорое ещеОдно _ First sEcond oneMore");
	nds_real point[3];
	ret = ncedGetPoint(point, L"\nВведите координаты точки по x y z через ,: ", point);

	if (ret == RTKWORD)
	{
		NCHAR kw[100];
		ncedGetInput(kw);
		//Выводит английское название, хотя может вводится русское
		ncutPrintf(L"\nВами было введено ключевое слово: %s", kw);
	}
}

// EntryPoint
extern "C" __declspec(dllexport) NcRx::AppRetCode
ncrxEntryPoint(NcRx::AppMsgCode msg, void* pkt)
{
	switch (msg)
	{
	case NcRx::kInitAppMsg:
		ncedRegCmds->addCommand(L"HELLONRX_GROUP",
			L"_HELLONRX",
			L"HELLONRX",
			ACRX_CMD_TRANSPARENT,
			helloNrxCmd);
		break;
	case NcRx::kUnloadAppMsg:   
		acedRegCmds->removeGroup(L"HELLONRX_GROUP");
		break;
	}
	return NcRx::kRetOK;
}