#include "mudbox_deformers_bend_deformer.h"
#include "mudbox_deformers_common_math.h"

#include <cassert>
#include <cmath>

#include <Mudbox/mudbox.h>

#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QGridLayout>

using std::fabs;

using mudbox::Geometry;
using mudbox::Interface;
using mudbox::Kernel;
using mudbox::MeshRenderer;
using mudbox::SubdivisionLevel;
using mudbox::Vector;
using mudbox::AxisAlignedBoundingBox;
using mudbox::Quaternion;


BendDeformer *BendDeformer::existingWidget = NULL;

const QString BendDeformer::objName = "bendDeformerUI";


BendDeformer::BendDeformer(QWidget *parent, Qt::WindowFlags flags) :
		QWidget(parent, flags),
		bendAngleMin(-360),
		bendAngleMax(360),
		bendRadiusMin(-100),
		bendRadiusMax(100),
		bendRadiusOffset(0.0f)
{
	Geometry *activeGeo = Kernel()->Scene()->ActiveGeometry();
	if (!activeGeo) {
		Kernel()->Interface()->MessageBox(Interface::msgError,
										  "No mesh selected!",
										  "You need to select a mesh first!");
		close();

		return;
	}

	updateOriginalPointPositions();

	setObjectName(objName);
	setWindowTitle("Apply Bend Deformer");

	QVBoxLayout *mainLayout = new QVBoxLayout;

	QVBoxLayout *settingsGrpLayout = new QVBoxLayout;
	QGroupBox *settingsGrp = new QGroupBox("Bend settings");

	QHBoxLayout *axesGrpLayout = new QHBoxLayout;
	QGroupBox *axesGrp = new QGroupBox("Bend axis");

	comboBoxBendAxis = new QComboBox(this);
	comboBoxBendAxis->addItem("X", BendDeformerAxis::BEND_DEFORMER_AXIS_X);
	comboBoxBendAxis->addItem("Y", BendDeformerAxis::BEND_DEFORMER_AXIS_Y);
	comboBoxBendAxis->addItem("Z", BendDeformerAxis::BEND_DEFORMER_AXIS_Z);

	axesGrpLayout->addWidget(comboBoxBendAxis);

	axesGrp->setLayout(axesGrpLayout);
	settingsGrpLayout->addWidget(axesGrp);

	QGridLayout *layoutSliders = new QGridLayout;

	QLabel *labelBendAngle = new QLabel("Bend angle");

	spinBoxBendRangeStartAngle = new QSpinBox(this);
	spinBoxBendRangeStartAngle->setRange(bendAngleMin, bendAngleMax);
	spinBoxBendRangeStartAngle->setValue(bendAngleMin);

	spinBoxBendRangeEndAngle = new QSpinBox(this);
	spinBoxBendRangeEndAngle->setRange(bendAngleMin, bendAngleMax);
	spinBoxBendRangeEndAngle->setValue(bendAngleMax);

	spinBoxBendAngle = new QSpinBox(this);
	spinBoxBendAngle->setRange(bendAngleMin, bendAngleMax);
	spinBoxBendAngle->setValue(0);

	sliderBendAngle = new QSlider(Qt::Horizontal);
	sliderBendAngle->setRange(bendAngleMin, bendAngleMax);
	sliderBendAngle->setValue(0);

	QLabel *labelBendDirection = new QLabel("Bend direction");

	spinBoxBendDirectionStartAngle = new QSpinBox(this);
	spinBoxBendDirectionStartAngle->setRange(bendAngleMin, bendAngleMax);
	spinBoxBendDirectionStartAngle->setValue(bendAngleMin);

	spinBoxBendDirectionEndAngle = new QSpinBox(this);
	spinBoxBendDirectionEndAngle->setRange(bendAngleMin, bendAngleMax);
	spinBoxBendDirectionEndAngle->setValue(bendAngleMax);

	spinBoxBendDirectionAngle = new QSpinBox(this);
	spinBoxBendDirectionAngle->setRange(bendAngleMin, bendAngleMax);
	spinBoxBendDirectionAngle->setValue(0);

	sliderBendDirection = new QSlider(Qt::Horizontal);
	sliderBendDirection->setRange(bendAngleMin, bendAngleMax);
	sliderBendDirection->setValue(0);

	QLabel *labelBendRadius = new QLabel("Bend radius");

	spinBoxBendRadiusMin = new QSpinBox(this);
	spinBoxBendRadiusMin->setRange(bendRadiusMin, bendRadiusMax);
	spinBoxBendRadiusMin->setValue(bendRadiusMin);

	spinBoxBendRadiusMax = new QSpinBox(this);
	spinBoxBendRadiusMax->setRange(bendRadiusMin, bendRadiusMax);
	spinBoxBendRadiusMax->setValue(bendRadiusMax);

	spinBoxBendRadius = new QSpinBox(this);
	spinBoxBendRadius->setRange(bendRadiusMin, bendRadiusMax);
	spinBoxBendRadius->setValue(0);

	sliderBendRadius = new QSlider(Qt::Horizontal);
	sliderBendRadius->setRange(bendRadiusMin, bendRadiusMax);
	sliderBendRadius->setValue(0);

	layoutSliders->addWidget(labelBendAngle, 0, 0);
	layoutSliders->addWidget(spinBoxBendRangeStartAngle, 0, 1);
	layoutSliders->addWidget(sliderBendAngle, 0, 2);
	layoutSliders->addWidget(spinBoxBendRangeEndAngle, 0, 3);
	layoutSliders->addWidget(spinBoxBendAngle, 0, 4);

	layoutSliders->addWidget(labelBendDirection, 1, 0);
	layoutSliders->addWidget(spinBoxBendDirectionStartAngle, 1, 1);
	layoutSliders->addWidget(sliderBendDirection, 1, 2);
	layoutSliders->addWidget(spinBoxBendDirectionEndAngle, 1, 3);
	layoutSliders->addWidget(spinBoxBendDirectionAngle, 1, 4);

	layoutSliders->addWidget(labelBendRadius, 2, 0);
	layoutSliders->addWidget(spinBoxBendRadiusMin, 2, 1);
	layoutSliders->addWidget(sliderBendRadius, 2, 2);
	layoutSliders->addWidget(spinBoxBendRadiusMax, 2, 3);
	layoutSliders->addWidget(spinBoxBendRadius, 2, 4);

	QPushButton *applyBtn = new QPushButton("Apply");
	QPushButton *resetBtn = new QPushButton("Reset");
	QPushButton *closeBtn = new QPushButton("Close");

	settingsGrpLayout->addLayout(layoutSliders);
	settingsGrp->setLayout(settingsGrpLayout);

	mainLayout->addWidget(settingsGrp);
	mainLayout->addWidget(applyBtn);
	mainLayout->addWidget(resetBtn);
	mainLayout->addWidget(closeBtn);

	setLayout(mainLayout);

	// NOTE: (sonictk) Signals for the various GUI buttons
	bool result = connect(closeBtn, SIGNAL(released()), this, SLOT(close()));
	assert(result == true);

	result = connect(applyBtn, SIGNAL(released()), this, SLOT(applyCB()));
	assert(result == true);

	result = connect(resetBtn, SIGNAL(released()), this, SLOT(resetCB()));
	assert(result == true);

	// NOTE: (sonictk) Signals for the bend angle group
	result = connect(sliderBendAngle, SIGNAL(valueChanged(int)), this, SLOT(bendCB(int)));
	assert(result == true);

	result = connect(spinBoxBendAngle, SIGNAL(valueChanged(int)), this, SLOT(setBendAngleCB(int)));
	assert(result == true);

	result = connect(spinBoxBendRangeStartAngle, SIGNAL(valueChanged(int)), this, SLOT(setBendRangeStartAngleCB(int)));
	assert(result == true);

	result = connect(spinBoxBendRangeEndAngle, SIGNAL(valueChanged(int)), this, SLOT(setBendRangeEndAngleCB(int)));
	assert(result == true);

	// NOTE: (sonictk) Signals for the bend direction group
	result = connect(sliderBendDirection, SIGNAL(valueChanged(int)), this, SLOT(bendDirectionCB(int)));
	assert(result == true);

	result = connect(spinBoxBendDirectionAngle, SIGNAL(valueChanged(int)), this, SLOT(setBendDirectionAngleCB(int)));
	assert(result == true);

	result = connect(spinBoxBendDirectionStartAngle, SIGNAL(valueChanged(int)), this, SLOT(setBendDirectionStartAngleCB(int)));
	assert(result == true);

	result = connect(spinBoxBendDirectionEndAngle, SIGNAL(valueChanged(int)), this, SLOT(setBendDirectionEndAngleCB(int)));
	assert(result == true);

	// NOTE: (sonictk) Signals for the bend radius group
	result = connect(sliderBendRadius, SIGNAL(valueChanged(int)), this, SLOT(bendRadiusCB(int)));
	assert(result == true);

	result = connect(spinBoxBendRadiusMin, SIGNAL(valueChanged(int)), this, SLOT(setBendRadiusMinCB(int)));
	assert(result == true);

	result = connect(spinBoxBendRadiusMax, SIGNAL(valueChanged(int)), this, SLOT(setBendRadiusMaxCB(int)));
	assert(result == true);

	result = connect(spinBoxBendRadius, SIGNAL(valueChanged(int)), this, SLOT(setBendRadiusCB(int)));
	assert(result == true);
}


