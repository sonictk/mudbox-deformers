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
using mudbox::Vector;
using mudbox::AxisAlignedBoundingBox;
using mudbox::Scene;
using mudbox::Geometry;

using std::malloc;
using std::strlen;


// NOTE: (sonictk) For Mudbox RTTI system
IMPLEMENT_CLASS(SpherifyDeformer, TreeNode, SPHERIFY_DEFORMER_NAME)

const char *SpherifyDeformer::displayName = SPHERIFY_DEFORMER_NAME;

const float SpherifyDeformer::defaultWeight = 0.0f;


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

	Geometry *activeGeo = Kernel()->Scene()->ActiveGeometry();
	targetMesh = activeGeo;

	spherifyWeight.SetValue(defaultWeight);
}


QWidget *SpherifyDeformer::CreatePropertiesWindow(QWidget *parent)
{
	QWidget *propertiesWidget = TreeNode::CreatePropertiesWindow(parent);

	// TODO: (sonictk) Figure out why Mudbox keeps references to these widgets and
	// where, since stack alloc crashes after widget closure
	QVBoxLayout *mainLayout = new QVBoxLayout;
	QPushButton *closeBtn = new QPushButton("Close");

	// TODO: (sonictk) Figure out why without the widget holder, nothing shows up
	// TODO: (sonictk) Figure out why there's blank space at the top of the widget
	mainLayout->addWidget(closeBtn);

	QWidget *widgetHolder = new QWidget;
	widgetHolder->setLayout(mainLayout);

	propertiesWidget->connect(closeBtn,
							  SIGNAL(released()),
							  propertiesWidget,
							  SLOT(reject()));

	propertiesWidget->setWindowTitle(DisplayName());
	propertiesWidget->layout()->addWidget(widgetHolder);
	propertiesWidget->show();
	// TODO: (sonictk) Figure out why the mudbox widget attribute holder doesn't
	// resize to its contents automatically
	propertiesWidget->setMinimumHeight(propertiesWidget->height() + 50);

	return propertiesWidget;
}


void SpherifyDeformer::spherifyCB(float weight)
{
	SubdivisionLevel *currentSubdivisionLevel = targetMesh->ActiveLevel();
	unsigned int numOfVertices = currentSubdivisionLevel->VertexCount();

	// TODO: (sonictk) Figure out why the mesh isn't updating real-time
	// TODO: (sonictk) Live update must take application into account
	AxisAlignedBoundingBox bBox = currentSubdivisionLevel->BoundingBox(false);
	float bBoxMaxLength = bBox.Size();
	for (unsigned int i=0; i < numOfVertices; ++i) {
		Vector pos = currentSubdivisionLevel->VertexPosition(i);
		pos.SetLength(bBoxMaxLength * weight);
		currentSubdivisionLevel->SetVertexPosition(i, pos);
	}

	currentSubdivisionLevel->ApplyChanges(true);
	currentSubdivisionLevel->ContentChanged();
	currentSubdivisionLevel->RecalculateAdjacency();
	currentSubdivisionLevel->RecalculateNormals();
	currentSubdivisionLevel->Modified.Trigger();

	Kernel()->ViewPort()->Redraw();
	Kernel()->Interface()->RefreshUI();
}


void SpherifyDeformer::OnNodeEvent(const Attribute &attribute, NodeEventType eventType)
{
	if (attribute == spherifyWeight) {
		switch (eventType) {
		case etValueChanged: {
			float weight = spherifyWeight.Value();
			if (areFloatsEqual(weight, 0.0f)) {
				return;
			}

			// TODO: (sonictk) This check seems to be worthless
			if (!targetMesh) {
				Kernel()->Interface()->MessageBox(Interface::msgError,
												  QString("Cannot apply deformation!"),
												  QString("Please select a mesh first from the drop-down menu!"));
				return;
			}

			spherifyCB(weight);

			break;
		}
		case etPointerTargetDestroyed:
			NUM_OF_SPHERIFY_DEFORMER_NODES--;
			break;
		default:
			break;
		}

	} else if (attribute == applyEvent && eventType == etEventTriggered) {
		// TODO: (sonictk) Save results
		Kernel()->Interface()->RefreshUI();

	} else if (attribute == resetEvent && eventType == etEventTriggered) {
		// TODO: (sonictk) Undo results
		spherifyWeight.SetValue(defaultWeight);
		Kernel()->Interface()->RefreshUI();

	} else if (attribute == deleteEvent && eventType == etEventTriggered) {
		// TODO: (sonictk) Undo results
		// NOTE: (sonictk) Remove the node from the graph first otherwise children
		// will have invalid sibling ptrs.
		Kernel()->Scene()->RemoveChild(this);
		Kernel()->Interface()->RefreshUI();

		delete this;
	}
}
