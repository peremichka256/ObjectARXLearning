//-----------------------------------------------------------------------------
//----- AcDbCrossCircle.cpp : Implementation of AcDbCrossCircle
//-----------------------------------------------------------------------------
#include "pch.h"
#include "AcDbCrossCircle.h"
//#include "ComCrCircleWrapper.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 AcDbCrossCircle::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
                         AcDbCrossCircle, AcDbEntity,
                         AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
                         AcDbProxyEntity::kNoOperation, ACDBCROSSCIRCLE,
                         CROSSCIRCLEAPP
                         |Product Desc:     A description for your object
                         |Company:          Your company name
                         |WEB Address:      Your company WEB site address
                        )

//-----------------------------------------------------------------------------

AcDbCrossCircle::AcDbCrossCircle() : AcDbEntity()
{
    setCenter(AcGePoint3d(0, 0, 0));
    setRadiusVector(AcGeVector3d(1, 0, 0));
    setNormal(AcGeVector3d(0, 0, 1));
}

AcDbCrossCircle::~AcDbCrossCircle()
{
}

// Методы чтения и записи параметров
//Геттеры
AcGePoint3d AcDbCrossCircle::center()
{
    assertReadEnabled();
    return _center;
}

AcGeVector3d AcDbCrossCircle::radiusVector()
{
    assertReadEnabled();
    return _radiusVector;
}

AcGeVector3d AcDbCrossCircle::normal()
{
    assertReadEnabled();
    return _normal;
}

double AcDbCrossCircle::angle()
{
    assertReadEnabled();
    AcGeVector3d xVector = acdbHostApplicationServices()
        ->workingDatabase()->ucsxdir();
    return _radiusVector.angleTo(xVector, _normal);
}

