#include "mudbox_deformers_main.h"
#include <Mudbox/mudbox.h>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtGui/QAction>
#include <QtGui/QMenu>


using mudbox::Node;
using mudbox::Kernel;
using mudbox::Interface;


// NOTE: (sonictk) Creates an instance of the ``Plugin`` class as per-documentation;
// this is required for plugin initialization
MB_PLUGIN(MUDBOX_DEFORMERS_PLUGIN_NAME,
		  MUDBOX_DEFORMERS_PLUGIN_DESCRIPTION,
		  MUDBOX_DEFORMERS_PLUGIN_AUTHOR,
		  MUDBOX_DEFORMERS_PLUGIN_URL,
		  initializer);


void initializer()
{
	const char deformersMenuName[] = "Deformers";

	// TODO: (sonictk) Get the version of mudbox being used and modify the
	// initialization accordingly since the signature here changes
	Kernel()->Interface()->AddCallbackMenuItem(deformersMenuName,
											   "Bend",
											   "Bend mesh",
											   showBendDeformerUI);

	// Kernel()->Interface()->AddCallbackMenuItem(Interface::menuMesh,
	// 										   QString(deformersMenuName),
	// 										   QString("Spherify"),
	// 										   showSpherifyDeformerUI);

	Kernel()->Interface()->AddClassMenuItem(deformersMenuName,
											"Spherify",
											SpherifyDeformer::StaticClass(),
											"New Spherify Operation");
}
