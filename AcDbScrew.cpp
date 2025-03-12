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
    setDirection(AcGeVector3d::kYAxis);
    setBodyLength(4000);
    setHeadDiameter(eFifth);
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
    _normal = direction().crossProduct(direction().normal().perpVector());
    AcGePoint3d bodyPoints[4];

    //Тело
    AcGePoint3d bodyPoint = center() + direction().perpVector() * bodyDiameter() / 2;
    bodyPoints[0] = bodyPoint;
    bodyPoints[1] = bodyPoint - direction() * bodyLength();
    bodyPoint = center() - direction().perpVector() * bodyDiameter() / 2;
    bodyPoints[2] = bodyPoint - direction() * bodyLength();
    bodyPoints[3] = bodyPoint;
    pWD->geometry().polyline(4, bodyPoints);

    //Шляпка
    AcGePoint3d headPoints[2];
    headPoints[0] = center() + direction().perpVector() * headDiameter() / 2;
    headPoints[1] = center() - direction().perpVector() * headDiameter() / 2;
    pWD->geometry().polyline(2, headPoints);
    pWD->geometry().circularArc(center(), headDiameter() / 2,
        normal(), headPoints[1] - headPoints[0], PI);

    //Начало резьбы
    AcGePoint3d transitionPoints[4];
    AcGeVector3d transitionVector = direction();
    transitionPoints[0] = bodyPoints[1];
    transitionPoints[1] = transitionPoints[0] - transitionVector.rotateBy(PI / 4, normal())
            * sqrt(2 * pow(transition(), 2));
    transitionPoints[3] = bodyPoints[2];
    transitionPoints[2] = transitionPoints[3] - transitionVector.rotateBy(-PI / 2, normal())
            * sqrt(2 * pow(transition(), 2));
    pWD->geometry().polyline(4, transitionPoints);

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
struct GripData
{
    int index;
};

//////////////////////////////////////////////////////////////////////////
bool scaleGripWorldDraw(AcDbGripData* pThis, AcGiWorldDraw* pWd, const AcDbObjectId& entId, AcDbGripOperations::DrawType type, AcGePoint3d* imageGripPoint, double dGripSize)
{
    NcGePoint3d gripPoint = pThis->gripPoint(); 
    AcDbEntity* pEntity = nullptr;
    Acad::ErrorStatus es = acdbOpenAcDbEntity(pEntity, entId, AcDb::kForRead);
    AcDbScrew* screw = static_cast<AcDbScrew*>(pEntity);
    screw->direction();
    
    dGripSize *= 1.5;

    AcGePoint3d firstTriangle[4];
    gripPoint -= screw->direction().perpVector() * (dGripSize / 2);
    firstTriangle[0] = gripPoint + screw->direction() * dGripSize;
    firstTriangle[1] = gripPoint - screw->direction().perpVector() * (dGripSize * sqrt(3));
    firstTriangle[2] = gripPoint - screw->direction() * dGripSize;
    pWd->subEntityTraits().setFillType(kAcGiFillAlways);
    pWd->geometry().polygon(3, firstTriangle);

    AcGePoint3d secondTriangle[4];
    gripPoint += screw->direction().perpVector() * dGripSize;
    secondTriangle[0] = gripPoint + screw->direction() * dGripSize;
    secondTriangle[1] = gripPoint + screw->direction().perpVector() 
        * (dGripSize * sqrt(3));
    secondTriangle[2] = gripPoint - screw->direction() * dGripSize;

    pWd->geometry().polygon(3, secondTriangle);
    return true;
}

//////////////////////////////////////////////////////////////////////////
bool lengthGripWorldDraw(AcDbGripData* pThis, AcGiWorldDraw* pWd, const AcDbObjectId& entId, AcDbGripOperations::DrawType type, AcGePoint3d* imageGripPoint, double dGripSize)
{
    NcGePoint3d gripPoint = pThis->gripPoint();
    AcDbEntity* pEntity = nullptr;
    Acad::ErrorStatus es = acdbOpenAcDbEntity(pEntity, entId, AcDb::kForRead);
    AcDbScrew* screw = static_cast<AcDbScrew*>(pEntity);
    screw->direction();

    dGripSize *= 1.5;
    AcGePoint3d gripPoints[3];
    gripPoints[0] = gripPoint + screw->direction().perpVector() * dGripSize;
    gripPoints[1] = gripPoint - screw->direction() * (dGripSize * sqrt(3));
    gripPoints[2] = gripPoint - screw->direction().perpVector() * dGripSize;

    pWd->subEntityTraits().setFillType(kAcGiFillAlways);
    pWd->geometry().polygon(3, gripPoints);
    pEntity->close();
    return true;
}