BendDeformer::~BendDeformer()
{
	existingWidget = NULL;
}


void BendDeformer::closeEvent(QCloseEvent *event)
{
	resetSliders();

	resetGeometryPositions();

	updateSubdivisionLevel(activeSubdivLevel);

	QWidget::closeEvent(event);
}


bool BendDeformer::checkActiveGeometryAndUpdateCache()
{
	Geometry *activeGeo = Kernel()->Scene()->ActiveGeometry();
	if (!activeGeo) {
		activeSubdivLevel = NULL;

		return true;
	}

	SubdivisionLevel *currentSubdivLevel = activeGeo->ActiveLevel();

	if (currentSubdivLevel != activeSubdivLevel) {
		updateOriginalPointPositions();

		return true;
	}

	return false;
}


void BendDeformer::updateOriginalPointPositions()
{
	Geometry *activeGeo = Kernel()->Scene()->ActiveGeometry();
	if (!activeGeo) {
		return;
	}

	origPtPositions.clear();
	activeSubdivLevel = activeGeo->ActiveLevel();
	unsigned int numOfVertices = activeSubdivLevel->VertexCount();

	for (unsigned int i=0; i < numOfVertices; ++i) {
		Vector pos = activeSubdivLevel->VertexPosition(i);
		origPtPositions.push_back(pos);
	}
}


