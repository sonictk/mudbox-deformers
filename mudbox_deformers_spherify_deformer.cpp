#include "mudbox_deformers_spherify_deformer.h"
#include <ssmath/ss_common_math.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <QtCore/Qt>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>

using mudbox::Interface;
using mudbox::Kernel;
using mudbox::Mesh;
using mudbox::Node;
using mudbox::etEventTriggered;
using mudbox::etPointerTargetDestroyed;
using mudbox::etValueChanged;
using mudbox::SubdivisionLevel;
using mudbox::MeshChange;
using std::malloc;
using std::strlen;


// NOTE: (sonictk) For Mudbox RTTI system
IMPLEMENT_CLASS(SpherifyDeformer, TreeNode, SPHERIFY_DEFORMER_NAME)

const char *SpherifyDeformer::displayName = SPHERIFY_DEFORMER_NAME;


SpherifyDeformer::SpherifyDeformer() : Node(SPHERIFY_DEFORMER_NAME),
									   spherifyWeight(this, "Spherify weight"),
									   targetMesh(this, "Target mesh"),
									   applyEvent(this, "Apply"),
									   deleteEvent(this, "Delete"),
									   resetEvent(this, "Reset")
{
	// NOTE: (sonictk) Assign a unique name to this node, starting from 1.
	NUM_OF_SPHERIFY_DEFORMER_NODES++;

	int nameLen = int(strlen(SPHERIFY_DEFORMER_NAME)) + findNumberOfDigits(NUM_OF_SPHERIFY_DEFORMER_NODES);
	char *nodeName = (char *)malloc(sizeof(char) * nameLen + 1);
	snprintf(nodeName, sizet(nameLen), "%s%d", SPHERIFY_DEFORMER_NAME, NUM_OF_SPHERIFY_DEFORMER_NODES);
	QString nodeNameQS(nodeName);
	SetName(nodeNameQS);
	SetDisplayName(QString(displayName));

	free(nodeName);

	targetMesh.m_sNullString = QString("Select a mesh");
	spherifyWeight = 1.0f;
}


QWidget *SpherifyDeformer::CreatePropertiesWindow(QWidget *parent)
{
	QWidget *propertiesWidget = TreeNode::CreatePropertiesWindow(parent);

	// TODO: (sonictk) Figure out why Mudbox keeps references to these widgets and
	// where, since stack alloc crashes after widget closure
	QVBoxLayout *mainLayout = new QVBoxLayout;
	QHBoxLayout *btnLayout = new QHBoxLayout;

	QLabel *weightLabel = new QLabel("Weight");
	QGroupBox *grpSettings = new QGroupBox("Spherify settings");
	QVBoxLayout *grpSettingsLayout = new QVBoxLayout;
	QHBoxLayout *weightLayout = new QHBoxLayout;
	QSlider *sliderWeight = new QSlider(Qt::Horizontal);
	sliderWeight->setRange(0, 100);

	QPushButton *closeBtn = new QPushButton("Close");

	QWidget *widgetHolder = new QWidget;

	propertiesWidget->setWindowTitle(DisplayName());

	weightLayout->addWidget(weightLabel);
	weightLayout->addWidget(sliderWeight);
	grpSettingsLayout->addLayout(weightLayout);
	grpSettings->setLayout(grpSettingsLayout);

	btnLayout->addWidget(closeBtn);

	mainLayout->addWidget(grpSettings);
	mainLayout->addLayout(btnLayout);

	// TODO: (sonictk) Figure out why without the widget holder, nothing shows up
	// TODO: (sonictk) Figure out why there's blank space at the top of the widget
	widgetHolder->setLayout(mainLayout);

	propertiesWidget->connect(closeBtn,
							  SIGNAL(released()),
							  propertiesWidget,
							  SLOT(reject()));

	propertiesWidget->layout()->addWidget(widgetHolder);

	propertiesWidget->show();

	return propertiesWidget;
}


void SpherifyDeformer::spherify()
{
	if (targetMesh == 0) {
		Kernel()->Interface()->MessageBox(Interface::msgError,
										  QString("Cannot apply deformation!"),
										  QString("Please select a mesh first from the drop-down menu!"));
		return;
	}

	// TODO: (sonictk) Apply spherify operation
	SubdivisionLevel *currentSubdivisionLevel = targetMesh->ActiveLevel();

	unsigned int numOfVertices = currentSubdivisionLevel->VertexCount();

	MeshChange *meshChanges = currentSubdivisionLevel->StartChange();

	currentSubdivisionLevel->EndChange();

	Kernel()->Interface()->RefreshUI();
}


void SpherifyDeformer::OnNodeEvent(const Attribute &attribute, NodeEventType eventType)
{
	if (attribute == spherifyWeight) {
		switch (eventType) {
		case etValueChanged: {
			// TODO: (sonictk) Apply the deformation live and see the mesh change
			Kernel()->Interface()->MessageBox(Interface::msgInformation,
											  QString("Test"),
											  QString("Test"));
			break;
		}
		case etPointerTargetDestroyed:
			NUM_OF_SPHERIFY_DEFORMER_NODES--;
			break;
		default:
			break;
		}

	} else if (attribute == applyEvent && eventType == etEventTriggered) {
		// TODO: (sonictk) Apply deformation as undoable command
		spherify();

	} else if (attribute == resetEvent && eventType == etEventTriggered) {
		spherifyWeight = 1.0f;
		Kernel()->Interface()->RefreshUI();

	} else if (attribute == deleteEvent && eventType == etEventTriggered) {
		spherifyWeight = 0.0f;

		// NOTE: (sonictk) Remove the node from the graph first otherwise children
		// will have invalid sibling ptrs.
		Kernel()->Scene()->RemoveChild(this);
		Kernel()->Interface()->RefreshUI();

		delete this;
	}
}
