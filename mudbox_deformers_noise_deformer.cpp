#include "mudbox_deformers_util.h"
#include "mudbox_deformers_noise.h"
#include "mudbox_deformers_noise_deformer.h"

#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QGridLayout>

#include <Mudbox/mudbox.h>

using mudbox::Geometry;
using mudbox::SubdivisionLevel;


NoiseDeformer *NoiseDeformer::existingWidget = NULL;

const QString NoiseDeformer::objName = "noiseDeformerUI";


NoiseDeformer::NoiseDeformer(QWidget *parent, Qt::WindowFlags flags) :
		QWidget(parent, flags),
		weightMin(0),
		weightMax(100),
		defaultWeight(0),
		octavesMin(1),
		octavesMax(10),
		defaultOctaves(1)
{
	Geometry *activeGeo = Kernel()->Scene()->ActiveGeometry();
	if (!activeGeo) {
		Kernel()->Interface()->MessageBox(Interface::msgError,
										  "No mesh selected!",
										  "You need to select a mesh first!");
		close();

		return;
	}

	setObjectName(objName);
	setWindowTitle("Apply Noise Deformer");

	QVBoxLayout *mainLayout = new QVBoxLayout;

	QPushButton *btnApply = new QPushButton("Apply");
	QPushButton *btnReset = new QPushButton("Reset");
	QPushButton *btnClose = new QPushButton("Close");

	QGroupBox *grpNoiseSettings = new QGroupBox("Noise settings");

	// NOTE: (sonictk) Perlin weight widgets
	QLabel *labelPerlinWeight = new QLabel("Weight");

	spinBoxWeightMin = new QSpinBox(this);
	spinBoxWeightMin->setRange(weightMin, weightMax);
	spinBoxWeightMin->setValue(weightMin);

	spinBoxWeightMax = new QSpinBox(this);
	spinBoxWeightMax->setRange(weightMin, weightMax);
	spinBoxWeightMax->setValue(weightMax);

	spinBoxWeight = new QSpinBox(this);
	spinBoxWeight->setRange(weightMin, weightMax);
	spinBoxWeight->setValue(defaultWeight);

	sliderWeight = new QSlider(this);
	sliderWeight->setOrientation(Qt::Horizontal);
	sliderWeight->setRange(weightMin, weightMax);
	sliderWeight->setValue(defaultWeight);

	// NOTE: (sonictk) Perlin octave widgets
	QLabel *labelPerlinOctaves = new QLabel("Octaves");

	spinBoxOctavesMin = new QSpinBox(this);
	spinBoxOctavesMin->setRange(octavesMin, octavesMax);
	spinBoxOctavesMin->setValue(octavesMin);

	spinBoxOctavesMax = new QSpinBox(this);
	spinBoxOctavesMax->setRange(octavesMin, octavesMax);
	spinBoxOctavesMax->setValue(octavesMax);

	spinBoxOctaves = new QSpinBox(this);
	spinBoxOctaves->setRange(octavesMin, octavesMax);
	spinBoxOctaves->setValue(defaultOctaves);

	sliderOctaves = new QSlider(this);
	sliderOctaves->setOrientation(Qt::Horizontal);
	sliderOctaves->setRange(octavesMin, octavesMax);
	sliderOctaves->setValue(defaultOctaves);

	QGridLayout *perlinSettingsLayout = new QGridLayout;
	perlinSettingsLayout->addWidget(labelPerlinWeight, 0, 0);
	perlinSettingsLayout->addWidget(spinBoxWeightMin, 0, 1);
	perlinSettingsLayout->addWidget(sliderWeight, 0, 2);
	perlinSettingsLayout->addWidget(spinBoxWeightMax, 0, 3);
	perlinSettingsLayout->addWidget(spinBoxWeight, 0, 4);

	perlinSettingsLayout->addWidget(labelPerlinOctaves, 1, 0);
	perlinSettingsLayout->addWidget(spinBoxOctavesMin, 1, 1);
	perlinSettingsLayout->addWidget(sliderOctaves, 1, 2);
	perlinSettingsLayout->addWidget(spinBoxOctavesMax, 1, 3);
	perlinSettingsLayout->addWidget(spinBoxOctaves, 1, 4);

	grpNoiseSettings->setLayout(perlinSettingsLayout);

	mainLayout->addWidget(grpNoiseSettings);

	mainLayout->addWidget(btnApply);
	mainLayout->addWidget(btnReset);
	mainLayout->addWidget(btnClose);

	setLayout(mainLayout);

	// NOTE: (sonictk) Signals for the various GUI buttons
	bool result = connect(btnClose, SIGNAL(released()), this, SLOT(close()));
	assert(result == true);

	result = connect(btnApply, SIGNAL(released()), this, SLOT(applyCB()));
	assert(result == true);

	result = connect(btnReset, SIGNAL(released()), this, SLOT(resetCB()));
	assert(result == true);

	// NOTE: (sonictk) Signals for the weight group
	result = connect(sliderWeight, SIGNAL(valueChanged(int)), this, SLOT(weightChangedCB(int)));
	assert(result == true);

	result = connect(spinBoxWeight, SIGNAL(valueChanged(int)), this, SLOT(setWeightCB(int)));
	assert(result == true);

	result = connect(spinBoxWeightMin, SIGNAL(valueChanged(int)), this, SLOT(setMinWeightCB(int)));
	assert(result == true);

	result = connect(spinBoxWeightMax, SIGNAL(valueChanged(int)), this, SLOT(setMaxWeightCB(int)));
	assert(result == true);

	// NOTE: (sonictk) Signals for the octaves group
	result = connect(sliderOctaves, SIGNAL(valueChanged(int)), this, SLOT(octavesChangedCB(int)));
	assert(result == true);

	result = connect(spinBoxOctaves, SIGNAL(valueChanged(int)), this, SLOT(setOctavesCB(int)));
	assert(result == true);

	result = connect(spinBoxOctavesMin, SIGNAL(valueChanged(int)), this, SLOT(setMinOctavesCB(int)));
	assert(result == true);

	result = connect(spinBoxOctavesMax, SIGNAL(valueChanged(int)), this, SLOT(setMaxOctavesCB(int)));
	assert(result == true);

	updateOriginalPointPositions();
}