//////////////////////////////////////////////////////////////////////////
bool rotateGripWorldDraw(AcDbGripData* pThis, AcGiWorldDraw* pWd, const AcDbObjectId& entId, AcDbGripOperations::DrawType type, AcGePoint3d* imageGripPoint, double dGripSize)
{

    AcGePoint3d center = pThis->gripPoint();
    double innerRadius = dGripSize;
    double outerRadius = 2 * dGripSize;

    AcDbEntity* pEntity = nullptr;
    Acad::ErrorStatus es = acdbOpenAcDbEntity(pEntity, entId, AcDb::kForRead);
    AcDbScrew* screw = static_cast<AcDbScrew*>(pEntity);

    pWd->geometry().circle(center, innerRadius, screw->normal());
    pWd->geometry().circle(center, outerRadius, screw->normal());

    const int numSegments = 37; 
    AcGePoint3d outerPoints[numSegments];
    AcGePoint3d innerPoints[numSegments];

    for (int i = 0; i < numSegments - 1; i++)
    {
        double angle = 2.0 * PI * i / numSegments;
        outerPoints[i] = center + AcGeVector3d(outerRadius 
            * cos(angle), outerRadius * sin(angle), 0);
        innerPoints[i] = center + AcGeVector3d(innerRadius 
            * cos(angle), innerRadius * sin(angle), 0);
    }
    outerPoints[numSegments - 1] = outerPoints[0];
    innerPoints[numSegments - 1] = innerPoints[0];

    AcGePoint3d ringPoints[numSegments * 2];
    for (int i = 0; i < numSegments; i++)
    {
        ringPoints[i] = outerPoints[i];
        ringPoints[numSegments * 2 - 1 - i] = innerPoints[i];
    }

    pWd->subEntityTraits().setFillType(kAcGiFillAlways);
    pWd->geometry().polygon(numSegments * 2, ringPoints);
    pEntity->close();

    return true;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::subGetGripPoints(AcDbGripDataPtrArray& grips,
    const double curViewUnitSize,
    const int gripSize,
    const AcGeVector3d& curViewDir,
    const int bitflags) const
{
    assertReadEnabled();

    // Грип в центр винта
    auto centerGrip = make_unique<AcDbGripData>();
    centerGrip->setGripPoint(_center);
    centerGrip->setAppData(new GripData{ eGripIndex::eCenter });
    grips.append(centerGrip.release());

    // Грип за пределами винта
    auto rotateGrip = make_unique<AcDbGripData>();
    rotateGrip->setGripPoint(_center - _direction * (_bodyLength + _transition * 5));
    rotateGrip->setAppData(new GripData{ eGripIndex::eRotate });
    rotateGrip->setWorldDraw(rotateGripWorldDraw);
    grips.append(rotateGrip.release());

    // Грип на конце винта
    auto lengthGrip = make_unique<AcDbGripData>();
    lengthGrip->setGripPoint(_center - _direction * (_bodyLength + _transition + gripSize));
    lengthGrip->setAppData(new GripData{ eGripIndex::eLength });
    lengthGrip->setWorldDraw(lengthGripWorldDraw);
    grips.append(lengthGrip.release());

    // Грипы на боковые стороны винта
    auto scaleGripFirst = make_unique<AcDbGripData>();
    scaleGripFirst->setGripPoint(_center - (_direction * _bodyLength / 2)
        - _direction.perpVector() * (_bodyDiameter / 2));
    scaleGripFirst->setAppData(new GripData{ eGripIndex::eScaleFirst });
    scaleGripFirst->setWorldDraw(scaleGripWorldDraw);
    grips.append(scaleGripFirst.release());

    auto scaleGripSecond = make_unique<AcDbGripData>();
    scaleGripSecond->setGripPoint(_center - (_direction * _bodyLength / 2)
        + _direction.perpVector() * (_bodyDiameter / 2));
    scaleGripSecond->setAppData(new GripData{ eGripIndex::eScaleSecond });
    scaleGripSecond->setWorldDraw(scaleGripWorldDraw);
    grips.append(scaleGripSecond.release());

    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::subMoveGripPointsAt(const AcDbVoidPtrArray& appData,
    const AcGeVector3d& offset,
    const int bitflags)
{
    assertWriteEnabled();

    if (appData.length() == 0 || offset.isZeroLength())
        return Acad::eOk;

    for (int i = 0; i < appData.length(); i++)
    {
        GripData* gripIndex = static_cast<GripData*>(appData[i]);

        switch (gripIndex->index)
        {
        case eGripIndex::eCenter:
        {
            setCenter(center() + offset);
        }
        break;
        case eGripIndex::eRotate:
        {
            setDirection(((bodyLength() + headDiameter() / 2) / 2 + transition())
                * (direction() - offset).normalize());
        }
        break;
        case eGripIndex::eLength:
        {
            double oldDiameter = bodyLength();
            double newDiameter = (offset - (direction() * bodyLength())).length();
            double koef = newDiameter / oldDiameter;
            setBodyLength(bodyLength() * koef);
        }
        break;
        case eGripIndex::eScaleFirst:
        case eGripIndex::eScaleSecond:
        {
            double oldDiameter = bodyDiameter();
            double newDiameter = (offset - (direction() * bodyDiameter() / 2)).length();
            double keof = newDiameter / oldDiameter;

            setBodyLength(bodyLength() * keof);
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
//Рудимент
Acad::ErrorStatus AcDbScrew::subGetGripPoints(AcGePoint3dArray& gripPoints,
    AcDbIntArray& osnapModes, AcDbIntArray& geomIds) const
{
    assertReadEnabled();
    AcGePoint3dArray aScrew;
    aScrew.setLogicalLength(4);
    
    aScrew[0] = _center;
    aScrew[1] = _center - _direction * ((_bodyLength + _headDiameter / 2) / 2 + _transition);
    aScrew[2] = _center - _direction.perpVector() * (_bodyDiameter / 2);
    aScrew[3] = _center + _direction.perpVector() * (_bodyDiameter / 2);
    
    gripPoints.append(aScrew);

    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
//Рудимент
Acad::ErrorStatus AcDbScrew::subMoveGripPointsAt(const AcDbIntArray& indices,
    const AcGeVector3d& offset)
{
    assertWriteEnabled();
    if (indices.length() == 0 || offset.isZeroLength())
        return Acad::eOk;
    
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
                this->setDirection(((_bodyLength + _headDiameter / 2) / 2 + _transition)
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
                setHeadDiameter(headDiameter() * keof);
                setBodyDiameter(bodyDiameter() * keof);
                setTransition(transition() * keof);
            }
            break;
        }
    }
    return Acad::eOk;
}