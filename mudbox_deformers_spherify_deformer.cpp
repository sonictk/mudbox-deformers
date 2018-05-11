#include <ssmath/ss_common_math.h>
#include "mudbox_deformers_spherify_deformer.h"
#include "mudbox_deformers_util.h"
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
#include <QtGui/QMessageBox>

using mudbox::AxisAlignedBoundingBox;
using mudbox::Interface;
using mudbox::Kernel;
using mudbox::Mesh;
using mudbox::MeshChange;
using mudbox::MeshRenderer;
using mudbox::Node;
using mudbox::Scene;
using mudbox::SubdivisionLevel;
using mudbox::etEventTriggered;
using mudbox::etPointerContentChanged;
using mudbox::etPointerTargetDestroyed;
using mudbox::etValueChanged;

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

	updateOriginalPointPositions();

	spherifyWeight.SetValue(defaultWeight);

	// NOTE: (sonictk) Connect the scene member event to the global one so that
	// we can catch the deletion event of the target mesh if it happens and delete
	// this node as well
	sceneEvent.Connect(Kernel()->Scene()->SceneMembershipEvent);
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


void SpherifyDeformer::updateOriginalPointPositions()
{
	origPtPositions.clear();
	SubdivisionLevel *currentSubdivisionLevel = targetMesh->ActiveLevel();
	AxisAlignedBoundingBox bBox = currentSubdivisionLevel->BoundingBox(false);
	unsigned int numOfVertices = currentSubdivisionLevel->VertexCount();

	for (unsigned int i=0; i < numOfVertices; ++i) {
		Vector pos = currentSubdivisionLevel->VertexPosition(i);
		origPtPositions.push_back(pos);
	}

	origBBoxMaxLength = bBox.Size();
}


void SpherifyDeformer::spherifyCB(float weight)
{
	SubdivisionLevel *currentSubdivisionLevel = targetMesh->ActiveLevel();
	unsigned int numOfVertices = currentSubdivisionLevel->VertexCount();

	// NOTE: (sonictk) If the vertices no longer match, re-cache the mesh point positions.
	if (numOfVertices != (unsigned int)origPtPositions.size()) {
		Kernel()->Interface()->HUDMessageShow("Mis-matching vertex counts! Please "
											  "make sure you're on the same subd level "
											  "that you started with!");

		return;
	}

	for (unsigned int i=0; i < numOfVertices; ++i) {
		Vector origPos = origPtPositions[i];
		Vector newPos = Vector(origPos);
		newPos.SetLength(origBBoxMaxLength);
		Vector finalPos = lerp(origPos, newPos, weight);
		currentSubdivisionLevel->SetVertexPosition(i, finalPos);
	}

	currentSubdivisionLevel->ApplyChanges(true);

	markSubdivisionLevelDirty(currentSubdivisionLevel);
	Kernel()->ViewPort()->Redraw();
}


void SpherifyDeformer::OnNodeEvent(const Attribute &attribute, NodeEventType eventType)
{
	// TODO: (sonictk) Figure out how to get this callback working
	if (attribute == sceneEvent) {
		Kernel()->Interface()->HUDMessageShow("test");
	}

	if (attribute == targetMesh) {
		switch (eventType) {

		case etPointerTargetDestroyed:
			origPtPositions.clear();
			origBBoxMaxLength = 0.0f;
			break;

		case etPointerContentChanged:
		case etValueChanged:
			if (targetMesh) {
				updateOriginalPointPositions();
			}
			break;

		default:
			break;
		}

	} else if (attribute == spherifyWeight) {
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

	} else if (attribute == applyEvent && eventType == etEventTriggered && targetMesh) {
		SubdivisionLevel *currentSubdivisionLevel = targetMesh->ActiveLevel();
		updateSubdivisionLevel(currentSubdivisionLevel);

		return;

	} else if (attribute == resetEvent && eventType == etEventTriggered) {
		if (targetMesh) {
			SubdivisionLevel *currentSubdivisionLevel = targetMesh->ActiveLevel();
			unsigned int numOfVertices = currentSubdivisionLevel->VertexCount();

			if (numOfVertices != (unsigned int)origPtPositions.size()) {
				Kernel()->Interface()->HUDMessageShow("Mis-matching vertex counts! "
													  "Unable to reset the original "
													  "mesh point positions!");

				return;
			}

			for (unsigned int i=0; i < numOfVertices; ++i) {
				Vector origPos = origPtPositions[i];
				currentSubdivisionLevel->SetVertexPosition(i, origPos);
			}

			updateSubdivisionLevel(currentSubdivisionLevel);
		}

		spherifyWeight.SetValue(defaultWeight);

	} else if (attribute == deleteEvent && eventType == etEventTriggered) {
		if (targetMesh) {
			SubdivisionLevel *currentSubdivisionLevel = targetMesh->ActiveLevel();
			unsigned int numOfVertices = currentSubdivisionLevel->VertexCount();

			if (numOfVertices != (unsigned int)origPtPositions.size()) {
				Kernel()->Interface()->HUDMessageShow("Mis-matching vertex counts! "
													  "Unable to restore the original "
													  "mesh point positions!");

				return;
			}

			for (unsigned int i=0; i < numOfVertices; ++i) {
				Vector origPos = origPtPositions[i];
				currentSubdivisionLevel->SetVertexPosition(i, origPos);
			}

			updateSubdivisionLevel(currentSubdivisionLevel);
		}

		// NOTE: (sonictk) Remove the node from the graph first otherwise children
		// will have invalid sibling ptrs.
		Kernel()->Scene()->RemoveChild(this);
		// NOTE: (sonictk) If the UI is not refreshed before ``this`` is deleted,
		// Mudbox crashes. Guess the UI holds a reference to the ``TreeNode`` class
		// instance or something. Wonderful.
		Kernel()->Interface()->RefreshUI();

		delete this;
	}
}