NoiseDeformer::~NoiseDeformer()
{
	existingWidget = NULL;
}


void NoiseDeformer::resetSliders()
{
	sliderWeight->setValue(0);
	spinBoxWeight->setValue(0);

	sliderOctaves->setValue(0);
	spinBoxOctaves->setValue(0);
}


NoiseDeformerStatus NoiseDeformer::resetGeometryPositions()
{
	Geometry *currentActiveGeo = Kernel()->Scene()->ActiveGeometry();
	SubdivisionLevel *currentSubdivLevel = currentActiveGeo->ActiveLevel();

	if (!currentActiveGeo || currentSubdivLevel != activeSubdivLevel) {
		return NoiseDeformerStatus::NOISE_DEFORMER_STATUS_MISMATCHED_SUBDIV_LEVEL;
	}

	unsigned int numOfVertices = currentSubdivLevel->VertexCount();

	for (unsigned int i=0; i < numOfVertices; ++i) {
		activeSubdivLevel->SetVertexPosition(i, origPtPositions[i]);
	}

	return NoiseDeformerStatus::NOISE_DEFORMER_STATUS_SUCCESS;
}


void NoiseDeformer::updateOriginalPointPositions()
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


void NoiseDeformer::resetSlidersWithoutAffectingGeometry()
{
	sliderWeight->blockSignals(true);
	spinBoxWeight->blockSignals(true);

	sliderOctaves->blockSignals(true);
	spinBoxOctaves->blockSignals(true);

	resetSliders();

	sliderWeight->blockSignals(false);
	spinBoxWeight->blockSignals(false);

	sliderOctaves->blockSignals(false);
	spinBoxOctaves->blockSignals(false);
}


bool NoiseDeformer::checkActiveGeometryAndUpdateCache()
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


void NoiseDeformer::closeEvent(QCloseEvent *event)
{
	resetSliders();

	resetGeometryPositions();

	updateSubdivisionLevel(activeSubdivLevel);

	QWidget::closeEvent(event);
}


