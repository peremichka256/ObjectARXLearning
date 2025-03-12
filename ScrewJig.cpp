#include "pch.h"
#include "ScrewJig.h"

ScrewJig::ScrewJig() : AcEdJig(),
mCurrentInputLevel(0), screw(NULL)
{

}

ScrewJig::~ScrewJig()
{

}

AcEdJig::DragStatus ScrewJig::startJig(AcDbScrew* pEntity)
{
    screw = pEntity;
    ACHAR* inputPrompts[4] =
    {
      _T("\nПоставьте базовую точку: "),
      _T("\nВыберите направления винта: "),
      _T("\nВведите длину винта: "),
      _T("\nВыберите тип шляпки: ")
    };

    bool appendOk = true;
    AcEdJig::DragStatus status = AcEdJig::kNull;

    for (mCurrentInputLevel = 0; mCurrentInputLevel < 4; mCurrentInputLevel++)
    {
        mInputPoints.append(AcGePoint3d());
        setDispPrompt(inputPrompts[mCurrentInputLevel]);
        bool quit = false;
        status = drag();

        if (status != kNormal)
        {
            switch (status)
            {
            case kCancel:
            case kNull:
                quit = true;
                appendOk = false;
                break;
            }
        }
        else
        {
            appendOk = true;
        }

        if (quit)
        {
            break;
        }
    }

    if (appendOk)
    {
        append();
    }
    else
    {
        delete screw;
    }

    return status;
}

AcEdJig::DragStatus ScrewJig::sampler()
{
    AcEdJig::UserInputControls userInputControls[4] =
    {
        (UserInputControls)(AcEdJig::kAccept3dCoordinates
            | AcEdJig::kNoNegativeResponseAccepted),
        (UserInputControls)(AcEdJig::kAccept3dCoordinates
            | AcEdJig::kGovernedByOrthoMode
            | AcEdJig::kAcceptOtherInputString
            | AcEdJig::kNoNegativeResponseAccepted
            | AcEdJig::kNullResponseAccepted
            | AcEdJig::kNoZeroResponseAccepted),
        (UserInputControls)(AcEdJig::kAccept3dCoordinates
            | AcEdJig::kGovernedByOrthoMode
            | AcEdJig::kAcceptOtherInputString
            | AcEdJig::kNoNegativeResponseAccepted
            | AcEdJig::kNullResponseAccepted
            | AcEdJig::kNoZeroResponseAccepted),
        (UserInputControls)(AcEdJig::kAccept3dCoordinates
            | AcEdJig::kGovernedByOrthoMode
            | AcEdJig::kAcceptOtherInputString
            | AcEdJig::kNoNegativeResponseAccepted
            | AcEdJig::kNullResponseAccepted
            | AcEdJig::kNoZeroResponseAccepted)
    };
    setUserInputControls(userInputControls[mCurrentInputLevel]);

    AcEdJig::DragStatus status = AcEdJig::kCancel;
    switch (mCurrentInputLevel + 1)
    {
    case 1:
        status = GetStartPoint();
        break;
    case 2:
        status = GetDirectionPoint();
        break;
    case 3:
        status = GetLength();
        break;
    case 4:
        status = GetHeadDiameter();
        break;
    default:
        break;
    }
    return status;
}

Adesk::Boolean ScrewJig::update()
{
    switch (mCurrentInputLevel + 1)
    {
        case 1:
        {
            AcGeMatrix3d UcsToWcsMat;
            acdbUcsMatrix(UcsToWcsMat);

            AcGePoint3d orgPt;
            AcGeVector3d xAxis, yAxis, zAxis;
            UcsToWcsMat.getCoordSystem(orgPt, xAxis, yAxis, zAxis);

            screw->setCenter(mInputPoints[mCurrentInputLevel]);
            screw->setNormal(screw->direction().crossProduct(screw->direction().normal().perpVector()));
        }
        break;
        case 2:
        {
            AcGeVector3d acqVector = mInputPoints[mCurrentInputLevel] - mInputPoints[mCurrentInputLevel - 1];

            if (acqVector.isZeroLength())
            {
                acqVector = screw->direction();
            }

            screw->setDirection(acqVector);
        }
        break;
        case 3:
        {
            screw->setBodyLength(_length);
        }
        break;
        case 4:
        {
            screw->setHeadDiameter(_headDiameter);
        }
        break;
        default:
            break;
    }
    return Acad::eOk;
}

