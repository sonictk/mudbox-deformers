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
		QWidget(parent, flags), bendAngleMin(-360), bendAngleMax(360)
{
	activeGeo = Kernel()->Scene()->ActiveGeometry();
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

	QHBoxLayout *bendAngleLayout = new QHBoxLayout;

	QLabel *sliderLabel = new QLabel("Bend angle");

	spinBoxBendRangeStartAngle = new QSpinBox(this);
	spinBoxBendRangeStartAngle->setValue(bendAngleMin);

	spinBoxBendRangeEndAngle = new QSpinBox(this);
	spinBoxBendRangeEndAngle->setValue(bendAngleMax);

	sliderBendAngle = new QSlider(Qt::Horizontal);
	sliderBendAngle->setRange(bendAngleMin, bendAngleMax);
	sliderBendAngle->setValue(0);

	bendAngleLayout->addWidget(sliderLabel);
	bendAngleLayout->addWidget(spinBoxBendRangeStartAngle);
	bendAngleLayout->addWidget(sliderBendAngle);
	bendAngleLayout->addWidget(spinBoxBendRangeEndAngle);

	QPushButton *applyBtn = new QPushButton("Apply");

	QPushButton *resetBtn = new QPushButton("Reset");

	QPushButton *closeBtn = new QPushButton("Close");

	settingsGrpLayout->addLayout(bendAngleLayout);
	settingsGrp->setLayout(settingsGrpLayout);

	mainLayout->addWidget(settingsGrp);
	mainLayout->addWidget(applyBtn);
	mainLayout->addWidget(resetBtn);
	mainLayout->addWidget(closeBtn);

	setLayout(mainLayout);

	bool result = connect(closeBtn, SIGNAL(released()), this, SLOT(close()));
	assert(result == true);

	result = connect(applyBtn, SIGNAL(released()), this, SLOT(applyCB()));
	assert(result == true);

	result = connect(resetBtn, SIGNAL(released()), this, SLOT(resetCB()));
	assert(result == true);

	result = connect(sliderBendAngle, SIGNAL(valueChanged(int)), this, SLOT(bendCB(int)));
	assert(result == true);

	// TODO: (sonictk) Connect other GUI widgets
}


BendDeformer::~BendDeformer()
{
	existingWidget = NULL;
}


void BendDeformer::bendCB(int angle)
{
	if (angle == 0) {
		return;
	}

	Geometry *currentActiveGeo = Kernel()->Scene()->ActiveGeometry();
	if (!currentActiveGeo) {
		Kernel()->Interface()->MessageBox(Interface::msgError,
										  "No mesh selected!",
										  "You need to select a mesh first!");
		return;
	}
	else if (currentActiveGeo != activeGeo) {
		activeGeo = currentActiveGeo;
		updateOriginalPointPositions();
	}

	SubdivisionLevel *activeSubdivLevel = activeGeo->ActiveLevel();
	unsigned int numOfVertices = activeSubdivLevel->VertexCount();

	BendDeformerAxis bendAxis = static_cast<BendDeformerAxis>
		(comboBoxBendAxis->itemData(comboBoxBendAxis->currentIndex()).toInt());

	// NOTE: (sonictk) For simplicity's sake, all UI will be in degrees, but all
	// calculations will be done in radians.
	float bendAngle = (float)sliderBendAngle->value() * DEG_TO_RAD * 0.01f;

	Vector helperRotateAxis;
	switch (bendAxis) {
	case BendDeformerAxis::BEND_DEFORMER_AXIS_X:
		helperRotateAxis = Vector(1, 0, 0);
		break;

	case BendDeformerAxis::BEND_DEFORMER_AXIS_Y:
		helperRotateAxis = Vector(0, -1, 0);
		break;

	case BendDeformerAxis::BEND_DEFORMER_AXIS_Z:
	default:
		helperRotateAxis = Vector(0, 0, 1);
		break;
	}

	for (unsigned int i=0; i < numOfVertices; ++i) {
		Vector origPos = origPtPositions[i];

		float helperAngle;
		switch (bendAxis) {
		case BendDeformerAxis::BEND_DEFORMER_AXIS_Y:
			helperAngle = origPos.x * bendAngle;
			break;

		case BendDeformerAxis::BEND_DEFORMER_AXIS_X:
		case BendDeformerAxis::BEND_DEFORMER_AXIS_Z:
		default:
			helperAngle = origPos.y * bendAngle;
			break;
		}

		Vector finalPos = rotateBy(origPos, helperRotateAxis, helperAngle);
		activeSubdivLevel->SetVertexPosition(i, finalPos);
	}

	updateSubdivisionLevel(activeSubdivLevel);
}


void BendDeformer::applyCB()
{
	sliderBendAngle->blockSignals(true);
	sliderBendAngle->setValue(0);
	sliderBendAngle->blockSignals(false);

	Geometry *currentActiveGeo = Kernel()->Scene()->ActiveGeometry();
	if (!currentActiveGeo) {
		Kernel()->Interface()->MessageBox(Interface::msgError,
										  "No mesh selected!",
										  "You need to select a mesh first!");
		return;
	}
	else if (currentActiveGeo != activeGeo) {
		activeGeo = currentActiveGeo;
		updateOriginalPointPositions();
	}

	SubdivisionLevel *currentSubdivisionLevel = activeGeo->ActiveLevel();
	updateSubdivisionLevel(currentSubdivisionLevel);
}


void BendDeformer::resetCB()
{
	sliderBendAngle->blockSignals(true);

	sliderBendAngle->setValue(0);

	sliderBendAngle->blockSignals(false);

	Geometry *currentActiveGeo = Kernel()->Scene()->ActiveGeometry();
	if (!currentActiveGeo) {
		Kernel()->Interface()->MessageBox(Interface::msgError,
										  "No mesh selected!",
										  "You need to select a mesh first!");
		return;
	}
	else if (currentActiveGeo != activeGeo) {
		activeGeo = currentActiveGeo;
		updateOriginalPointPositions();
	}

	SubdivisionLevel *currentSubdivisionLevel = activeGeo->ActiveLevel();
	unsigned int numOfVertices = currentSubdivisionLevel->VertexCount();
	for (unsigned int i=0; i < numOfVertices; ++i) {
		currentSubdivisionLevel->SetVertexPosition(i, origPtPositions[i]);
	}

	updateSubdivisionLevel(currentSubdivisionLevel);
}


void BendDeformer::updateOriginalPointPositions()
{
	activeGeo = Kernel()->Scene()->ActiveGeometry();
	if (!activeGeo) {
		return;
	}

	origPtPositions.clear();
	SubdivisionLevel *currentSubdivisionLevel = activeGeo->ActiveLevel();
	unsigned int numOfVertices = currentSubdivisionLevel->VertexCount();

	for (unsigned int i=0; i < numOfVertices; ++i) {
		Vector pos = currentSubdivisionLevel->VertexPosition(i);
		origPtPositions.push_back(pos);
	}
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
