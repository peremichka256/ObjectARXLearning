//
// �������� (�) 2020, ��� ��������� ����������. ��� ����� ��������.
// 
// ������ ����������� �����������, ��� �������������� ����� �� ����, ���
// ������������ � ���������������� ��������� ����������� ��� ��������� ����������.
// ������ ����������� ����������� ����� �������������� ��� ���������� � �������
// � ������ ������������� ����������� ��������� ��� ���������� �������
// �������������, ����������� � ������������� �������� �������������
// �� ������������� ��������� ��� ��� ���������� nanoCAD��.
// 
// ������ ����������� ����������� �������� � ������������ � �����������������
// ���������� ��������� �� ���������������� ������������� � ��������������
// ��������� ������.
// 
// ��������� ������ ����������� �����������,  ��� ������������ �
// ���������������� ��������� �� ������������ � ��������� �������������,
// ���������� ����. 
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

  // ������ ������� � ����������
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

  // �����
  virtual Acad::ErrorStatus subGetGripPoints(AcGePoint3dArray &gripPoints, AcDbIntArray &osnapModes, AcDbIntArray &geomIds) const override;
  virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbIntArray &indices, const AcGeVector3d &offset) override;

  // ������������
  virtual Acad::ErrorStatus AcDbCrossCircle::subGetStretchPoints(AcGePoint3dArray& points) const override;
  virtual Acad::ErrorStatus AcDbCrossCircle::subMoveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset) override;

  // ��������� ��������
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

  // ���������
  virtual Acad::ErrorStatus subExplode(AcDbVoidPtrArray& entitySet) const override;

  // �����������
  virtual Acad::ErrorStatus subIntersectWith(const AcDbEntity* entity, AcDb::Intersect intType,
                                             AcGePoint3dArray& points, Adesk::GsMarker this_marker = 0,
                                             Adesk::GsMarker other_marker = 0) const override;
  virtual Acad::ErrorStatus subIntersectWith(const AcDbEntity* entity, AcDb::Intersect intType,
                                             const AcGePlane& plane, AcGePoint3dArray& points,
                                             Adesk::GsMarker this_marker = 0,
                                             Adesk::GsMarker other_marker = 0) const override;
  // M5: ���������� ������ �� �������
  AcGeVector3d computeVecLineRef(AcDbLine *pLine);
  AcGeVector3d computeVecLineRef();

private:
  // ��������� ���������
    AcGePoint3d _center;        // ����� ����������
    AcGeVector3d _radiusVector; // ������ �������
    AcGeVector3d _normal;        // �������

  // TODO M5: Uncomment
  //AcDbSoftPointerId m_lineId; // ������ �� �������
  //AcGeVector3d m_vecLineRef;  // ������ �� ������ ���������� �� ������ �������
};
