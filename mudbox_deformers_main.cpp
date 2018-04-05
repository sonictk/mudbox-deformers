#include <Mudbox/mudbox.h>
#include <QtCore/QString>

#include "mudbox_deformers_main.h"

using mudbox::Node;
using mudbox::Kernel;
using mudbox::Interface;


// NOTE: (sonictk) Creates an instance of the ``Plugin`` class as per-documentation
MB_PLUGIN(MUDBOX_DEFORMERS_PLUGIN_NAME,
		  MUDBOX_DEFORMERS_PLUGIN_DESCRIPTION,
		  MUDBOX_DEFORMERS_PLUGIN_AUTHOR,
		  MUDBOX_DEFORMERS_PLUGIN_URL,
		  initializer);


void initializer()
{
	// Kernel()->Interface()->AddCallbackMenuItem(Interface::menuMesh,
	// 										   QString("Deformers"),
	// 										   QString("Spherify"),
	// 										   needsAVoidReturnFunction,
	// 										   QString(""),
	// 										   -1,
	// 										   true,
	// 										   0);

	Kernel()->Interface()->AddClassMenuItem(Interface::menuMesh,
											"Deformers",
											SpherifyDeformer::StaticClass(),
											"Spherify");

}
