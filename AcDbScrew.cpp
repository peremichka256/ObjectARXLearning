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
    setBodyDiameter(eFifth);
    setBodyLength(eFifth * 5);
    setTransition(eFifth / 5);
}

//Деструктор
//////////////////////////////////////////////////////////////////////////
AcDbScrew::~AcDbScrew()
{
    _gripsData.clear();

    for (AcDb3dSolid* entity : _cachedGeometry)
    {
        delete entity;
    }
    _cachedGeometry.clear();
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
    _isGeometryActual = false;
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::setDirection(AcGeVector3d direction)
{
    assertWriteEnabled();
    _direction = direction.normal();
    _normal = _direction.crossProduct(_direction.normal().perpVector());
    _isGeometryActual = false;
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::setNormal(AcGeVector3d normal)
{
    assertWriteEnabled();
    _normal = normal.normal();
    _isGeometryActual = false;
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::setBodyLength(double bodyLength)
{
    assertWriteEnabled();
    _bodyLength = bodyLength;
    _isGeometryActual = false;
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::setBodyDiameter(double bodyDiameter)
{
    assertWriteEnabled();

    double* abs = new double[diameterValues.size()];

    for (int i = 0; i < diameterValues.size(); i++)
    {
        abs[i] = std::abs(bodyDiameter - diameterValues[i]);
    }

    int nearestDiameterIndex = 0;
    for (int i = 0; i < diameterValues.size() - 1; i++)
    {
        if (abs[i + 1] < abs[i])
        {
            nearestDiameterIndex = i + 1;
        }
    }

    switch (nearestDiameterIndex)
    {
    case 0:
        bodyDiameter = diameterValues[0];
        break;
    case 1:
        bodyDiameter = diameterValues[1];
        break;
    case 2:
        bodyDiameter = diameterValues[2];
        break;
    case 3:
        bodyDiameter = diameterValues[3];
        break;
    case 4:
        bodyDiameter = diameterValues[4];
        break;
    case 5:
        bodyDiameter = diameterValues[5];
        break;
    case 6:
        bodyDiameter = diameterValues[6];
        break;
    case 7:
        bodyDiameter = diameterValues[7];
        break;
    case 8:
        bodyDiameter = diameterValues[8];
        break;
    case 9:
        bodyDiameter = diameterValues[9];
        break;
    }
    _headDiameter = bodyDiameter * HEAD_DIAMETER_TO_BODY_DIAMETER_KOEF;
    _bodyDiameter = bodyDiameter;
    _isGeometryActual = false; 
    delete abs;
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::setTransition(double transition)
{
    assertWriteEnabled();
    _transition = transition;
    _isGeometryActual = false;
    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
void AcDbScrew::reBuild()
{
    if (!_cachedGeometry[eBody])
    {
        delete _cachedGeometry[eBody];
    }
    if (!_cachedGeometry[eThread])
    {
        delete _cachedGeometry[eThread];
    }

    //Тело
    AcDb3dSolid* body = new AcDb3dSolid();
    AcDbCircle circle;
    circle.setCenter(center());
    circle.setNormal(direction());
    circle.setRadius(bodyDiameter() / 2);
    AcDbSweepOptions options;
    body->createExtrudedSolid(&circle, -direction() * bodyLength(), options);
    _cachedGeometry[eBody] = body;

    //Резьба
    AcDb3dSolid* thread = new AcDb3dSolid();
    thread->createFrustum(transition(), bodyDiameter() / 2,
        bodyDiameter() / 2, bodyDiameter() / 2 - transition());
    AcGePoint3d threadCenter = center() - direction() * (bodyLength() + transition() / 2);
    //TODO: добавить поддержу матрицы пользовательской системы координат
    AcGeMatrix3d matrix;
    matrix.setToTranslation(threadCenter.asVector());
    thread->transformBy(matrix);
    AcGeVector3d rotationAxis = normal().crossProduct(direction());
    rotationAxis.normalize();
    matrix.setToRotation(-PI / 2, rotationAxis, threadCenter);
    thread->transformBy(matrix);
    _cachedGeometry[eThread] = thread;

    _isGeometryActual = true;

    if (_isLengthChanged)
    {
        _isLengthChanged = false;
        return;
    }
    else
    {
        if (!_cachedGeometry[eHead])
        {
            delete _cachedGeometry[eHead];
        }

        //Шляпка
        AcDb3dSolid* head = new AcDb3dSolid();
        head->createSphere(headDiameter() / 2);

        matrix.setToTranslation(center().asVector());
        head->transformBy(matrix);
        circle.setRadius(headDiameter() / 2);
        AcDb3dSolid barier;
        barier.createExtrudedSolid(&circle, -direction() * headDiameter() / 2, options);
        head->booleanOper(AcDb::kBoolSubtract, &barier);
        _cachedGeometry[eHead] = head;
    }
}

//////////////////////////////////////////////////////////////////////////
Adesk::Boolean AcDbScrew::subWorldDraw(AcGiWorldDraw* pWD)
{
    if (!_isGeometryActual) 
    {
        reBuild();
    }

    for (AcDb3dSolid* entity : _cachedGeometry)
    {
        pWD->geometry().draw(entity->drawable());
    }

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
bool scaleGripWorldDraw(AcDbGripData* pThis, 
    AcGiWorldDraw* pWd,
    const AcDbObjectId& entId,
    AcDbGripOperations::DrawType type,
    AcGePoint3d* imageGripPoint,
    double dGripSize)
{
    NcGePoint3d gripPoint = pThis->gripPoint();
    AcDbEntity* pEntity = nullptr;
    Acad::ErrorStatus es = acdbOpenAcDbEntity(pEntity, entId, AcDb::kForRead);
    AcDbScrew* screw = static_cast<AcDbScrew*>(pEntity);
    screw->direction();

    dGripSize *= 1.5;

    AcGePoint3d firstTriangle[3];
    gripPoint -= screw->direction().perpVector() * dGripSize;
    firstTriangle[0] = gripPoint + screw->direction() * dGripSize;
    firstTriangle[1] = gripPoint - screw->direction().perpVector()
        * (dGripSize * sqrt(3));
    firstTriangle[2] = gripPoint - screw->direction() * dGripSize;
    pWd->subEntityTraits().setFillType(kAcGiFillAlways);
    pWd->geometry().polygon(3, firstTriangle);

    AcGePoint3d secondTriangle[3];
    gripPoint += screw->direction().perpVector() * dGripSize * 2;
    secondTriangle[0] = gripPoint + screw->direction() * dGripSize;
    secondTriangle[1] = gripPoint + screw->direction().perpVector()
        * (dGripSize * sqrt(3));
    secondTriangle[2] = gripPoint - screw->direction() * dGripSize;
    pWd->geometry().polygon(3, secondTriangle);

    AcGePoint3d thirdTriangle[3];
    gripPoint = pThis->gripPoint();
    gripPoint += screw->direction() * dGripSize;
    thirdTriangle[0] = gripPoint
        + screw->direction().perpVector() * dGripSize;
    thirdTriangle[1] = gripPoint + screw->direction()
        * (dGripSize * sqrt(3));
    thirdTriangle[2] = gripPoint
        - screw->direction().perpVector() * dGripSize;
    pWd->geometry().polygon(3, thirdTriangle);

    AcGePoint3d fourthTriangle[3];
    gripPoint -= screw->direction() * dGripSize * 2;
    fourthTriangle[0] = gripPoint
        + screw->direction().perpVector() * dGripSize;
    fourthTriangle[1] = gripPoint - screw->direction()
        * (dGripSize * sqrt(3));
    fourthTriangle[2] = gripPoint
        - screw->direction().perpVector() * dGripSize;
    pWd->geometry().polygon(3, fourthTriangle);

    return true;
}

//////////////////////////////////////////////////////////////////////////
bool lengthGripWorldDraw(AcDbGripData* pThis, 
    AcGiWorldDraw* pWd,
    const AcDbObjectId& entId, 
    AcDbGripOperations::DrawType type,
    AcGePoint3d* imageGripPoint, 
    double dGripSize)
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
bool rotateGripWorldDraw(AcDbGripData* pThis,
    AcGiWorldDraw* pWd, 
    const AcDbObjectId& entId,
    AcDbGripOperations::DrawType type,
    AcGePoint3d* imageGripPoint, 
    double dGripSize)
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
const ACHAR* rotateGripToolTip(AcDbGripData* pThis)
{
    return L"Ручка для вращения";
}

//////////////////////////////////////////////////////////////////////////
AcGeVector3d setDimValueCbackFunc(AcDbDimData* pDimData,
    AcDbEntity* pEnt,
    double newValue,
    const AcGeVector3d& offset)
{
    AcGeVector3d newOffset(offset);

    if ((pDimData == NULL) || (pEnt == NULL)) {
        return newOffset;
    }

    AcDbScrew* pScrew = AcDbScrew::cast(pEnt);
    if (pScrew == NULL) {
        return newOffset;
    }

    pScrew->setBodyLength(newValue);
    pScrew->_isLengthChanged = true;

    return newOffset;
}

//////////////////////////////////////////////////////////////////////////
void lengthDimensionFunc(AcDbGripData* pThis,
    const  AcDbObjectId& entId,
    double  dimScale,
    AcDbDimDataPtrArray& dimDataArr)
{
    AcDbEntity* pEnt;
    acdbOpenAcDbEntity(pEnt, entId, AcDb::kForRead);
    AcDbScrew* pScrew = AcDbScrew::cast(pEnt);

    AcGePoint3d center = pScrew->center();
    AcGePoint3d screwEnd = pScrew->center() 
        - pScrew->direction() 
        * pScrew->bodyLength();

    AcDbAlignedDimension* pAlignedDim
        = new  AcDbAlignedDimension();

    pAlignedDim->setDatabaseDefaults();

    pAlignedDim->setDimsah(true);
    pAlignedDim->setDimse1(true);
    pAlignedDim->setDimblk1(_T("None"));
    pAlignedDim->setNormal(pScrew->normal());
    pAlignedDim->setElevation(0.0);
    pAlignedDim->setHorizontalRotation(0.0);
    pAlignedDim->setXLine1Point(center);
    pAlignedDim->setXLine2Point(screwEnd);
    pAlignedDim->setDimLinePoint(center 
        + pScrew->direction().perpVector() 
        * pScrew->headDiameter() / 2);
    pAlignedDim->setDynamicDimension(true);
    pAlignedDim->setColorIndex(1);
    pScrew->close();

    AcDbDimData* pDimData
        = new  AcDbDimData(pAlignedDim);

    pDimData->setOwnerId(entId);
    pDimData->setDimFocal(true);
    pDimData->setDimHideIfValueIsZero(true);
    pDimData->setDimEditable(true);
    pDimData->setDimValueFunc(setDimValueCbackFunc);

    dimDataArr.append(pDimData); 
    pScrew->close();
}

//////////////////////////////////////////////////////////////////////////
Acad::ErrorStatus AcDbScrew::subGetGripPoints(AcDbGripDataPtrArray& grips,
    const double curViewUnitSize,
    const int gripSize,
    const AcGeVector3d& curViewDir,
    const int bitflags) const
{
    assertReadEnabled();

    // Грип на базовой точке винта
    auto centerGrip = make_unique<AcDbGripData>();
    centerGrip->setGripPoint(_center);
    _gripsData[eCenter] = new GripData{ eCenter };
    centerGrip->setAppData(_gripsData[eCenter]);
    grips.append(centerGrip.release());

    // Грип за пределами винта
    auto rotateGrip = make_unique<AcDbGripData>();
    rotateGrip->setGripPoint(_center - _direction * (_bodyLength + _transition * 5));
    _gripsData[eRotate] = new GripData{ eRotate };
    rotateGrip->setAppData(_gripsData[eRotate]);
    rotateGrip->setWorldDraw(rotateGripWorldDraw);
    rotateGrip->setToolTipFunc(rotateGripToolTip);
    grips.append(rotateGrip.release());

    // Грип на конце винта
    auto lengthGrip = make_unique<AcDbGripData>();
    lengthGrip->setGripPoint(_center - _direction * (_bodyLength + _transition));
    _gripsData[eLength] = new GripData{ eLength };
    lengthGrip->setAppData(_gripsData[eLength]);
    lengthGrip->setWorldDraw(lengthGripWorldDraw);
    lengthGrip->setHoverDimensionFunc(lengthDimensionFunc);
    lengthGrip->setHotGripDimensionFunc(lengthDimensionFunc);
    grips.append(lengthGrip.release());

    // Грипы на боковые стороны винта
    auto scaleGripFirst = make_unique<AcDbGripData>();
    scaleGripFirst->setGripPoint(_center - (_direction * _bodyLength / 2)
        - _direction.perpVector() * (_bodyDiameter / 2));
    _gripsData[eScale] = new GripData{ eScale };
    scaleGripFirst->setAppData(_gripsData[eScale]);
    scaleGripFirst->setWorldDraw(scaleGripWorldDraw);
    grips.append(scaleGripFirst.release());

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
        GripData* grip = static_cast<GripData*>(appData[i]);

        switch (grip->index)
        {
        case eCenter:
        {
            setCenter(center() + offset);
        }
        break;
        case eRotate:
        {
            setDirection(((bodyLength() + headDiameter() / 2) / 2 + transition())
                * (direction() - offset).normalize());
        }
        break;
        case eScale:
        {
            double oldDiameter = bodyDiameter();
            double newDiameter = (offset - (direction() * bodyDiameter() / 2)).length();
            double keof = newDiameter / oldDiameter;

            setBodyDiameter(bodyDiameter() * keof);
        }
        break;
        }
    }

    return Acad::eOk;
}

//////////////////////////////////////////////////////////////////////////
void AcDbScrew::subGripStatus(const AcDb::GripStat status)
{
    if (status == AcDb::kGripsToBeDeleted)
    {
        for (auto* gripData : _gripsData) 
        {
            if (gripData != nullptr) 
            {
                delete gripData;
            }
        }
    }
};

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
                setBodyDiameter(bodyDiameter() * keof);
                setTransition(transition() * keof);
            }
            break;
        }
    }
    return Acad::eOk;
}