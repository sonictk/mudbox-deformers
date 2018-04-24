/**
 * @file   mudbox_deformers_spherify_deformer.h
 * @brief
 */
#ifndef MUDBOX_DEFORMERS_SPHERIFY_DEFORMER_H
#define MUDBOX_DEFORMERS_SPHERIFY_DEFORMER_H

#include <Mudbox/mudbox.h>
#include <QtGui/QWidget>
#include <QtGui/QSlider>
#include <QtCore/QString>

#include <vector>

using mudbox::Attribute;
using mudbox::Geometry;
using mudbox::NodeEventType;
using mudbox::TreeNode;
using mudbox::Vector;
using mudbox::aevent;
using mudbox::afloatr;
using mudbox::aptr;

using std::vector;


globalVar const char SPHERIFY_DEFORMER_NAME[] = "SpherifyDeformer";

/// This is used to keep track of how many deformers are in the scene.
globalVar int NUM_OF_SPHERIFY_DEFORMER_NODES = 0;


struct SpherifyDeformer : TreeNode
{
	DECLARE_CLASS // NOTE: (sonictk) Required for Mudbox RTTI system.

	static const float defaultWeight;

	/// The display name of the node. May be translated according to language settings.
	static const char *displayName;

	/// The mesh selector combobox.
	aptr<Geometry> targetMesh;

	/// Internal storage used for the original mesh positions.
	vector<Vector> origPtPositions;

	/// Internal storage for the original max. edge length of the mesh bounding box.
	float origBBoxMaxLength;

	afloatr spherifyWeight;

	aevent applyEvent;
	aevent deleteEvent;
	aevent resetEvent;

	/**
	 * The constructor. Creates the UI and sets up default values.
	 *
	 * @return		A new instance of the deformer class.
	 */
	SpherifyDeformer();

	/**
	 * This updates the internal cache with the original positions of the mesh.
	 * It is used for blending the spherify weights against the original mesh.
	 */
	void updateOriginalPointPositions();

	/**
	 * This callback is executed when the weight slider's value is changed. It is
	 * responsible for applying the spherify operation to the mesh.
	 *
	 * @param weight	The amount of spherification to apply.
	 */
	void spherifyCB(float weight);

	/**
	 * Overridden to provide the properties window for modifying the deformer attributes.
	 *
	 * @param parent	The parent widget.
	 *
	 * @return			An instance of the properties window.
	 */
	virtual QWidget *CreatePropertiesWindow(QWidget *parent);

	virtual void OnNodeEvent(const Attribute &attribute, NodeEventType eventType);
};


#endif /* MUDBOX_DEFORMERS_SPHERIFY_DEFORMER_H */
