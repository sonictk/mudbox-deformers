#include "mudbox_deformers_bend_deformer.h"
#include <cassert>
#include <Mudbox/mudbox.h>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QSlider>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>

using mudbox::Kernel;
using mudbox::Geometry;
using mudbox::SubdivisionLevel;
using mudbox::Interface;
using mudbox::MeshRenderer;


BendDeformer *BendDeformer::existingWidget = NULL;

const QString BendDeformer::objName = "bendDeformerUI";


BendDeformer::BendDeformer(QWidget *parent, Qt::WindowFlags flags) : QWidget(parent, flags)
{
	setObjectName(objName);
	setWindowTitle("Apply Bend Deformer");

	QVBoxLayout *mainLayout = new QVBoxLayout;

	QVBoxLayout *settingsGrpLayout = new QVBoxLayout;
	QGroupBox *settingsGrp = new QGroupBox("Bend settings");

	QHBoxLayout *weightLayout = new QHBoxLayout;
	QLabel *sliderLabel = new QLabel("Weight");
	QSlider *sliderWeight = new QSlider(Qt::Horizontal);
	sliderWeight->setRange(0, 100);

	QPushButton *closeBtn = new QPushButton("Close");

	QPushButton *testBtn = new QPushButton("Test");

	weightLayout->addWidget(sliderLabel);
	weightLayout->addWidget(sliderWeight);

	settingsGrpLayout->addLayout(weightLayout);
	settingsGrp->setLayout(settingsGrpLayout);

	mainLayout->addWidget(settingsGrp);
	mainLayout->addWidget(closeBtn);

	mainLayout->addWidget(testBtn);
	setLayout(mainLayout);

	bool result = connect(closeBtn, SIGNAL(released()), this, SLOT(close()));
	assert(result == true);

	result = connect(sliderWeight, SIGNAL(valueChanged(int)), this, SLOT(bendCB(int)));
	assert(result == true);
}


BendDeformer::~BendDeformer()
{
	existingWidget = NULL;
}


void BendDeformer::bendCB(int weight)
{
	Kernel()->Interface()->HUDMessageShow("test");

	float bendWeight = 1.0f / float(weight);

	Geometry *activeGeo = Kernel()->Scene()->ActiveGeometry();
	if (!activeGeo) {
		Kernel()->Interface()->MessageBox(Interface::msgError,
										  "No mesh selected!",
										  "You need to select a mesh first!");
		return;
	}

	SubdivisionLevel *activeSubdivLevel = activeGeo->ActiveLevel();

	unsigned int numOfVertices = activeSubdivLevel->VertexCount();

	// TODO: (sonictk) Make this actually bend
	for (unsigned int i=0; i < numOfVertices; ++i) {
		Vector pos = activeSubdivLevel->VertexPosition(i);
		pos *= weight;
		activeSubdivLevel->SetVertexPosition(i, pos);
	}

	// NOTE: (sonictk) Propagate all changes across all subdivision levels.
	activeSubdivLevel->ApplyChanges(true);

	// NOTE: (sonictk) Mark the mesh components dirty and notify all the renderers
	// to redraw the mesh.
	MeshRenderer *renderer;
	for (unsigned int i=0; renderer = activeSubdivLevel->ChildByClass<MeshRenderer>(i); ++i) {
		for (unsigned int f=0; f < activeSubdivLevel->FaceCount(); ++f) {
			for (int j=0; j < 4; ++j) {
				renderer->OnVertexPositionChange(activeSubdivLevel->QuadIndex(f, j), f);
			}
		}
	}

	// NOTE: (sonictk) Still need to refresh the viewport after the renderers
	// have been notified.
	Kernel()->ViewPort()->Redraw();
}


void showBendDeformerUI()
{
	QWidget *mudboxMainWindow = Kernel()->Interface()->MainWindow();

	// NOTE: (sonictk) Ensure that only one instance of the window exists
	if (!BendDeformer::existingWidget) {
		BendDeformer *ui = new BendDeformer(mudboxMainWindow, Qt::Window);
		BendDeformer::existingWidget = ui;
	}

	BendDeformer::existingWidget->show();
	BendDeformer::existingWidget->raise();
}
