#include "pch.h"
#include "AcDbScrew.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 AcDbScrew::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
    AcDbScrew,
    AcDbEntity,
    AcDb::kDHL_CURRENT, 
    AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    ACDBSCREW,
    SCREWAPP
    | Product Desc : A description for your object
    | Company : Your company name
    | WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------

//Стандартный конструктор
//////////////////////////////////////////////////////////////////////////
AcDbScrew::AcDbScrew() : AcDbEntity()
{
    setBodyLength(6000);
    setHeadHeight(1000);
    setHeadDiameter(2000);
    setBodyDiameter(1000);
    setTransition(200);
}

//Деструктор
//////////////////////////////////////////////////////////////////////////
AcDbScrew::~AcDbScrew()
{

}

// Методы чтения и записи параметров
//Геттеры
//////////////////////////////////////////////////////////////////////////
AcGePoint3d AcDbScrew::center()
{
    assertReadEnabled();
    return _center;
}

//////////////////////////////////////////////////////////////////////////
AcGeVector3d AcDbScrew::direction()
{
    assertReadEnabled();
    return _direction;
}

//////////////////////////////////////////////////////////////////////////
AcGeVector3d AcDbScrew::normal()
{
    assertReadEnabled();
    return _normal;
}

//////////////////////////////////////////////////////////////////////////
double AcDbScrew::bodyLength()
{
    assertReadEnabled();
    return _bodyLength;
}

//////////////////////////////////////////////////////////////////////////
double AcDbScrew::headHeight()
{
    assertReadEnabled();
    return _headHeight;
}

//////////////////////////////////////////////////////////////////////////
double AcDbScrew::headDiameter()
{
    assertReadEnabled();
    return _headDiameter;
}

//////////////////////////////////////////////////////////////////////////
double AcDbScrew::bodyDiameter()
{
    assertReadEnabled();
    return _bodyDiameter;
}

//////////////////////////////////////////////////////////////////////////
double AcDbScrew::transition()
{
    assertReadEnabled();
    return _transition;
}

