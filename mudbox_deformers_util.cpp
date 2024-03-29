#include "mudbox_deformers_util.h"

#include <Mudbox/mudbox.h>

using mudbox::Kernel;
using mudbox::SubdivisionLevel;
using mudbox::MeshRenderer;
using mudbox::Geometry;
using mudbox::Interface;


void markSubdivisionLevelDirty(SubdivisionLevel *subdivLevel)
{
	MeshRenderer *renderer;
	for (unsigned int i=0; renderer = subdivLevel->ChildByClass<MeshRenderer>(i); ++i) {
		for (unsigned int f=0; f < subdivLevel->FaceCount(); ++f) {
			for (int j=0; j < 4; ++j) {
				renderer->OnVertexPositionChange(subdivLevel->QuadIndex(f, j), f);
			}
		}
	}
}


void updateSubdivisionLevel(SubdivisionLevel *subdivLevel)
{
	subdivLevel->ApplyChanges(true);
	subdivLevel->RecalculateAdjacency();
	subdivLevel->RecalculateNormals();

	// NOTE: (sonictk) Mark the mesh components dirty and notify all the renderers
	// to redraw the mesh
	markSubdivisionLevelDirty(subdivLevel);

	Kernel()->ViewPort()->Redraw();
}


void quickUpdateSubdivisionLevel(SubdivisionLevel *subdivLevel)
{
	subdivLevel->ApplyChanges(true);

	// NOTE: (sonictk) Mark the mesh components dirty and notify all the renderers
	// to redraw the mesh
	markSubdivisionLevelDirty(subdivLevel);

	Kernel()->ViewPort()->Redraw();
}


bool checkIfNoGeometrySelectedAndDisplayWarning()
{
	Geometry *currentActiveGeo = Kernel()->Scene()->ActiveGeometry();
	if (!currentActiveGeo) {
		Kernel()->Interface()->MessageBox(Interface::msgError,
										  "No mesh selected!",
										  "You need to select a mesh first!");
		return false;
	}

	return true;
}