void BendDeformer::resetSliders()
{
	sliderBendAngle->setValue(0);
	spinBoxBendAngle->setValue(0);

	sliderBendDirection->setValue(0);
	spinBoxBendDirectionAngle->setValue(0);

	sliderBendRadius->setValue(0);
	spinBoxBendRadius->setValue(0);
}


void BendDeformer::resetSlidersWithoutAffectingGeometry()
{
	sliderBendAngle->blockSignals(true);
	spinBoxBendAngle->blockSignals(true);

	sliderBendDirection->blockSignals(true);
	spinBoxBendDirectionAngle->blockSignals(true);

	sliderBendRadius->blockSignals(true);
	spinBoxBendRadius->blockSignals(true);

	resetSliders();

	spinBoxBendAngle->blockSignals(false);
	sliderBendAngle->blockSignals(false);

	spinBoxBendDirectionAngle->blockSignals(false);
	sliderBendDirection->blockSignals(false);

	sliderBendRadius->blockSignals(false);
	spinBoxBendRadius->blockSignals(false);
}


BendDeformerStatus BendDeformer::resetGeometryPositions()
{
	Geometry *currentActiveGeo = Kernel()->Scene()->ActiveGeometry();
	SubdivisionLevel *currentSubdivLevel = currentActiveGeo->ActiveLevel();
	if (!currentActiveGeo || currentSubdivLevel != activeSubdivLevel) {
		return BendDeformerStatus::BEND_DEFORMER_STATUS_MISMATCHED_SUBDIV_LEVEL;
	}

	unsigned int numOfVertices = currentSubdivLevel->VertexCount();

	for (unsigned int i=0; i < numOfVertices; ++i) {
		activeSubdivLevel->SetVertexPosition(i, origPtPositions[i]);
	}

	return BendDeformerStatus::BEND_DEFORMER_STATUS_SUCCESS;
}


