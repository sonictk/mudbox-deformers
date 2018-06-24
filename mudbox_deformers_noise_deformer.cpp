#include "mudbox_deformers_noise_deformer.h"

#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>


NoiseDeformer *NoiseDeformer::existingWidget = NULL;

const QString NoiseDeformer::objName = "noiseDeformerUI";


NoiseDeformer::NoiseDeformer(QWidget *parent, Qt::WindowFlags flags) :
		weightMin(0),
		weightMax(100),
		weight(0),
		octavesMin(1),
		octavesMax(10),
		octaves(1)
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

	QVBoxLayout *noiseSettingsLayout = new QVBoxLayout;

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
	spinBoxWeight->setValue(weight);

	sliderWeight = new QSlider(this);
	sliderWeight->setOrientation(Qt::Horizontal);
	sliderWeight->setRange(weightMin, weightMax);
	sliderWeight->setValue(weight);

	QHBoxLayout *perlinWeightLayout = new QHBoxLayout;
	perlinWeightLayout->addWidget(labelPerlinWeight);
	perlinWeightLayout->addWidget(spinBoxWeightMin);
	perlinWeightLayout->addWidget(sliderWeight);
	perlinWeightLayout->addWidget(spinBoxWeightMax);
	perlinWeightLayout->addWidget(spinBoxWeight);

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
	spinBoxOctaves->setValue(octaves);

	sliderOctaves = new QSlider(this);
	sliderOctaves->setOrientation(Qt::Horizontal);
	sliderOctaves->setRange(octavesMin, octavesMax);
	sliderOctaves->setValue(octaves);

	QHBoxLayout *perlinOctavesLayout = new QHBoxLayout;
	perlinOctavesLayout->addWidget(labelPerlinOctaves);
	perlinOctavesLayout->addWidget(spinBoxOctavesMin);
	perlinOctavesLayout->addWidget(sliderOctaves);
	perlinOctavesLayout->addWidget(spinBoxOctavesMax);
	perlinOctavesLayout->addWidget(spinBoxOctaves);

	noiseSettingsLayout->addLayout(perlinWeightLayout);
	noiseSettingsLayout->addLayout(perlinOctavesLayout);

	grpNoiseSettings->setLayout(noiseSettingsLayout);

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
}


NoiseDeformer::~NoiseDeformer()
{
	existingWidget = NULL;
}


void NoiseDeformer::applyCB()
{
	// TODO: (sonictk)
}


void NoiseDeformer::resetCB()
{
	// TODO: (sonictk)
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
