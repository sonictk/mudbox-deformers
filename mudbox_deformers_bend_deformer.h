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
#include <QtGui/QCloseEvent>

#include <vector>


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

	/// The minimum bend angle limit.
	int bendAngleMin;

	/// The maximum bend angle limit.
	int bendAngleMax;

	/// Storage for the original positions of the mesh before the deformation application.
	std::vector<mudbox::Vector> origPtPositions;

	/// Storage for the original active geometry being deformed.
	mudbox::Geometry *activeGeo;

	/// Widget that allows selection of the cardinal axis along which to perform deformation.
	QComboBox *comboBoxBendAxis;

	/// Widget that specifies lower bound for bend angle.
	QSpinBox *spinBoxBendRangeStartAngle;

	/// Widget that specifies upper bound for bend angle.
	QSpinBox *spinBoxBendRangeEndAngle;

	/// Widget that specifies bend angle.
	QSpinBox *spinBoxBendAngle;

	/// Widget that allows for selection of bend angle.
	QSlider *sliderBendAngle;

	/// Widget that specifies lower bound for bend direction angle.
	QSpinBox *spinBoxBendDirectionStartAngle;

	/// Widget that specifies upper bound for bend direction angle.
	QSpinBox *spinBoxBendDirectionEndAngle;

	/// Widget that specifies bend direction angle.
	QSpinBox *spinBoxBendDirectionAngle;

	/// Widget that allows for selection of bend direction angle.
	QSlider *sliderBendDirection;

	/**
	 * Overridden in order to allow for resetting the UI widgets to default state
	 * before closing the widget.
	 *
	 * @param event 	The close event.
	 */
	void closeEvent(QCloseEvent *event);

public slots:
	/**
	 * This callback is triggered when the user moves the weight slider in the UI.
	 * It is responsible for bending the actively-selected mesh.
	 *
	 * @param angle	The value selected in the UI slider.
	 */
	void bendCB(int angle);

	/**
	 * This callback is triggered when the user enters a value in the bend angle spinbox.
	 *
	 * @param angle 	The value entered in the spinbox.
	 */
	void setBendAngleCB(int angle);

	/**
	 * This callback is triggered when the user moves the bend direction slider in the UI.
	 *
	 * @param angle 	The value selected in the UI slider.
	 */
	void bendDirectionCB(int angle);

	void setBendDirectionAngleCB(int angle);

	void setBendDirectionStartAngleCB(int angle);

	void setBendDirectionEndAngleCB(int angle);

	void setBendRangeStartAngleCB(int angle);

	void setBendRangeEndAngleCB(int angle);

	void resetSliders();

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