void BendDeformer::bendCB(int angle)
{
	spinBoxBendAngle->blockSignals(true);
	spinBoxBendAngle->setValue(angle);
	spinBoxBendAngle->blockSignals(false);

	if (angle == 0) {
		return;
	}

	if (!checkIfNoGeometrySelectedAndDisplayWarning()) {
		return;
	}

	checkActiveGeometryAndUpdateCache();

	if (!activeSubdivLevel) {
		return;
	}

	unsigned int numOfVertices = activeSubdivLevel->VertexCount();

	BendDeformerAxis bendAxis = static_cast<BendDeformerAxis>
		(comboBoxBendAxis->itemData(comboBoxBendAxis->currentIndex()).toInt());

	// NOTE: (sonictk) For simplicity's sake, all UI will be in degrees, but all
	// calculations will be done in radians.
	float bendAngle = (float)sliderBendAngle->value() * DEG_TO_RAD;
	float helperBendAngle = fabs(bendAngle);

	float bendDirectionAngle = (float)sliderBendDirection->value() * DEG_TO_RAD;

	Vector helperRotateAxis;
	AxisAlignedBoundingBox bBox = activeSubdivLevel->BoundingBox();
	float bendRadius;
	switch (bendAxis) {
	case BendDeformerAxis::BEND_DEFORMER_AXIS_X:
		helperRotateAxis = Vector(1, 0, 0);
		bendRadius = bBox.XSize();
		break;

	case BendDeformerAxis::BEND_DEFORMER_AXIS_Y:
		helperRotateAxis = Vector(0, 1, 0);
		bendRadius = bBox.YSize();
		break;

	case BendDeformerAxis::BEND_DEFORMER_AXIS_Z:
	default:
		helperRotateAxis = Vector(0, 0, 1);
		bendRadius = bBox.ZSize();
		break;
	}

	bendRadius += bendRadiusOffset;
	bendRadius *= PI / helperBendAngle;

	float helperRadius = bendAngle < 0.0f ? -bendRadius : bendRadius;

	for (unsigned int i=0; i < numOfVertices; ++i) {
		Vector origPos = origPtPositions[i];
		Vector finalPos = Vector(origPos);

		// NOTE: (sonictk) Take bend direction into account
		finalPos = rotateBy(finalPos, helperRotateAxis, bendDirectionAngle);

		float meshPtAxisLength;
		switch (bendAxis) {
		case BendDeformerAxis::BEND_DEFORMER_AXIS_X:
			meshPtAxisLength = finalPos.x;
			break;

		case BendDeformerAxis::BEND_DEFORMER_AXIS_Y:
			meshPtAxisLength = finalPos.y;
			break;

		case BendDeformerAxis::BEND_DEFORMER_AXIS_Z:
		default:
			meshPtAxisLength = finalPos.z;
			break;
		}

		float helperArcAngle = clamp(meshPtAxisLength / helperRadius, -helperBendAngle, helperBendAngle);

		float helperRemainingUnbentLength = meshPtAxisLength - (helperArcAngle * helperRadius);

		float helperCosArcAngle = cosf(helperArcAngle);
		float helperSinArcAngle = sinf(helperArcAngle);

		// NOTE: (sonictk) If the bend radius is 0, rotate the whole mesh
		if (areFloatsEqual(helperRadius, 0.0f)) {
			Vector helperAxis;
			switch (bendAxis) {
			case BendDeformerAxis::BEND_DEFORMER_AXIS_X:
				helperAxis = Vector(0, 1, 0);
				break;

			case BendDeformerAxis::BEND_DEFORMER_AXIS_Y:
				helperAxis = Vector(0, 0, 1);
				break;

			case BendDeformerAxis::BEND_DEFORMER_AXIS_Z:
			default:
				helperAxis = Vector(1, 0, 0);
				break;
			}

			finalPos = rotateBy(finalPos, helperAxis, -helperBendAngle);

		} else {
			Vector helperOriginVector;
			float tmpValue;
			switch (bendAxis) {
			case BendDeformerAxis::BEND_DEFORMER_AXIS_X:
				helperOriginVector = Vector(0, 0, -helperRadius);
				helperOriginVector = rotateBy(helperOriginVector, Vector(0, 1, 0), -helperArcAngle);

				tmpValue = helperOriginVector.z + (finalPos.z * helperCosArcAngle) + (helperRemainingUnbentLength * helperSinArcAngle) + helperRadius;

				finalPos.x = (helperOriginVector.x - (finalPos.z * helperSinArcAngle)) + (helperRemainingUnbentLength * helperCosArcAngle);

				finalPos.z = tmpValue;

				break;

			case BendDeformerAxis::BEND_DEFORMER_AXIS_Y:
				helperOriginVector = Vector(-helperRadius, 0, 0);
				helperOriginVector = rotateBy(helperOriginVector, Vector(0, 0, 1), -helperArcAngle);

				tmpValue = helperOriginVector.x + (finalPos.x * helperCosArcAngle) + (helperRemainingUnbentLength * helperSinArcAngle) + helperRadius;

				finalPos.y = (helperOriginVector.y - (finalPos.x * helperSinArcAngle)) + (helperRemainingUnbentLength * helperCosArcAngle);

				finalPos.x = tmpValue;

				break;

			case BendDeformerAxis::BEND_DEFORMER_AXIS_Z:
			default:
				helperOriginVector = Vector(0, -helperRadius, 0);
				helperOriginVector = rotateBy(helperOriginVector, Vector(1, 0, 0), -helperArcAngle);

				tmpValue = helperOriginVector.y + (finalPos.y * helperCosArcAngle) + (helperRemainingUnbentLength * helperSinArcAngle) + helperRadius;

				finalPos.z = (helperOriginVector.z - (finalPos.y * helperSinArcAngle)) + (helperRemainingUnbentLength * helperCosArcAngle);

				finalPos.y = tmpValue;

				break;
			}
		}

		// NOTE: (sonictk) Take bend direction into account
		finalPos = rotateBy(finalPos, helperRotateAxis, -bendDirectionAngle);

		activeSubdivLevel->SetVertexPosition(i, finalPos);
	}

	quickUpdateSubdivisionLevel(activeSubdivLevel);
}


