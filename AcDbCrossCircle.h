//
// Копирайт (С) 2020, ООО «Нанософт разработка». Все права защищены.
// 
// Данное программное обеспечение, все исключительные права на него, его
// документация и сопроводительные материалы принадлежат ООО «Нанософт разработка».
// Данное программное обеспечение может использоваться при разработке и входить
// в состав разработанных программных продуктов при соблюдении условий
// использования, оговоренных в «Лицензионном договоре присоединения
// на использование программы для ЭВМ «Платформа nanoCAD»».
// 
// Данное программное обеспечение защищено в соответствии с законодательством
// Российской Федерации об интеллектуальной собственности и международными
// правовыми актами.
// 
// Используя данное программное обеспечение,  его документацию и
// сопроводительные материалы вы соглашаетесь с условиями использования,
// указанными выше. 
//

//-----------------------------------------------------------------------------
//----- AcDbCrossCircle.h : Declaration of the AcDbCrossCircle
//-----------------------------------------------------------------------------
#pragma once

#ifdef CROSSCIRCLE_MODULE
#define DLLIMPEXP __declspec(dllexport)
#else
#define DLLIMPEXP
#endif

//-----------------------------------------------------------------------------
class DLLIMPEXP AcDbCrossCircle : public AcDbEntity
{

public:
  ACRX_DECLARE_MEMBERS(AcDbCrossCircle);

protected:
  static Adesk::UInt32 kCurrentVersionNumber;

public:
  AcDbCrossCircle();
  virtual ~AcDbCrossCircle();

  // Методы доступа к параметрам
  AcGePoint3d center();
  AcGeVector3d radiusVector();
  AcGeVector3d normal();
  double angle();

  Acad::ErrorStatus setCenter(AcGePoint3d);
  Acad::ErrorStatus setRadiusVector(AcGeVector3d);
  Acad::ErrorStatus setNormal(AcGeVector3d);
  Acad::ErrorStatus setAngle(double);

  Acad::ErrorStatus attachLine(AcDbObjectId lineId);

  // AcDbObject
  virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler *pFiler) const override;
  virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler *pFiler) override;

  virtual Acad::ErrorStatus dxfOutFields (AcDbDxfFiler *pFiler) const override;
  virtual Acad::ErrorStatus dxfInFields (AcDbDxfFiler *pFiler) override;

protected:
  virtual Acad::ErrorStatus subGetClassID(CLSID* pClsid) const override;
  void modified(const AcDbObject* pObj) override;
  void erased(const AcDbObject* pObj, Adesk::Boolean bErasing = true) override;

  // AcDbEntity
  virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw *pWD) override;
  virtual void subViewportDraw(AcGiViewportDraw *pVD) override;
  virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform) override;

  // Ручки
  virtual Acad::ErrorStatus subGetGripPoints(AcGePoint3dArray &gripPoints, AcDbIntArray &osnapModes, AcDbIntArray &geomIds) const override;
  virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbIntArray &indices, const AcGeVector3d &offset) override;

  // Растягивание
  virtual Acad::ErrorStatus AcDbCrossCircle::subGetStretchPoints(AcGePoint3dArray& points) const override;
  virtual Acad::ErrorStatus AcDbCrossCircle::subMoveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset) override;

  // Объектные привязки
  virtual Acad::ErrorStatus subGetOsnapPoints(
    AcDb::OsnapMode osnapMode,
    Adesk::GsMarker gsSelectionMark,
    const AcGePoint3d &pickPoint,
    const AcGePoint3d &lastPoint,
    const AcGeMatrix3d &viewXform,
    AcGePoint3dArray &snapPoints,
    AcDbIntArray &geomIds) const override;
  virtual Acad::ErrorStatus subGetOsnapPoints(
    AcDb::OsnapMode osnapMode,
    Adesk::GsMarker gsSelectionMark,
    const AcGePoint3d &pickPoint,
    const AcGePoint3d &lastPoint,
    const AcGeMatrix3d &viewXform,
    AcGePoint3dArray &snapPoints,
    AcDbIntArray &geomIds,
    const AcGeMatrix3d &insertionMat) const override;

  // Разбиение
  virtual Acad::ErrorStatus subExplode(AcDbVoidPtrArray& entitySet) const override;

  // Пересечение
  virtual Acad::ErrorStatus subIntersectWith(const AcDbEntity* entity, AcDb::Intersect intType,
                                             AcGePoint3dArray& points, Adesk::GsMarker this_marker = 0,
                                             Adesk::GsMarker other_marker = 0) const override;
  virtual Acad::ErrorStatus subIntersectWith(const AcDbEntity* entity, AcDb::Intersect intType,
                                             const AcGePlane& plane, AcGePoint3dArray& points,
                                             Adesk::GsMarker this_marker = 0,
                                             Adesk::GsMarker other_marker = 0) const override;
  // M5: Вычисление ссылки на отрезок
  AcGeVector3d computeVecLineRef(AcDbLine *pLine);
  AcGeVector3d computeVecLineRef();

private:
  // Параметры примитива
    AcGePoint3d _center;        // центр окружности
    AcGeVector3d _radiusVector; // вектор радиуса
    AcGeVector3d _normal;        // нормаль

  // TODO M5: Uncomment
  //AcDbSoftPointerId m_lineId; // Ссылка на отрезок
  //AcGeVector3d m_vecLineRef;  // Вектор от центра КрестКруга до центра отрезка
};
