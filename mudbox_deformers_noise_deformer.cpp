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
using mudbox::Interface;
using mudbox::Kernel;
using mudbox::LayerMeshData;
using mudbox::SubdivisionLevel;
using mudbox::Vertex;


NoiseDeformer *NoiseDeformer::existingWidget = NULL;

const QString NoiseDeformer::objName = "noiseDeformerUI";


NoiseDeformer::NoiseDeformer(QWidget *parent, Qt::WindowFlags flags) :
		QWidget(parent, flags),
		weightMin(0),
		weightMax(100),
		defaultWeight(0),
		frequencyMin(1),
		frequencyMax(100),
		defaultFrequency(1)
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

	static const char btnApplyHelpText[] = "Bake the current deformation to the mesh.";
	btnApply->setToolTip(btnApplyHelpText);
	btnApply->setStatusTip(btnApplyHelpText);

	QPushButton *btnApplyChangesToLayer = new QPushButton("Apply changes to layer");

	static const char btnApplyChangesToLayerHelpText[] = "Bake the current deformation to a new sculpt layer.";
	btnApplyChangesToLayer->setToolTip(btnApplyChangesToLayerHelpText);
	btnApplyChangesToLayer->setStatusTip(btnApplyChangesToLayerHelpText);

	QPushButton *btnReset = new QPushButton("Reset");

	static const char btnResetHelpText[] = "Resets the mesh to its original state before deformation.";
	btnReset->setToolTip(btnResetHelpText);
	btnReset->setStatusTip(btnResetHelpText);

	QPushButton *btnClose = new QPushButton("Close");

	static const char btnCloseHelpText[] = "Closes this dialog";
	btnClose->setToolTip(btnCloseHelpText);
	btnClose->setStatusTip(btnCloseHelpText);

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
	QLabel *labelPerlinFrequency = new QLabel("Frequency");

	spinBoxFrequencyMin = new QSpinBox(this);
	spinBoxFrequencyMin->setRange(frequencyMin, frequencyMax);
	spinBoxFrequencyMin->setValue(frequencyMin);

	spinBoxFrequencyMax = new QSpinBox(this);
	spinBoxFrequencyMax->setRange(frequencyMin, frequencyMax);
	spinBoxFrequencyMax->setValue(frequencyMax);

	spinBoxFrequency = new QSpinBox(this);
	spinBoxFrequency->setRange(frequencyMin, frequencyMax);
	spinBoxFrequency->setValue(defaultFrequency);

	sliderFrequency = new QSlider(this);
	sliderFrequency->setOrientation(Qt::Horizontal);
	sliderFrequency->setRange(frequencyMin, frequencyMax);
	sliderFrequency->setValue(defaultFrequency);

	QGridLayout *perlinSettingsLayout = new QGridLayout;
	perlinSettingsLayout->addWidget(labelPerlinWeight, 0, 0);
	perlinSettingsLayout->addWidget(spinBoxWeightMin, 0, 1);
	perlinSettingsLayout->addWidget(sliderWeight, 0, 2);
	perlinSettingsLayout->addWidget(spinBoxWeightMax, 0, 3);
	perlinSettingsLayout->addWidget(spinBoxWeight, 0, 4);

	perlinSettingsLayout->addWidget(labelPerlinFrequency, 1, 0);
	perlinSettingsLayout->addWidget(spinBoxFrequencyMin, 1, 1);
	perlinSettingsLayout->addWidget(sliderFrequency, 1, 2);
	perlinSettingsLayout->addWidget(spinBoxFrequencyMax, 1, 3);
	perlinSettingsLayout->addWidget(spinBoxFrequency, 1, 4);

	grpNoiseSettings->setLayout(perlinSettingsLayout);

	mainLayout->addWidget(grpNoiseSettings);

	mainLayout->addWidget(btnApply);
	mainLayout->addWidget(btnApplyChangesToLayer);

	mainLayout->addWidget(btnReset);

	mainLayout->addWidget(btnClose);

	setLayout(mainLayout);

	// NOTE: (sonictk) Signals for the various GUI buttons
	bool result = connect(btnClose, SIGNAL(released()), this, SLOT(close()));
	assert(result == true);

	result = connect(btnApply, SIGNAL(released()), this, SLOT(applyCB()));
	assert(result == true);

	result = connect(btnApplyChangesToLayer, SIGNAL(released()), this, SLOT(applyChangesToLayerCB()));
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

	// NOTE: (sonictk) Signals for the frequency group
	result = connect(sliderFrequency, SIGNAL(valueChanged(int)), this, SLOT(frequencyChangedCB(int)));
	assert(result == true);

	result = connect(spinBoxFrequency, SIGNAL(valueChanged(int)), this, SLOT(setFrequencyCB(int)));
	assert(result == true);

	result = connect(spinBoxFrequencyMin, SIGNAL(valueChanged(int)), this, SLOT(setMinFrequencyCB(int)));
	assert(result == true);

	result = connect(spinBoxFrequencyMax, SIGNAL(valueChanged(int)), this, SLOT(setMaxFrequencyCB(int)));
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

	sliderFrequency->setValue(0);
	spinBoxFrequency->setValue(0);
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

	sliderFrequency->blockSignals(true);
	spinBoxFrequency->blockSignals(true);

	resetSliders();

	sliderWeight->blockSignals(false);
	spinBoxWeight->blockSignals(false);

	sliderFrequency->blockSignals(false);
	spinBoxFrequency->blockSignals(false);
}


