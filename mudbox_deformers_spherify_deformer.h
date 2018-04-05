/**
 * @file   mudbox_deformers_spherify_deformer.h
 * @brief
 */
#ifndef MUDBOX_DEFORMERS_SPHERIFY_DEFORMER_H
#define MUDBOX_DEFORMERS_SPHERIFY_DEFORMER_H

#include <Mudbox/mudbox.h>
#include <QtGui/QWidget>
#include <QtCore/QString>

using mudbox::TreeNode;
using mudbox::Attribute;
using mudbox::NodeEventType;
using mudbox::Geometry;
using mudbox::AttributePointer;
using mudbox::AttributeInstance;
using mudbox::EventGate;


struct SpherifyDeformer : TreeNode
{
	DECLARE_CLASS // NOTE: (sonictk) Required for Mudbox RTTI system.

	/// The display name of the node. May be translated according to language settings.
	static const char *displayName;

	aptr<Geometry> targetObj;

	AttributeInstance<float> spherifyWeight;

	EventGate runEvent;
	EventGate deleteEvent;

	SpherifyDeformer();

	/**
	 * Overridden to provide the properties window for modifying the deformer attributes.
	 *
	 * @param parent	The parent widget.
	 *
	 * @return			An instance of the properties window.
	 */
	virtual QWidget *CreatePropertiesWindow(QWidget *parent);

	/**
	 * Overriden to allow for setting display name of the node.
	 *
	 * @param displayName	The name to set.
	 */
	virtual void SetDisplayName(const QString &displayName);

	virtual void OnNodeEvent(const Attribute &attribute, NodeEventType eventType);
};


#endif /* MUDBOX_DEFORMERS_SPHERIFY_DEFORMER_H */
