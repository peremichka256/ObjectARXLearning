#include "AcDbScrew.h"
#pragma once

class ScrewJig : public AcEdJig
{
public:
	AcGePoint3dArray mInputPoints;
	AcDbScrew* screw;
	ScrewJig();
	~ScrewJig();
	AcEdJig::DragStatus startJig(AcDbScrew*);
private:
	int mCurrentInputLevel;
	double _length;
	double _headDiameter;

	AcEdJig::DragStatus GetStartPoint();
	AcEdJig::DragStatus GetDirectionPoint();
	AcEdJig::DragStatus GetLength();
	AcEdJig::DragStatus GetHeadDiameter();
protected:
	virtual Adesk::Boolean update();
	virtual DragStatus sampler();
	virtual AcDbEntity* entity() const;
};

