#pragma once

#ifdef CROSSCIRCLE_MODULE
#define DLLIMPEXP __declspec(dllexport)
#else
#define DLLIMPEXP
#endif

//-----------------------------------------------------------------------------
class DLLIMPEXP AcDbScrew : public AcDbEntity
{
public:
	ACRX_DECLARE_MEMBERS(AcDbScrew);

	AcDbScrew();
	virtual ~AcDbScrew();

	//Геттеры
	AcGePoint3d center();
	AcGeVector3d direction();
	AcGeVector3d normal();
	double bodyLength();
	double headDiameter();
	double bodyDiameter();
	double transition();

	//Сеттеры
	Acad::ErrorStatus setCenter(AcGePoint3d);
	Acad::ErrorStatus setDirection(AcGeVector3d);
	Acad::ErrorStatus setNormal(AcGeVector3d);
	Acad::ErrorStatus setBodyLength(double);
	Acad::ErrorStatus setBodyDiameter(double);
	Acad::ErrorStatus setTransition(double);

	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* pFiler) const override;
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* pFiler) override;

	//Грипы
	static enum eGripIndex
	{
		eCenter		= 0,
		eRotate		= 1,
		eLength		= 2,
		eScale		= 3
	};
	static enum eDiameter
	{
		eFirst		= 10,
		eSecond		= 12,
		eThird		= 14,
		eFourth		= 16,
		eFifth		= 18,
		eSixth		= 20,
		eSeventh	= 22,
		eEighth		= 24,
		eNinth		= 27,
		eTenth		= 30,
	};
	const std::vector<int> diameterValues = {
		AcDbScrew::eDiameter::eFirst,
		AcDbScrew::eDiameter::eSecond,
		AcDbScrew::eDiameter::eThird,
		AcDbScrew::eDiameter::eFourth,
		AcDbScrew::eDiameter::eFifth,
		AcDbScrew::eDiameter::eSixth,
		AcDbScrew::eDiameter::eSeventh,
		AcDbScrew::eDiameter::eEighth,
		AcDbScrew::eDiameter::eNinth,
		AcDbScrew::eDiameter::eTenth 
	};
	bool _isLengthChanged;

protected:
	static Adesk::UInt32 kCurrentVersionNumber;
	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* pWD) override;
	virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform) override;
	// Работа грипов
	virtual Acad::ErrorStatus subGetGripPoints(
		AcDbGripDataPtrArray& grips,
		const double curViewUnitSize,
		const int gripSize,
		const AcGeVector3d& curViewDir,
		const int bitflags) const;
	virtual Acad::ErrorStatus subGetGripPoints(
		AcGePoint3dArray& gripPoints,
		AcDbIntArray& osnapModes,
		AcDbIntArray& geomIds) const override;
	virtual Acad::ErrorStatus subMoveGripPointsAt(
		const AcDbVoidPtrArray& appData,
		const AcGeVector3d& offset,
		const int bitflags);
	virtual Acad::ErrorStatus subMoveGripPointsAt(
		const AcDbIntArray& indices,
		const AcGeVector3d& offset) override;
	virtual void subGripStatus(const AcDb::GripStat status) override;

private:
	//Данные о грипах
	struct GripData
	{
		int index;
	};
	mutable std::vector<GripData*> _gripsData ={
		nullptr,
		nullptr,
		nullptr,
		nullptr,
	};

	// Параметры винта
	const double HEAD_DIAMETER_TO_BODY_DIAMETER_KOEF = 1.8;
	AcGePoint3d _center;
	AcGeVector3d _direction;
	AcGeVector3d _normal;
	double _bodyLength;
	double _headDiameter;
	double _bodyDiameter;
	double _transition;

	//Кэширование геометрии
	static enum eEntityIndex
	{
		eHead = 0,
		eBody = 1,
		eThread = 2,
	};
	mutable std::vector<AcDb3dSolid*> _cachedGeometry = {
		nullptr,
		nullptr,
		nullptr
	};
	bool _isGeometryActual;
	void reBuild();
};