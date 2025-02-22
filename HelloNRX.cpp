// HelloNRX.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "HelloNRX.h"

#ifdef _DEBUG
#endif

/// <summary>
/// Функция добавления примитива в пространство модели
/// </summary>
void addToModelSpace(AcDbObjectId& objId, AcDbEntity* pEntity)
{
	//Таблица
	AcDbBlockTable* pBlockTable;
	//Коллекция объектов из таблицы
	AcDbBlockTableRecord* pBlock;

	acdbHostApplicationServices()->workingDatabase()
		->getSymbolTable(pBlockTable, AcDb::kForRead);
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlock, AcDb::kForWrite);
	pBlockTable->close();

	pBlock->appendAcDbEntity(objId, pEntity);
	pBlock->close();
}

/// <summary>
/// Команда создания отрезка классичесим способом
/// </summary>
void helloNrxLine()
{
	AcDbLine* pLine = new AcDbLine(AcGePoint3d(0, 0, 0), AcGePoint3d(100, 100, 0));
	addToModelSpace(pLine->objectId(), pLine);
	pLine->close();
}

/// <summary>
/// Команда создания отрезков при помощи транзакций
/// </summary>
void helloNxLineTran()
{
	AcTransaction* pTrans = actrTransactionManager->startTransaction();
	AcDbDatabase* dB = acdbHostApplicationServices()->workingDatabase();

	try 
	{
		//Открытие блока текущего пространства на запись
		AcDbObject* pBlockTableObj = nullptr;
		AcDbBlockTable* pBlockTable = nullptr;
		pTrans->getObject(pBlockTableObj, dB->blockTableId(), AcDb::kForRead);
		//Приводим указатель к типу BlockTable
		pBlockTable = AcDbBlockTable::cast(pBlockTableObj);

		AcDbBlockTableRecord* pModelSpace = nullptr;
		pBlockTable->getAt(ACDB_MODEL_SPACE, pModelSpace, AcDb::kForWrite);

		//Создание и добавление линии в блок
		AcGePoint3d firstPoint;
		int retFirst = ncedGetPoint(NULL, L"\nПоставьте первую точку ", asDblArray(firstPoint));
		AcGePoint3d secondPoint;
		int retSecond = ncedGetPoint(NULL, L"\nПоставьте вторую точку ", asDblArray(secondPoint));

		AcDbLine* pLine = nullptr;

		if (retFirst != RTCAN && retSecond != RTCAN)
		{
			pLine = new AcDbLine(AcGePoint3d(firstPoint.x, firstPoint.y, 0),
				AcGePoint3d(secondPoint.x, secondPoint.y, 0));
		}
		else
		{
			pLine = new AcDbLine(AcGePoint3d(0, 0, 0), AcGePoint3d(-100, -100, 0));
		}

		pModelSpace->appendAcDbEntity(pLine->objectId(), pLine);
		actrTransactionManager->addNewlyCreatedDBRObject(pLine);
		actrTransactionManager->endTransaction();
		pModelSpace->close();
		pBlockTable->close();
		pLine->close();
	}
	catch (const Acad::ErrorStatus)
	{
		actrTransactionManager->abortTransaction();
	}
}

/// <summary>
/// Команда с аффинными преобразованиями примитивов
/// </summary>
void helloNrxMove()
{
	ads_name ssname;
	//Берём ВСЕ примитвы с чертежа
	acedSSGet(L"A", NULL, NULL, NULL, ssname);
	long length = 0;
	acedSSLength(ssname, &length);
	ads_name ent; 

	//В циле каждый примитив вертим
	for (long i = 0; i < length; i++)
	{
		acedSSName(ssname, i, ent); 
		AcDbObjectId objId;
		acdbGetObjectId(objId, ent); 
		AcDbEntity* pEntity = nullptr;
		acdbOpenAcDbEntity(pEntity, objId, AcDb::kForWrite);
		//Сначала разворачиваем на 360
		NcGeMatrix3d mat = NcGeMatrix3d::rotation(PI, NcGeVector3d::kZAxis);
		pEntity->transformBy(mat);
		//Увеличиваем в два раза
		mat = NcGeMatrix3d::scaling(2, NcGePoint3d::kOrigin);
		pEntity->transformBy(mat);
		//Отражаем от оси Y
		mat = NcGeMatrix3d::mirroring(NcGePlane::kYZPlane);
		pEntity->transformBy(mat);
		//Смещаем по оси Y на 1000 вверх
		AcGeVector3d displacementVector(0, 1000, 0);	
		mat = NcGeMatrix3d::translation(displacementVector);
		pEntity->transformBy(mat);
	}
	acedSSFree(ssname);
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
			helloNrxLine);
		ncedRegCmds->addCommand(L"HELLONRX_GROUP",
			L"_HELLONRXTRAN",
			L"HELLONRXTRAN",
			ACRX_CMD_TRANSPARENT,
			helloNxLineTran);
		ncedRegCmds->addCommand(L"HELLONRX_GROUP",
			L"_HELLONRXMOVE",
			L"HELLONRXMOVE",
			ACRX_CMD_TRANSPARENT,
			helloNrxMove);
		break;
	case NcRx::kUnloadAppMsg:   
		acedRegCmds->removeGroup(L"HELLONRX_GROUP");
		break;
	}
	return NcRx::kRetOK;
}