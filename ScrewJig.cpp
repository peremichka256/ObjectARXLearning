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
    ACHAR* inputPrompts[2] =
    {
      _T("\nЦентр винта: "),
      _T("\nТочка определяющая направление: ")
    };

    bool appendOk = true;
    AcEdJig::DragStatus status = AcEdJig::kNull;

    for (mCurrentInputLevel = 0; mCurrentInputLevel < 2; mCurrentInputLevel++)
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
    AcEdJig::UserInputControls userInputControls[2] =
    {
        (UserInputControls)(AcEdJig::kAccept3dCoordinates
            | AcEdJig::kNoNegativeResponseAccepted),
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

            screw->setDirection(acqVector);
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