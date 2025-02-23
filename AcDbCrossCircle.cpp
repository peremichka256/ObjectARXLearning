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
  // TODO M3: �������� ������ �� ������� � ��������� m_vecLineRef

  return pFiler->filerStatus();
}

Acad::ErrorStatus AcDbCrossCircle::dxfOutFields(AcDbDxfFiler *pFiler) const
{
  assertReadEnabled();

  // ���������� ������ �������� ������
  Acad::ErrorStatus es = AcDbEntity::dxfOutFields(pFiler);
  if (es != Acad::eOk)
    return es;

  // DXF �����
  es = pFiler->writeItem(AcDb::kDxfSubclass, _RXST("AcDbCrossCircle"));
  if (es != Acad::eOk)
    return es;

  // ������ �������
  if ((es = pFiler->writeUInt32(AcDb::kDxfInt32, AcDbCrossCircle::kCurrentVersionNumber)) != Acad::eOk)
    return es;

  // ��������� �������
  // TODO: ��������� ��������� ���������, ��������� DXF ���� AcDb::kDxfXCoord, AcDb::kDxfNormalX, AcDb::kDxfNormalX+1

  return pFiler->filerStatus();
}

Acad::ErrorStatus AcDbCrossCircle::dxfInFields(AcDbDxfFiler *pFiler)
{
  assertWriteEnabled();

  // ������ ������ �������� ������
  Acad::ErrorStatus es =AcDbEntity::dxfInFields(pFiler);

  // DXF �����  
  if (es != Acad::eOk || !pFiler->atSubclassData(_RXST("AcDbCrossCircle")))
    return pFiler->filerStatus();

  // ������ �������
  struct resbuf rb;

  pFiler->readItem(&rb);

  if (rb.restype != AcDb::kDxfInt32)
  {
    pFiler->pushBackItem();
    pFiler->setError(Acad::eInvalidDxfCode, _RXST("\nError: expected group code %d(version #)"), AcDb::kDxfInt32);
    return pFiler->filerStatus();
  }

  Adesk::UInt32 version = (Adesk::UInt32)rb.resval.rlong;

  if (version > AcDbCrossCircle::kCurrentVersionNumber)
    return Acad::eMakeMeProxy;

  if (version < AcDbCrossCircle::kCurrentVersionNumber)
    return Acad::eMakeMeProxy ;
  
  // ������ ���������� � ������������ ������� �� DXF ����
  while(es == Acad::eOk &&(es = pFiler->readResBuf(&rb)) == Acad::eOk)
  {
    switch(rb.restype)
    {
    case AcDb::kDxfXCoord:
      // TODO: ������������� rb.resval.rpoint � AcGePoint3d, ��������� asPnt3d()
      break;
    case AcDb::kDxfNormalX:
      // TODO: ������������� rb.resval.rpoint � AcGeVector3d, ��������� asVec3d()
      break;
    case AcDb::kDxfNormalX + 1:
      // TODO: ������������� rb.resval.rpoint � AcGeVector3d, ��������� asVec3d()
      break;
    default:
      // ����������� DXF ���
      pFiler->pushBackItem();
      es = Acad::eEndOfFile;
      break ;
    }
  }

  if (es != Acad::eEndOfFile)
    return Acad::eInvalidResBuf;

  return pFiler->filerStatus();
}

Acad::ErrorStatus AcDbCrossCircle::subTransformBy(const AcGeMatrix3d& xform)
{
  assertWriteEnabled();

  // TODO: ������������� ��������� ���������

  return Acad::eNotImplementedYet;
}

// �����
Acad::ErrorStatus AcDbCrossCircle::subGetGripPoints(
  AcGePoint3dArray &gripPoints, AcDbIntArray &osnapModes, AcDbIntArray &geomIds
  ) const
{
  assertReadEnabled();

  AcGePoint3dArray aCrCircle;
  aCrCircle.setLogicalLength(1); // TODO: ����� 5, ����� 9

  // �����
  //aCrCircle[0] = // TODO: ����� ����������� �����

  // ���� ��������
  //aCrCircle[1] = // TODO: ����� ����� ������
  //aCrCircle[2] = 
  //aCrCircle[3] = 
  //aCrCircle[4] = 

  // ������
  //AcGeVector3d vec45=m_vecRad;
  //vec45.rotateBy(atan(1.0), m_normal);

  //aCrCircle[5] = // TODO: ����� ����� �� ���������� ����� ������� ������
  //aCrCircle[6] = 
  //aCrCircle[7] = 
  //aCrCircle[8] = 

  // TODO: gripPoints.append(aCrCircle);

  return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircle::subMoveGripPointsAt(const AcDbIntArray &indices, const AcGeVector3d &offset)
{
  if (indices.length()== 0 || offset.isZeroLength())
    return Acad::eOk;

  assertWriteEnabled();

  //AcGeVector3d vec45=m_vecRad;
  //vec45.rotateBy(atan(1.0), m_normal);

  for (int i = 0; i < indices.length(); i++) 
  {
    switch(indices[i]) 
    {
    case 0: // �����
      // TODO:
      break;
    case 1: // ���� ��������
      // TODO: 
      break;
    case 2:
      // TODO: 
      break;
    case 3:
      // TODO: 
      break;
    case 4:
      // TODO: 
      break;
    case 5: // ������
      // TODO: 
      break;
    case 6:
      // TODO: 
      break;
    case 7:
      // TODO: 
      break;
    case 8:
      // TODO: 
      break;
    }
  }

  return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircle::subGetStretchPoints(AcGePoint3dArray& points) const
{
  // TODO: ������� subGetGripPoints()
  return Acad::eNotImplementedYet;
}

Acad::ErrorStatus AcDbCrossCircle::subMoveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
  // TODO: ������� subMoveGripPointsAt()
  return Acad::eNotImplementedYet;

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

