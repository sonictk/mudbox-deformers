#include "mudbox_deformers_noise_deformer.h"


NoiseDeformer *NoiseDeformer::existingWidget = NULL;

const QString NoiseDeformer::objName = "noiseDeformerUI";


NoiseDeformer::NoiseDeformer(QWidget *parent, Qt::WindowFlags flags)
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
}


NoiseDeformer::~NoiseDeformer()
{
	existingWidget = NULL;
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