bool NoiseDeformer::checkActiveGeometryAndUpdateCache()
{
	Geometry *activeGeo = Kernel()->Scene()->ActiveGeometry();
	if (!activeGeo) {
		activeSubdivLevel = NULL;

		return true;
	}

	SubdivisionLevel *currentSubdivLevel = activeGeo->ActiveLevel();
	size_t origNumPts = origPtPositions.size();
	unsigned int curNumPts = activeSubdivLevel->VertexCount();

	if (currentSubdivLevel != activeSubdivLevel || origNumPts != curNumPts) {
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


void NoiseDeformer::deformPoint(unsigned int index, float weight, int frequency)
{
	Vector origPos = origPtPositions[index];
	Vector finalPos = Vector(origPos);

	float perlinFactor = perlin(finalPos.x / frequency,
								finalPos.y / frequency,
								finalPos.z / frequency);

	finalPos *= 1.0f + (perlinFactor * weight);

	activeSubdivLevel->SetVertexPosition(index, finalPos);
}


NoiseDeformerStatus NoiseDeformer::deform(float weight, int frequency)
{
	if (!checkIfNoGeometrySelectedAndDisplayWarning()) {
		return NoiseDeformerStatus::NOISE_DEFORMER_STATUS_NO_GEOMETRY_SELECTED;
	}

	checkActiveGeometryAndUpdateCache();

	if (!activeSubdivLevel) {
		return NoiseDeformerStatus::NOISE_DEFORMER_STATUS_NO_ACTIVE_SUBDIV_LEVEL;
	}

	// NOTE: (sonictk) If there is an active vertex selection, only apply the
	// deformation to those vertices. Else, apply it to all vertices.
	unsigned int numOfVertices = activeSubdivLevel->VertexCount();

	if (activeSubdivLevel->SelectedFaceCount() > 0) {
		const Vertex *vertexDatas = activeSubdivLevel->VertexArray();

		for (unsigned int i=0; i < numOfVertices; ++i) {
			Vertex currentVertex = vertexDatas[i];
			if (currentVertex.IsSelected()) {
				deformPoint(i, weight, frequency);
			}
		}

	} else {
		for (unsigned int i=0; i < numOfVertices; ++i) {
			deformPoint(i, weight, frequency);
		}
	}

	quickUpdateSubdivisionLevel(activeSubdivLevel);

	return NoiseDeformerStatus::NOISE_DEFORMER_STATUS_SUCCESS;
}


NoiseDeformerStatus NoiseDeformer::resetMesh()
{
	if (!activeSubdivLevel) {
		return NoiseDeformerStatus::NOISE_DEFORMER_STATUS_NO_ACTIVE_SUBDIV_LEVEL;
	}

	unsigned int numOfVertices = activeSubdivLevel->VertexCount();
	if (numOfVertices != origPtPositions.size()) {
		return NoiseDeformerStatus::NOISE_DEFORMER_STATUS_MISMATCHED_SUBDIV_LEVEL;
	}

	for (unsigned int i=0; i < numOfVertices; ++i) {
		activeSubdivLevel->SetVertexPosition(i, origPtPositions[i]);
	}

	updateSubdivisionLevel(activeSubdivLevel);

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


void NoiseDeformer::applyChangesToLayerCB()
{
	resetSlidersWithoutAffectingGeometry();

	if (!checkIfNoGeometrySelectedAndDisplayWarning()) {
		return;
	}

	if (!activeSubdivLevel) {
		return;
	}

	unsigned int numOfAffectedVertices = 0;
	unsigned int numOfVertices = activeSubdivLevel->VertexCount();

	Vector *deltas = new Vector[numOfVertices];

	for (unsigned int i=0; i < numOfVertices; ++i) {
		const Vector curPos = activeSubdivLevel->VertexPosition(i);
		Vector origPos = origPtPositions[i];

		Vector delta = curPos - origPos;
		if (delta == Vector(0, 0, 0)) {
			continue;
		}

		deltas[i] = delta;
		numOfAffectedVertices++;
	}

	resetMesh();
	resetSliders();

	if (numOfAffectedVertices == 0) {
		goto cleanup;
	}

	LayerMeshData *layerMeshData = activeSubdivLevel->AddLayer();
	for (unsigned int j=0; j < numOfVertices; ++j) {
		unsigned int layerVertexIndex = layerMeshData->LayerVertexIndex(j, true);
		layerMeshData->SetVertexDelta(layerVertexIndex, j, deltas[j], true);
	}

	bool status = layerMeshData->FinishChanges();
	if (!status) {
		Kernel()->Interface()->MessageBox(Interface::msgError,
										  "Failed to apply changes!",
										  "Failed to apply changes to the sculpt layer! Please try the operation again!");
	}

cleanup:
	delete deltas;

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

	deform(float(sliderWeight->value()) / 100.0f, sliderFrequency->value());
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


void NoiseDeformer::frequencyChangedCB(int frequency)
{
	spinBoxFrequency->blockSignals(true);
	spinBoxFrequency->setValue(frequency);
	spinBoxFrequency->blockSignals(false);

	deform(float(sliderWeight->value()) / 100.0f, sliderFrequency->value());
}


void NoiseDeformer::setFrequencyCB(int frequency)
{
	sliderFrequency->setValue(frequency);
}


void NoiseDeformer::setMinFrequencyCB(int frequency)
{
	int frequencyMax = spinBoxFrequencyMax->value();
	if (frequency >= frequencyMax) {
		frequency = frequencyMax - 1;
		spinBoxFrequencyMin->setValue(frequency);
	}

	sliderFrequency->setMinimum(frequency);
	spinBoxFrequency->setMinimum(frequency);
}


void NoiseDeformer::setMaxFrequencyCB(int frequency)
{
	int frequencyMin = spinBoxFrequencyMin->value();
	if (frequency <= frequencyMin) {
		frequency = frequencyMin + 1;
		spinBoxFrequencyMax->setValue(frequency);
	}

	sliderFrequency->setMaximum(frequency);
	spinBoxFrequency->setMaximum(frequency);
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