void BendDeformer::setBendAngleCB(int angle)
{
	sliderBendAngle->setValue(angle);
}


void BendDeformer::bendDirectionCB(int angle)
{
	spinBoxBendDirectionAngle->blockSignals(true);
	spinBoxBendDirectionAngle->setValue(angle);
	spinBoxBendDirectionAngle->blockSignals(false);

	int bendAngle = sliderBendAngle->value();
	bendCB(bendAngle);
}


void BendDeformer::setBendDirectionAngleCB(int angle)
{
	sliderBendDirection->setValue(angle);
}


void BendDeformer::setBendDirectionStartAngleCB(int angle)
{
	int endAngle = spinBoxBendDirectionEndAngle->value();
	if (angle >= endAngle) {
		angle = endAngle - 1;
		spinBoxBendDirectionStartAngle->setValue(angle);
	}

	sliderBendDirection->setMinimum(angle);
	spinBoxBendDirectionAngle->setMinimum(angle);
}


void BendDeformer::setBendDirectionEndAngleCB(int angle)
{
	int startAngle = spinBoxBendDirectionStartAngle->value();
	if (angle <= startAngle) {
		angle = startAngle + 1;
		spinBoxBendDirectionEndAngle->setValue(angle);
	}

	sliderBendDirection->setMaximum(angle);
	spinBoxBendDirectionAngle->setMaximum(angle);
}


