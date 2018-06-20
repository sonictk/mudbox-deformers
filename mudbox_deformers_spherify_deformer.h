/**
 * @file   mudbox_deformers_spherify_deformer.h
 * @brief  Spherify deformer.
 */
#ifndef MUDBOX_DEFORMERS_SPHERIFY_DEFORMER_H
#define MUDBOX_DEFORMERS_SPHERIFY_DEFORMER_H

#include <Mudbox/mudbox.h>

#include <QtGui/QWidget>
#include <QtGui/QSlider>
#include <QtCore/QString>

#include <vector>


static const char SPHERIFY_DEFORMER_NAME[] = "SpherifyDeformer";

/// This is used to keep track of how many deformers are in the scene.
static int NUM_OF_SPHERIFY_DEFORMER_NODES = 0;


/// This is a Spherify Deformer node that appears in the Mudbox scene graph. It
/// allows for performing a "bloat" effect on the selected mesh.
struct SpherifyDeformer : mudbox::TreeNode
{
	DECLARE_CLASS // NOTE: (sonictk) Required for Mudbox RTTI system.

	/// The default weight for the spherification to be applied.
	static const float defaultWeight;

	/// The display name of the node. May be translated according to language settings.
	static const char *displayName;

	/// The mesh selector combobox.
	mudbox::aptr<mudbox::Geometry> targetMesh;

	/// Used for detecting when the mesh that this deformer is associated with is
	/// deleted; cleanup of this node is performed at the same time.
	mudbox::aptr<mudbox::SceneMembershipEventNotifier> sceneEvent;

	/// Internal storage used for the original mesh positions.
	std::vector<mudbox::Vector> origPtPositions;

	/// Internal storage for the original max. edge length of the mesh bounding box.
	float origBBoxMaxLength;

	/// The slider for the amount of spherification to apply.
	mudbox::afloatr spherifyWeight;

	/// The button for applying the spherification. (Actually updates the mesh surface)
	mudbox::aevent applyEvent;

	/// The button for removing the operation.
	mudbox::aevent deleteEvent;

	/// The button for resetting the operation.
	mudbox::aevent resetEvent;

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

	/**
	 * This callback is executed when UI attributes are changed. It is responsible
	 * for executing the spherify operation.
	 *
	 * @param attribute	The attribute being affected.
	 * @param eventType	The type of event being triggered.
	 */
	virtual void OnNodeEvent(const mudbox::Attribute &attribute,
							 mudbox::NodeEventType eventType);
};


#endif /* MUDBOX_DEFORMERS_SPHERIFY_DEFORMER_H */
