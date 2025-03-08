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

	//�������
	AcGePoint3d center();
	AcGeVector3d direction();
	AcGeVector3d normal();
	double bodyLength();
	double headHeight();
	double headDiameter();
	double bodyDiameter();
	double transition();

	//�������
	Acad::ErrorStatus setCenter(AcGePoint3d);
	Acad::ErrorStatus setDirection(AcGeVector3d);
	Acad::ErrorStatus setNormal(AcGeVector3d);
	Acad::ErrorStatus setBodyLength(double);
	Acad::ErrorStatus setHeadHeight(double);
	Acad::ErrorStatus setHeadDiameter(double);
	Acad::ErrorStatus setBodyDiameter(double);
	Acad::ErrorStatus setTransition(double);

	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* pFiler) const override;
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* pFiler) override;

protected:
	static Adesk::UInt32 kCurrentVersionNumber;
	// AcDbEntity
	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* pWD) override;
	virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform) override;
	// �����
	virtual Acad::ErrorStatus subGetGripPoints(AcGePoint3dArray& gripPoints,
		AcDbIntArray& osnapModes, AcDbIntArray& geomIds) const override;
	virtual Acad::ErrorStatus subGetGripPoints(AcDbGripDataPtrArray& grips,
		const double curViewUnitSize,
		const int gripSize,
		const AcGeVector3d& curViewDir,
		const int bitflags) const;
	virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbIntArray& indices,
		const AcGeVector3d& offset) override;
	virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbVoidPtrArray& appData,
		const AcGeVector3d& offset,
		const int bitflags);

private:
	// ��������� �����
	AcGePoint3d _center;
	AcGeVector3d _direction;
	AcGeVector3d _normal;
	double _bodyLength;
	double _headHeight;
	double _headDiameter;
	double _bodyDiameter;
	double _transition;
};