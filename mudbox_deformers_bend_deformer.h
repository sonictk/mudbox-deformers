/**
 * @file   mudbox_deformers_bend_deformer.h
 * @brief
 */
#ifndef MUDBOX_DEFORMERS_BEND_DEFORMER_H
#define MUDBOX_DEFORMERS_BEND_DEFORMER_H

#include <Mudbox/mudbox.h>
#include <QtGui/QWidget>
#include <QtGui/QSlider>
#include <QtGui/QComboBox>
#include <QtCore/QString>
#include <QtGui/QSpinBox>

#include <vector>


static const char BEND_DEFORMER_NAME[] = "BendDeformer";


enum BendDeformerAxis
{
	BEND_DEFORMER_AXIS_X,
	BEND_DEFORMER_AXIS_Y,
	BEND_DEFORMER_AXIS_Z
};


/// A deformer that allows for bending meshes.
struct BendDeformer : public QWidget
{
	Q_OBJECT

public:
	BendDeformer(QWidget *parent, Qt::WindowFlags flags);

	~BendDeformer();

	/// Global variable that holds reference to the UI that is already open.
	static BendDeformer *existingWidget;

	/// Name of the Qt object.
	static const QString objName;

	int bendAngleMin;
	int bendAngleMax;

	std::vector<mudbox::Vector> origPtPositions;

	mudbox::Geometry *activeGeo;

	QComboBox *comboBoxBendAxis;

	QSpinBox *spinBoxBendRangeStartAngle;

	QSpinBox *spinBoxBendRangeEndAngle;

	QSlider *sliderBendAngle;

public slots:
	/// This callback is triggered when the user moves the weight slider in the UI.
	/// It is responsible for bending the actively-selected mesh.
	void bendCB(int angle);

	void applyCB();

	void resetCB();

	void updateOriginalPointPositions();
};


/**
 * Displays the Bend deformer UI.
 *
 */
void showBendDeformerUI();


#endif /* MUDBOX_DEFORMERS_BEND_DEFORMER_H */