NoiseDeformerStatus NoiseDeformer::deform(float weight, int octaves)
{
	if (!checkIfNoGeometrySelectedAndDisplayWarning()) {
		return NoiseDeformerStatus::NOISE_DEFORMER_STATUS_NO_GEOMETRY_SELECTED;
	}

	checkActiveGeometryAndUpdateCache();

	if (!activeSubdivLevel) {
		return NoiseDeformerStatus::NOISE_DEFORMER_STATUS_NO_ACTIVE_SUBDIV_LEVEL;
	}

	unsigned int numOfVertices = activeSubdivLevel->VertexCount();
	for (unsigned int i=0; i < numOfVertices; ++i) {
		Vector origPos = origPtPositions[i];
		Vector finalPos = Vector(origPos);

		float perlinFactor = perlin(finalPos.x, finalPos.y, finalPos.z, -1);

		finalPos *= 1.0f + (perlinFactor * weight);

		activeSubdivLevel->SetVertexPosition(i, finalPos);
	}

	quickUpdateSubdivisionLevel(activeSubdivLevel);

	return NoiseDeformerStatus::NOISE_DEFORMER_STATUS_SUCCESS;
}


void NoiseDeformer::applyCB()
{
	resetSlidersWithoutAffectingGeometry();

	if (!checkIfNoGeometrySelectedAndDisplayWarning()) {
		return;
	}

	updateSubdivisionLevel(activeSubdivLevel);

	updateOriginalPointPositions();
}


void NoiseDeformer::resetCB()
{
	resetSliders();

	resetGeometryPositions();

	updateSubdivisionLevel(activeSubdivLevel);
}


void NoiseDeformer::weightChangedCB(int weight)
{
	spinBoxWeight->blockSignals(true);
	spinBoxWeight->setValue(weight);
	spinBoxWeight->blockSignals(false);

	deform(float(sliderWeight->value()) / 100.0f, sliderOctaves->value());
}


void NoiseDeformer::setWeightCB(int weight)
{
	sliderWeight->setValue(weight);
}


void NoiseDeformer::setMinWeightCB(int weight)
{
	int weightMax = spinBoxWeightMax->value();
	if (weight >= weightMax) {
		weight = weightMax - 1;
		spinBoxWeightMin->setValue(weight);
	}

	sliderWeight->setMinimum(weight);
	spinBoxWeight->setMinimum(weight);
}


void NoiseDeformer::setMaxWeightCB(int weight)
{
	int weightMin = spinBoxWeightMin->value();
	if (weight <= weightMin) {
		weight = weightMin + 1;
		spinBoxWeightMax->setValue(weight);
	}

	sliderWeight->setMaximum(weight);
	spinBoxWeight->setMaximum(weight);
}


void NoiseDeformer::octavesChangedCB(int octaves)
{
	spinBoxOctaves->blockSignals(true);
	spinBoxOctaves->setValue(octaves);
	spinBoxOctaves->blockSignals(false);

	deform(float(sliderWeight->value()) / 100.0f, sliderOctaves->value());
}


void NoiseDeformer::setOctavesCB(int octaves)
{
	sliderOctaves->setValue(octaves);
}


void NoiseDeformer::setMinOctavesCB(int octaves)
{
	int octavesMax = spinBoxOctavesMax->value();
	if (octaves >= octavesMax) {
		octaves = octavesMax - 1;
		spinBoxOctavesMin->setValue(octaves);
	}

	sliderOctaves->setMinimum(octaves);
	spinBoxOctaves->setMinimum(octaves);
}


void NoiseDeformer::setMaxOctavesCB(int octaves)
{
	int octavesMin = spinBoxOctavesMin->value();
	if (octaves <= octavesMin) {
		octaves = octavesMin + 1;
		spinBoxOctavesMax->setValue(octaves);
	}

	sliderOctaves->setMaximum(octaves);
	spinBoxOctaves->setMaximum(octaves);
}


void showNoiseDeformerUI()
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
	if (!NoiseDeformer::existingWidget) {
		NoiseDeformer *ui = new NoiseDeformer(mudboxMainWindow, Qt::Window);
		NoiseDeformer::existingWidget = ui;
	}

	NoiseDeformer::existingWidget->show();
	NoiseDeformer::existingWidget->raise();
}
