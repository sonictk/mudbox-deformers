#include <ssmath/ss_common_math.h>
#include "mudbox_deformers_spherify_deformer.h"
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

	if (numOfVertices != (unsigned int)origPtPositions.size()) {
		QMessageBox::critical(Kernel()->Interface()->MainWindow(),
							  "Failed to apply spherify operation!",
							  "The mesh in memory and the current mesh do not "
							  "have the same number of vertices! Please make sure that you're on the same subdivision level that you started the operation with!");

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

	// NOTE: (sonictk) Mark the mesh components dirty and notify all the renderers
	// to redraw the mesh
	MeshRenderer *renderer;
	for (unsigned int i=0; renderer = currentSubdivisionLevel->ChildByClass<MeshRenderer>(i); ++i) {
		for (unsigned int f=0; f < currentSubdivisionLevel->FaceCount(); ++f) {
			for (int j=0; j < 4; ++j) {
				renderer->OnVertexPositionChange(currentSubdivisionLevel->QuadIndex(f, j), f);
			}
		}
	}

	Kernel()->ViewPort()->Redraw();
}


void SpherifyDeformer::OnNodeEvent(const Attribute &attribute, NodeEventType eventType)
{
	if (attribute == targetMesh) {
		if (eventType == etPointerTargetDestroyed) {
			origPtPositions.clear();
			origBBoxMaxLength = 0.0f;

			return;
		}

		SubdivisionLevel *currentSubdivisionLevel = targetMesh->ActiveLevel();
		AxisAlignedBoundingBox bBox = currentSubdivisionLevel->BoundingBox(false);
		unsigned int numOfVertices = currentSubdivisionLevel->VertexCount();

		origPtPositions.clear();

		for (unsigned int i=0; i < numOfVertices; ++i) {
			Vector pos = currentSubdivisionLevel->VertexPosition(i);
			origPtPositions.push_back(pos);
		}

		origBBoxMaxLength = bBox.Size();

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

	} else if (attribute == applyEvent && eventType == etEventTriggered) {
		SubdivisionLevel *currentSubdivisionLevel = targetMesh->ActiveLevel();
		currentSubdivisionLevel->RecalculateAdjacency();
		currentSubdivisionLevel->RecalculateNormals();

	} else if (attribute == resetEvent && eventType == etEventTriggered) {
		// SubdivisionLevel *currentSubdivisionLevel = targetMesh->ActiveLevel();
		// unsigned int numOfVertices = currentSubdivisionLevel->VertexCount();

		// if (numOfVertices != (unsigned int)origPtPositions.size()) {
		// 	QMessageBox::critical(Kernel()->Interface()->MainWindow(),
		// 						  "Failed to apply spherify operation!",
		// 						  "The mesh in memory and the current mesh do not "
		// 						  "have the same number of vertices! Please make sure that you're on the same subdivision level that you started the operation with!");

		// 	return;
		// }

		// for (unsigned int i=0; i < numOfVertices; ++i) {
		// 	Vector origPos = origPtPositions[i];
		// 	currentSubdivisionLevel->SetVertexPosition(i, origPos);
		// }

		// currentSubdivisionLevel->ApplyChanges(true);
		// currentSubdivisionLevel->RecalculateAdjacency();
		// currentSubdivisionLevel->RecalculateNormals();

		// // NOTE: (sonictk) Mark the mesh components dirty and notify all the renderers
		// // to redraw the mesh
		// MeshRenderer *renderer;
		// for (unsigned int i=0; renderer = currentSubdivisionLevel->ChildByClass<MeshRenderer>(i); ++i) {
		// 	for (unsigned int f=0; f < currentSubdivisionLevel->FaceCount(); ++f) {
		// 		for (int j=0; j < 4; ++j) {
		// 			renderer->OnVertexPositionChange(currentSubdivisionLevel->QuadIndex(f, j), f);
		// 		}
		// 	}
		// }
		// Kernel()->ViewPort()->Redraw();

		spherifyWeight.SetValue(defaultWeight);

	} else if (attribute == deleteEvent && eventType == etEventTriggered) {
		// TODO: (sonictk) Undo results without triggering a crash
		// SubdivisionLevel *currentSubdivisionLevel = targetMesh->ActiveLevel();
		// unsigned int numOfVertices = currentSubdivisionLevel->VertexCount();

		// if (numOfVertices != (unsigned int)origPtPositions.size()) {
		// 	QMessageBox::critical(Kernel()->Interface()->MainWindow(),
		// 						  "Failed to apply spherify operation!",
		// 						  "The mesh in memory and the current mesh do not "
		// 						  "have the same number of vertices! Please make sure that you're on the same subdivision level that you started the operation with!");

		// 	return;
		// }

		// for (unsigned int i=0; i < numOfVertices; ++i) {
		// 	Vector origPos = origPtPositions[i];
		// 	currentSubdivisionLevel->SetVertexPosition(i, origPos);
		// }

		// currentSubdivisionLevel->ApplyChanges(true);

		// // NOTE: (sonictk) Mark the mesh components dirty and notify all the renderers
		// // to redraw the mesh
		// MeshRenderer *renderer;
		// for (unsigned int i=0; renderer = currentSubdivisionLevel->ChildByClass<MeshRenderer>(i); ++i) {
		// 	for (unsigned int f=0; f < currentSubdivisionLevel->FaceCount(); ++f) {
		// 		for (int j=0; j < 4; ++j) {
		// 			renderer->OnVertexPositionChange(currentSubdivisionLevel->QuadIndex(f, j), f);
		// 		}
		// 	}
		// }

		// Kernel()->ViewPort()->Redraw();

		// NOTE: (sonictk) Remove the node from the graph first otherwise children
		// will have invalid sibling ptrs.
		// Kernel()->Scene()->RemoveChild(this);

		// delete this;
	}
}