AcDbEntity* ScrewJig::entity() const
{
    return ((AcDbEntity*)screw);
}

AcEdJig::DragStatus ScrewJig::GetStartPoint()
{
    AcGePoint3d newPoint;
    AcEdJig::DragStatus status = acquirePoint(newPoint);

    if (status == AcEdJig::kNormal)
    {
        if (newPoint.isEqualTo(mInputPoints[mCurrentInputLevel]))
        {
            return AcEdJig::kNoChange;
        }

        mInputPoints[mCurrentInputLevel] = newPoint;
    }
    return status;
}

AcEdJig::DragStatus ScrewJig::GetDirectionPoint()
{
    AcGePoint3d newPoint;
    AcEdJig::DragStatus status = acquirePoint(newPoint, mInputPoints[mCurrentInputLevel - 1]);

    if (status == AcEdJig::kNormal)
    {
        if (newPoint.isEqualTo(mInputPoints[mCurrentInputLevel]))
        {
            return AcEdJig::kNoChange;
        }

        mInputPoints[mCurrentInputLevel] = newPoint;
    }
    return status;
}

AcEdJig::DragStatus ScrewJig::GetLength()
{
    double length;
    AcEdJig::DragStatus status = acquireDist(length, mInputPoints[mCurrentInputLevel - 2]);

    if (status == AcEdJig::kNormal)
    {
        _length = length;
    }
    return status;
}

AcEdJig::DragStatus ScrewJig::GetHeadDiameter()
{
    std::wstring kwds = to_wstring(screw->eFirst) + L" "
        + to_wstring(screw->eSecond) + L" "
        + to_wstring(screw->eThird) + L" "
        + to_wstring(screw->eFourth) + L" "
        + to_wstring(screw->eFifth) + L" "
        + to_wstring(screw->eSixth) + L" "
        + to_wstring(screw->eSeventh) + L" "
        + to_wstring(screw->eEighth) + L" "
        + to_wstring(screw->eNinth) + L" "
        + to_wstring(screw->eTenth);
    acedInitGet(0, kwds.c_str());

    std::wstring prompt = L"\nВыберите высоту шляпки ["
        + to_wstring(screw->eFirst) + L"/"
        + to_wstring(screw->eSecond) + L"/"
        + to_wstring(screw->eThird) + L"/"
        + to_wstring(screw->eFourth) + L"/"
        + to_wstring(screw->eFifth) + L"/"
        + to_wstring(screw->eSixth) + L"/"
        + to_wstring(screw->eSeventh) + L"/"
        + to_wstring(screw->eEighth) + L"/"
        + to_wstring(screw->eNinth) + L"/"
        + to_wstring(screw->eTenth) + L"]: ";
    wchar_t userInput[10];

    int retFourth = acedGetKword(prompt.c_str(), userInput);

    if (retFourth == RTNORM)
    {
        int headDiameter = _wtoi(userInput);

        switch (headDiameter)
        {
        case screw->eFirst:
        case screw->eSecond:
        case screw->eThird:
        case screw->eFourth:
        case screw->eFifth:
        case screw->eSixth:
        case screw->eSeventh:
        case screw->eEighth:
        case screw->eNinth:
        case screw->eTenth:
            _headDiameter = static_cast<AcDbScrew::eHeadHeight>(headDiameter);
            break;
        default:
            acutPrintf(L"Некорректная высота шляпки");
            break;
        }
    }
    return AcEdJig::kNormal;
}