//Сеттеры
Acad::ErrorStatus AcDbCrossCircle::setCenter(AcGePoint3d center)
{
    assertWriteEnabled();
    _center = center;
    return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircle::setRadiusVector(AcGeVector3d radiusVector)
{
    assertWriteEnabled();
    _radiusVector = radiusVector;
    return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircle::setNormal(AcGeVector3d normal)
{
    assertWriteEnabled();
    _normal = normal;
    return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircle::setAngle(double angle)
{
    assertWriteEnabled();
    AcGeVector3d xVector = acdbHostApplicationServices()
        ->workingDatabase()->ucsxdir();
    _radiusVector = _radiusVector.rotateBy(angle + _radiusVector
        .angleTo(xVector, _normal), _normal);
    return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircle::attachLine(AcDbObjectId lineId)
{
  assertWriteEnabled();
  
  // TODO: Проверить, связан ли КрестКруг с отрезком, если да, удалить персистентный реактор
  //       Если пришёл нулевой ObjectId, отвязать отрезок
  //       Запомнить ObjectId отрезка, добавить в него КрестКруг ерсистентным реактором
  //       Вычислить смещение от центра круга до середины отрезка

  return Acad::eOk;
}

// Отрисовка
Adesk::Boolean AcDbCrossCircle::subWorldDraw(AcGiWorldDraw *pWD)
{
    assertReadEnabled();
    
     // Круг
    pWD->geometry().circle(center(), radiusVector().length(), normal());

    // Крест
    //Первая линия
    AcGePoint3d pointsArray[2];
    //От центра в сторону вектора радиуса
    pointsArray[0] = center() + radiusVector() * 1.2;
    //От центра в обратную сторону от вектора радиуса
    pointsArray[1] = center() - radiusVector();
    pWD->geometry().polyline(2, pointsArray);

    //Вторая линия
    pointsArray[0] = center() + radiusVector().perpVector()
        * (radiusVector().length() * 1.1);
    pointsArray[1] = center() - radiusVector().perpVector() 
        * radiusVector().length();
    pWD->geometry().polyline(2, pointsArray);


  return Adesk::kTrue; // Если вернуть Adesk::kFalse, будет вызван метод subViewportDraw()
}

void AcDbCrossCircle::subViewportDraw(AcGiViewportDraw *pVD)
{
  // TODO: Бонус. Нарисовать красную окружность половинного радиуса в плоскости экрана
  //       Нормаль плоскости экрана определять при помощи pViewportDraw->viewport().viewDir();
}

Acad::ErrorStatus AcDbCrossCircle::dwgOutFields(AcDbDwgFiler *pFiler) const
{
  assertReadEnabled();

  // Сохранение данных базового класса
  Acad::ErrorStatus es = AcDbEntity::dwgOutFields(pFiler);
  if (es != Acad::eOk)
    return es;

  // Версия объекта
  if ((es =pFiler->writeUInt32(AcDbCrossCircle::kCurrentVersionNumber)) != Acad::eOk)
    return es;

  // Параметры объекта
  // TODO: Сохранить параметры примитива при помощи pFiler->writePoint3d(), pFiler->writeVector3d()
   pFiler->writePoint3d(_center);
   pFiler->writeVector3d(_normal);
   pFiler->writeVector3d(_radiusVector);
   //pFiler->writeDouble(_radiusVector.length());
  // TODO M5: Сохранить ссылку на отрезок

  return pFiler->filerStatus();
}

Acad::ErrorStatus AcDbCrossCircle::dwgInFields(AcDbDwgFiler *pFiler)
{
  assertWriteEnabled();

  // Чтение данных базового класса
  Acad::ErrorStatus es = AcDbEntity::dwgInFields(pFiler);
  if (es != Acad::eOk)
    return es;

  // Чтение версии объекта
  Adesk::UInt32 version = 0;

  if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
    return es;

  if (version > AcDbCrossCircle::kCurrentVersionNumber) // Если версия объекта больше текущей
    return Acad::eMakeMeProxy;                         // то создать прокси-объект

  // Чтение параметров
  // TODO: Прочесть параметры примитива при помощи pFiler->readPoint3d(), pFiler->readVector3d()
  pFiler->readPoint3d(&_center);
  pFiler->readVector3d(&_normal);
  pFiler->readVector3d(&_radiusVector);

  // TODO M3: Прочесть ссылку на отрезок и вычислить m_vecLineRef

  return pFiler->filerStatus();
}

Acad::ErrorStatus AcDbCrossCircle::subTransformBy(const AcGeMatrix3d& xform)
{
  assertWriteEnabled();

  // TODO: Преобразовать параметры примитива
  _center.transformBy(xform);
  _normal.transformBy(xform);
  _radiusVector.transformBy(xform);

  return Acad::eNotImplementedYet;
}

// Ручки
Acad::ErrorStatus AcDbCrossCircle::subGetGripPoints(AcGePoint3dArray &gripPoints,
    AcDbIntArray &osnapModes, AcDbIntArray &geomIds) const
{
     assertReadEnabled();

     AcGePoint3dArray aCrCircle;
     aCrCircle.setLogicalLength(9); 

     // Центр
     aCrCircle[0] = _center;

     // Угол поворота
     aCrCircle[1] = _center + _radiusVector;
     aCrCircle[2] = _center - _radiusVector;
     aCrCircle[3] = _center - _radiusVector.perpVector()
         * _radiusVector.length();
     aCrCircle[4] = _center + _radiusVector.perpVector()
         * _radiusVector.length();

     // Радиус
     AcGeVector3d vec45 = _radiusVector;
     vec45.rotateBy(atan(1.0), _normal);

     aCrCircle[5] = _center + vec45;
     aCrCircle[6] = _center - vec45;
     aCrCircle[7] = _center - vec45.perpVector() 
         * _radiusVector.length();
     aCrCircle[8] = _center + vec45.perpVector() 
         * _radiusVector.length();

     gripPoints.append(aCrCircle);

     return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircle::subMoveGripPointsAt(const AcDbIntArray &indices, const AcGeVector3d &offset)
{
    if (indices.length()== 0 || offset.isZeroLength())
        return Acad::eOk;

    assertWriteEnabled();

    AcGeVector3d vec45 = _radiusVector;
    vec45.rotateBy(atan(1.0), _normal);

    for (int i = 0; i < indices.length(); i++) 
    {
        switch(indices[i]) 
        {
        case 0: // Центр
            this->setCenter(_center + offset);
          break;
        case 1: // Угол поворота
            this->setRadiusVector(_radiusVector.length() 
                * (_radiusVector + offset).normalize());
          break;
        case 2:
            this->setRadiusVector(_radiusVector.length() 
                * (_radiusVector - offset).normalize());
          break;
        case 3:
            this->setRadiusVector(_radiusVector.length() 
                * (_radiusVector - offset.crossProduct(_normal)).normalize());
          break;
        case 4:
            this->setRadiusVector(_radiusVector.length() 
                * (_radiusVector + offset.crossProduct(_normal)).normalize());
          break;
        case 5: // Радиус
            this->setRadiusVector(_radiusVector.normalize() * (vec45 + offset).length());
          break;
        case 6:
            this->setRadiusVector(_radiusVector.normalize() * (-vec45 + offset).length());
          break;
        case 7:
            this->setRadiusVector(_radiusVector.normalize()
                * (-vec45.perpVector() * _radiusVector.length() + offset).length());
          break;
        case 8:
            this->setRadiusVector(_radiusVector.normalize()
                * (vec45.perpVector() * _radiusVector.length() + offset).length());
          break;
        }
    }

    return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircle::subGetStretchPoints(AcGePoint3dArray& points) const
{
    AcDbIntArray* array = nullptr;
    subGetGripPoints(points, *array, *array);
    return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircle::subMoveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
    subMoveGripPointsAt(indices, offset);
    return Acad::eOk;

}

Acad::ErrorStatus AcDbCrossCircle::subExplode(AcDbVoidPtrArray& entitySet) const
{
  assertReadEnabled();

  // Разбиваем КрестКруг на составляющие
  //
  // Круг
  // TODO: Создать AcDbCircle и добавить в entitySet

  // Первый отрезок
  // TODO: Создать AcDbLine и добавить в entitySet

  // Второй отрезок
  // TODO: Создать AcDbLine и добавить в entitySet

  return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircle::subIntersectWith(const AcDbEntity* entity, AcDb::Intersect intType,
                                                    AcGePoint3dArray& allPoints, Adesk::GsMarker this_marker,
                                                    Adesk::GsMarker other_marker) const
{
  assertReadEnabled();

  Acad::ErrorStatus es;
  AcDbVoidPtrArray entitySet;

  es = subExplode(entitySet);
  
  if (es != Acad::eOk)
    return es;

  for (int i=0; i<entitySet.length(); i++)
  {
    AcRxObject *pObj = (AcRxObject *)entitySet[i];

    // TODO: Вызвать intersectWith() для каждого примитива
    delete pObj;
  }

  return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircle::subIntersectWith(const AcDbEntity* entity, AcDb::Intersect intType,
                                                    const AcGePlane& plane, AcGePoint3dArray& allPoints,
                                                    Adesk::GsMarker this_marker,
                                                    Adesk::GsMarker other_marker) const
{
  assertReadEnabled();

  Acad::ErrorStatus es;
  AcDbVoidPtrArray entitySet;

  es = subExplode(entitySet);
  
  if (es != Acad::eOk)
    return es;

  for (int i=0; i<entitySet.length(); i++)
  {
    AcRxObject *pObj = (AcRxObject *)entitySet[i];

    // TODO: Вызвать intersectWith() для каждого примитива

    delete pObj;
  }

  return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircle::subGetOsnapPoints(
  AcDb::OsnapMode osnapMode,
  Adesk::GsMarker gsSelectionMark,
  const AcGePoint3d &pickPoint,
  const AcGePoint3d &lastPoint,
  const AcGeMatrix3d &viewXform,
  AcGePoint3dArray &snapPoints,
  AcDbIntArray &geomIds) const
{
  assertReadEnabled();

  Acad::ErrorStatus es;
  AcDbVoidPtrArray entitySet;
  AcGePoint3dArray allPoints;

  es = subExplode(entitySet);
  
  if (es != Acad::eOk)
    return es;

  for (int i=0; i<entitySet.length(); i++)
  {
    AcRxObject *pObj = (AcRxObject *)entitySet[i];

    // TODO: Вызвать subGetOsnapPoints() для каждого примитива

    delete pObj;
  }

  return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircle::subGetOsnapPoints(
  AcDb::OsnapMode osnapMode,
  Adesk::GsMarker gsSelectionMark,
  const AcGePoint3d &pickPoint,
  const AcGePoint3d &lastPoint,
  const AcGeMatrix3d &viewXform,
  AcGePoint3dArray &snapPoints,
  AcDbIntArray &geomIds,
  const AcGeMatrix3d &insertionMat) const
{
  assertReadEnabled();

  Acad::ErrorStatus es;
  AcDbVoidPtrArray entitySet;
  AcGePoint3dArray allPoints;

  es = subExplode(entitySet);
  
  if (es != Acad::eOk)
    return es;

  for (int i=0; i<entitySet.length(); i++)
  {
    AcRxObject *pObj = (AcRxObject *)entitySet[i];

    // TODO: Вызвать subGetOsnapPoints() для каждого примитива
    //       Если кол-во точек нулевое, вызвать вариант без insertionMat
    delete pObj;
  }

  return Acad::eOk;
}

AcGeVector3d AcDbCrossCircle::computeVecLineRef()
{
  Acad::ErrorStatus es;

  AcDbEntity *pEnt;
  AcDbLine *pLine;
  AcGeVector3d newVecLineRef = AcGeVector3d::kIdentity;

  // TODO: Открыть m_lineId и вызвать computeVecLineRef(pLine)

  return newVecLineRef;
}

AcGeVector3d AcDbCrossCircle::computeVecLineRef(AcDbLine *pLine)
{
  AcGeVector3d newVecLineRef = AcGeVector3d::kIdentity;

  if (!pLine)
    return AcGeVector3d::kIdentity;

  // TODO: Вычислить вектор из центра КрестКруга до середины отрезка pLine

  return newVecLineRef;
}

void AcDbCrossCircle::modified(const AcDbObject* pObj)
{
  if (!pObj)
    return;

  // TODO: Проверить, совпадает ли objectId отрезка с m_lineId
  //       Если не совпадает, то при перепривязывании к другому отрезку
  //       не был удалён персистентный реактор

  AcDbLine *pLine = AcDbLine::cast(pObj);

  if (!pLine)
    return;

  // Если новый вектор смещения отличается от старого,
  // сместить центр КрестКруга
  // TODO: Вычислить новый вектор смещения, сравнить со старым
  Acad::ErrorStatus es;
  Adesk::Boolean wasWritable;

  es = upgradeFromNotify(wasWritable);

  // TODO: Сместить центр КрестКруга

  downgradeToNotify(wasWritable);
}

void AcDbCrossCircle::erased(const AcDbObject* pObj, Adesk::Boolean bErasing)
{
  // TODO: Если bErasing, вызвать attachLine(AcDbObjectId::kNull)
  //       Если !bErasing, получить у pObj objectId и вызвать attachLine(objectId)
  //       Не забыть про upgradeFromNotify()/downgradeToNotify()

}

Acad::ErrorStatus AcDbCrossCircle::subGetClassID(CLSID* pClsid) const
{
  // TODO M6: Uncomment
  //*pClsid = CLSID_CrCircleW;
  //return Acad::eOk;

  return Acad::eNotImplementedYet;
}