void BendDeformer::setBendRangeStartAngleCB(int angle)
{
	int endAngle = spinBoxBendRangeEndAngle->value();
	if (angle >= endAngle) {
		angle = endAngle - 1;
		spinBoxBendRangeStartAngle->setValue(angle);
	}

	sliderBendAngle->setMinimum(angle);
	spinBoxBendAngle->setMinimum(angle);
}


void BendDeformer::setBendRangeEndAngleCB(int angle)
{
	int startAngle = spinBoxBendRangeStartAngle->value();
	if (angle <= startAngle) {
		angle = startAngle + 1;
		spinBoxBendRangeEndAngle->setValue(angle);
	}

	sliderBendAngle->setMaximum(angle);
	spinBoxBendAngle->setMaximum(angle);
}


void BendDeformer::bendRadiusCB(int radius)
{
	bendRadiusOffset = (float)radius;

	spinBoxBendRadius->blockSignals(true);
	spinBoxBendRadius->setValue(radius);
	spinBoxBendRadius->blockSignals(false);

	int bendAngle = sliderBendAngle->value();
	bendCB(bendAngle);
}


void BendDeformer::setBendRadiusMinCB(int radius)
{
	int maxRadius = spinBoxBendRadiusMax->value();
	if (radius >= maxRadius) {
		radius = maxRadius - 1;
		spinBoxBendRadiusMin->setValue(radius);
	}

	sliderBendRadius->setMinimum(radius);
	spinBoxBendRadius->setMinimum(radius);
}


void BendDeformer::setBendRadiusMaxCB(int radius)
{
	int minRadius = spinBoxBendRadiusMin->value();
	if (radius <= minRadius) {
		radius = minRadius + 1;
		spinBoxBendRadius->setValue(radius);
	}

	sliderBendRadius->setMaximum(radius);
	spinBoxBendRadius->setMaximum(radius);
}


void BendDeformer::setBendRadiusCB(int radius)
{
	sliderBendRadius->setValue(radius);
}


void BendDeformer::applyCB()
{
	resetSlidersWithoutAffectingGeometry();

	if (!checkIfNoGeometrySelectedAndDisplayWarning()) {
		return;
	}

	updateSubdivisionLevel(activeSubdivLevel);

	updateOriginalPointPositions();
}


void BendDeformer::resetCB()
{
	resetSliders();

	resetGeometryPositions();

	updateSubdivisionLevel(activeSubdivLevel);
}


void showBendDeformerUI()
{
	unsigned int numOfGeos = Kernel()->Scene()->GeometryCount();
	if (numOfGeos == 0) {
		Kernel()->Interface()->MessageBox(Interface::msgError,
										  "No meshes available to deform!",
										  "There are no meshes in the scene to deform!");

		return;
	}

	QWidget *mudboxMainWindow = Kernel()->Interface()->MainWindow();

	// NOTE: (sonictk) Ensure that only one instance of the window exists
	if (!BendDeformer::existingWidget) {
		BendDeformer *ui = new BendDeformer(mudboxMainWindow, Qt::Window);
		BendDeformer::existingWidget = ui;
	}

	BendDeformer::existingWidget->show();
	BendDeformer::existingWidget->raise();
}