//Сеттеры
//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::setCenter(AcGePoint3d center)
{
    assertWriteEnabled();
    _center = center;
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::setDirection(AcGeVector3d direction)
{
    assertWriteEnabled();
    _direction = direction.normal();
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::setNormal(AcGeVector3d normal)
{
    assertWriteEnabled();
    _normal = normal.normal();
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::setBodyLength(double bodyLength)
{
    assertWriteEnabled();
    _bodyLength = bodyLength;
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::setHeadHeight(double headHeight)
{
    assertWriteEnabled();
    _headHeight = headHeight;
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::setHeadDiameter(double headDiameter)
{
    assertWriteEnabled();
    _headDiameter = headDiameter;
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::setBodyDiameter(double bodyDiameter)
{
    assertWriteEnabled();
    _bodyDiameter = bodyDiameter;
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::setTransition(double transition)
{
    assertWriteEnabled();
    _transition = transition;
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Adesk::Boolean AcDbScrew::subWorldDraw(AcGiWorldDraw* pWD)
{
    //Основание
    double totalLength = bodyLength() + headHeight();
    _normal = direction().crossProduct(direction().normal().perpVector());
    AcGePoint3d bodyPoints[4];

    AcGePoint3d tempPoint = center() + direction().perpVector() * bodyDiameter() / 2;
    bodyPoints[0] = tempPoint + direction() * (totalLength / 2 - headHeight());
    bodyPoints[1] = tempPoint - direction() * totalLength / 2;
    tempPoint = center() - direction().perpVector() * bodyDiameter() / 2;
    bodyPoints[2] = tempPoint - direction() * totalLength / 2;
    bodyPoints[3] = tempPoint + direction() * (totalLength / 2 - headHeight());
    pWD->geometry().polyline(4, bodyPoints);

    //Конец
    AcGePoint3d transitionPoints[4];
    AcGeVector3d transitionVector = direction();
    transitionPoints[0] = bodyPoints[2];
    transitionPoints[1] = transitionPoints[0] - transitionVector.rotateBy(-PI / 4, normal())
        * sqrt(2 * pow(transition(), 2));
    transitionPoints[3] = bodyPoints[1];
    transitionPoints[2] = bodyPoints[1] - transitionVector.rotateBy(PI / 2, normal())
        * sqrt(2 * pow(transition(), 2));
    pWD->geometry().polyline(4, transitionPoints);

    //Шляпка
    AcGePoint3d headPoints[2];
    tempPoint = center() + direction() * (totalLength / 2 - headHeight());
    headPoints[0] = tempPoint + direction().perpVector() * headDiameter() / 2;
    headPoints[1] = tempPoint - direction().perpVector() * headDiameter() / 2;
    pWD->geometry().polyline(2, headPoints);
    pWD->geometry().circularArc(center() + direction() * (totalLength / 2 - headHeight()),
        headHeight(), normal(), headPoints[1] - headPoints[0], PI);

    return Adesk::kTrue;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::dwgOutFields(AcDbDwgFiler* pFiler) const
{
    assertReadEnabled();

    // Сохранение данных базового класса
    Acad::ErrorStatus es = AcDbEntity::dwgOutFields(pFiler);
    if (es != Acad::eOk)
        return es;

    // Версия объекта
    if ((es = pFiler->writeUInt32(AcDbScrew::kCurrentVersionNumber)) != Acad::eOk)
        return es;

    pFiler->writePoint3d(_center);
    pFiler->writeVector3d(_direction);
    pFiler->writeVector3d(_normal);
    pFiler->writeDouble(_bodyLength);
    pFiler->writeDouble(_headHeight);
    pFiler->writeDouble(_headDiameter);
    pFiler->writeDouble(_bodyDiameter);
    pFiler->writeDouble(_transition);

    return pFiler->filerStatus();
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::dwgInFields(AcDbDwgFiler* pFiler)
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

    if (version > AcDbScrew::kCurrentVersionNumber) // Если версия объекта больше текущей
        return Acad::eMakeMeProxy;                         // то создать прокси-объект

    // Чтение параметров
    pFiler->readPoint3d(&_center);
    pFiler->readVector3d(&_direction);
    pFiler->readVector3d(&_normal);
    pFiler->readDouble(&_bodyLength);
    pFiler->readDouble(&_headHeight);
    pFiler->readDouble(&_headDiameter);
    pFiler->readDouble(&_bodyDiameter);
    pFiler->readDouble(&_transition);

    return pFiler->filerStatus();
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::subTransformBy(const AcGeMatrix3d& xform)
{
    assertWriteEnabled();

    _center.transformBy(xform);
    _direction.transformBy(xform);
    _normal.transformBy(xform);

    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::subGetGripPoints(AcGePoint3dArray& gripPoints,
    AcDbIntArray& osnapModes, AcDbIntArray& geomIds) const
{
    assertReadEnabled();
    AcGePoint3dArray aScrew;
    aScrew.setLogicalLength(4);
    
    aScrew[0] = _center;
    aScrew[1] = _center - _direction * ((_bodyLength + _headHeight) / 2 + _transition);
    aScrew[2] = _center - _direction.perpVector() * (_bodyDiameter / 2);
    aScrew[3] = _center + _direction.perpVector() * (_bodyDiameter / 2);

    gripPoints.append(aScrew);

    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::subMoveGripPointsAt(const AcDbIntArray& indices,
    const AcGeVector3d& offset)
{
    if (indices.length() == 0 || offset.isZeroLength())
        return Acad::eOk;
    
    assertWriteEnabled();
    
    
    for (int i = 0; i < indices.length(); i++)
    {
        switch (indices[i])
        {
            case 0:
            {
                this->setCenter(_center + offset);
            }
            break;
            case 1:
            {
                this->setDirection(((_bodyLength + _headHeight) / 2 + _transition)
                    * (_direction - offset).normalize());
            }
            break;
            case 2:
            case 3:
            {
                double oldDiameter = bodyDiameter();
                double newDiameter = (offset - (direction() * bodyDiameter() / 2)).length();
                double keof = newDiameter / oldDiameter;

                setBodyLength(bodyLength() * keof);
                setHeadHeight(headHeight() * keof);
                setHeadDiameter(headDiameter() * keof);
                setBodyDiameter(bodyDiameter() * keof);
                setTransition(transition() * keof);
            }
            break;
        }
    }
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::subGetGripPoints(AcDbGripDataPtrArray& grips,
    const double curViewUnitSize,
    const int gripSize,
    const AcGeVector3d& curViewDir,
    const int bitflags) const
{
    assertReadEnabled();
    return Acad::eNotImplementedYet;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::subMoveGripPointsAt(const AcDbVoidPtrArray& appData,
    const AcGeVector3d& offset,
    const int bitflags)
{
    assertWriteEnabled();
    return Acad::eNotImplementedYet;
}