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

// ������ ������ � ������ ����������
//�������
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

//�������
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
  
  // TODO: ���������, ������ �� ��������� � ��������, ���� ��, ������� ������������� �������
  //       ���� ������ ������� ObjectId, �������� �������
  //       ��������� ObjectId �������, �������� � ���� ��������� ������������ ���������
  //       ��������� �������� �� ������ ����� �� �������� �������

  return Acad::eOk;
}

// ���������
Adesk::Boolean AcDbCrossCircle::subWorldDraw(AcGiWorldDraw *pWD)
{
    assertReadEnabled();
    
     // ����
    pWD->geometry().circle(center(), radiusVector().length(), normal());

    // �����
    //������ �����
    AcGePoint3d pointsArray[2];
    //�� ������ � ������� ������� �������
    pointsArray[0] = center() + radiusVector() * 1.2;
    //�� ������ � �������� ������� �� ������� �������
    pointsArray[1] = center() - radiusVector();
    pWD->geometry().polyline(2, pointsArray);

    //������ �����
    pointsArray[0] = center() + radiusVector().perpVector()
        * (radiusVector().length() * 1.1);
    pointsArray[1] = center() - radiusVector().perpVector() 
        * radiusVector().length();
    pWD->geometry().polyline(2, pointsArray);


  return Adesk::kTrue; // ���� ������� Adesk::kFalse, ����� ������ ����� subViewportDraw()
}

void AcDbCrossCircle::subViewportDraw(AcGiViewportDraw *pVD)
{
  // TODO: �����. ���������� ������� ���������� ����������� ������� � ��������� ������
  //       ������� ��������� ������ ���������� ��� ������ pViewportDraw->viewport().viewDir();
}

Acad::ErrorStatus AcDbCrossCircle::dwgOutFields(AcDbDwgFiler *pFiler) const
{
  assertReadEnabled();

  // ���������� ������ �������� ������
  Acad::ErrorStatus es = AcDbEntity::dwgOutFields(pFiler);
  if (es != Acad::eOk)
    return es;

  // ������ �������
  if ((es =pFiler->writeUInt32(AcDbCrossCircle::kCurrentVersionNumber)) != Acad::eOk)
    return es;

  // ��������� �������
  // TODO: ��������� ��������� ��������� ��� ������ pFiler->writePoint3d(), pFiler->writeVector3d()
   pFiler->writePoint3d(_center);
   pFiler->writeVector3d(_normal);
   pFiler->writeVector3d(_radiusVector);
   //pFiler->writeDouble(_radiusVector.length());
  // TODO M5: ��������� ������ �� �������

  return pFiler->filerStatus();
}

Acad::ErrorStatus AcDbCrossCircle::dwgInFields(AcDbDwgFiler *pFiler)
{
  assertWriteEnabled();

  // ������ ������ �������� ������
  Acad::ErrorStatus es = AcDbEntity::dwgInFields(pFiler);
  if (es != Acad::eOk)
    return es;

  // ������ ������ �������
  Adesk::UInt32 version = 0;

  if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
    return es;

  if (version > AcDbCrossCircle::kCurrentVersionNumber) // ���� ������ ������� ������ �������
    return Acad::eMakeMeProxy;                         // �� ������� ������-������

  // ������ ����������
  // TODO: �������� ��������� ��������� ��� ������ pFiler->readPoint3d(), pFiler->readVector3d()
  pFiler->readPoint3d(&_center);
  pFiler->readVector3d(&_normal);
  pFiler->readVector3d(&_radiusVector);

  // TODO M3: �������� ������ �� ������� � ��������� m_vecLineRef

  return pFiler->filerStatus();
}

Acad::ErrorStatus AcDbCrossCircle::subTransformBy(const AcGeMatrix3d& xform)
{
  assertWriteEnabled();

  // TODO: ������������� ��������� ���������
  _center.transformBy(xform);
  _normal.transformBy(xform);
  _radiusVector.transformBy(xform);

  return Acad::eNotImplementedYet;
}

// �����
Acad::ErrorStatus AcDbCrossCircle::subGetGripPoints(AcGePoint3dArray &gripPoints,
    AcDbIntArray &osnapModes, AcDbIntArray &geomIds) const
{
     assertReadEnabled();

     AcGePoint3dArray aCrCircle;
     aCrCircle.setLogicalLength(9); 

     // �����
     aCrCircle[0] = _center;

     // ���� ��������
     aCrCircle[1] = _center + _radiusVector;
     aCrCircle[2] = _center - _radiusVector;
     aCrCircle[3] = _center - _radiusVector.perpVector()
         * _radiusVector.length();
     aCrCircle[4] = _center + _radiusVector.perpVector()
         * _radiusVector.length();

     // ������
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
        case 0: // �����
            this->setCenter(_center + offset);
          break;
        case 1: // ���� ��������
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
        case 5: // ������
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

  // ��������� ��������� �� ������������
  //
  // ����
  // TODO: ������� AcDbCircle � �������� � entitySet

  // ������ �������
  // TODO: ������� AcDbLine � �������� � entitySet

  // ������ �������
  // TODO: ������� AcDbLine � �������� � entitySet

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

    // TODO: ������� intersectWith() ��� ������� ���������
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

    // TODO: ������� intersectWith() ��� ������� ���������

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

    // TODO: ������� subGetOsnapPoints() ��� ������� ���������

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

    // TODO: ������� subGetOsnapPoints() ��� ������� ���������
    //       ���� ���-�� ����� �������, ������� ������� ��� insertionMat
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

  // TODO: ������� m_lineId � ������� computeVecLineRef(pLine)

  return newVecLineRef;
}

AcGeVector3d AcDbCrossCircle::computeVecLineRef(AcDbLine *pLine)
{
  AcGeVector3d newVecLineRef = AcGeVector3d::kIdentity;

  if (!pLine)
    return AcGeVector3d::kIdentity;

  // TODO: ��������� ������ �� ������ ���������� �� �������� ������� pLine

  return newVecLineRef;
}

void AcDbCrossCircle::modified(const AcDbObject* pObj)
{
  if (!pObj)
    return;

  // TODO: ���������, ��������� �� objectId ������� � m_lineId
  //       ���� �� ���������, �� ��� ���������������� � ������� �������
  //       �� ��� ����� ������������� �������

  AcDbLine *pLine = AcDbLine::cast(pObj);

  if (!pLine)
    return;

  // ���� ����� ������ �������� ���������� �� �������,
  // �������� ����� ����������
  // TODO: ��������� ����� ������ ��������, �������� �� ������
  Acad::ErrorStatus es;
  Adesk::Boolean wasWritable;

  es = upgradeFromNotify(wasWritable);

  // TODO: �������� ����� ����������

  downgradeToNotify(wasWritable);
}

void AcDbCrossCircle::erased(const AcDbObject* pObj, Adesk::Boolean bErasing)
{
  // TODO: ���� bErasing, ������� attachLine(AcDbObjectId::kNull)
  //       ���� !bErasing, �������� � pObj objectId � ������� attachLine(objectId)
  //       �� ������ ��� upgradeFromNotify()/downgradeToNotify()

}

Acad::ErrorStatus AcDbCrossCircle::subGetClassID(CLSID* pClsid) const
{
  // TODO M6: Uncomment
  //*pClsid = CLSID_CrCircleW;
  //return Acad::eOk;

  return Acad::eNotImplementedYet;
}

