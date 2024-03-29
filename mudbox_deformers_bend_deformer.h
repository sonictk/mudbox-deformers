/**
 * @file   mudbox_deformers_bend_deformer.h
 * @brief  Bend deformer UI.
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

#include <climits>
#include <vector>


enum BendDeformerAxis
{
	BEND_DEFORMER_AXIS_X,
	BEND_DEFORMER_AXIS_Y,
	BEND_DEFORMER_AXIS_Z
};


enum BendDeformerStatus
{
	BEND_DEFORMER_STATUS_FAILURE = INT_MIN,
	BEND_DEFORMER_STATUS_MISMATCHED_SUBDIV_LEVEL,
	BEND_DEFORMER_STATUS_SUCCESS = 0
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

	/// The minimum bend radius.
	int bendRadiusMin;

	/// The maximum bend radius.
	int bendRadiusMax;

	/// The additional offset to apply to the bend radius during deformation.
	float bendRadiusOffset;

	/// Storage for the original positions of the mesh before the deformation application.
	std::vector<mudbox::Vector> origPtPositions;

	/// Storage for the original active subdivision level at the initialization
	/// time of the deformation operation.
	mudbox::SubdivisionLevel *activeSubdivLevel;

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

	/// Widget that allows for setting the min. bend radius.
	QSpinBox *spinBoxBendRadiusMin;

	/// Widget that allows for setting the max. bend radius.
	QSpinBox *spinBoxBendRadiusMax;

	/// Widget that allows for setting the bend radius.
	QSpinBox *spinBoxBendRadius;

	/// Slider that allows for setting the bend radius.
	QSlider *sliderBendRadius;

	/**
	 * Overridden in order to allow for resetting the UI widgets to default state
	 * before closing the widget.
	 *
	 * @param event 	The close event.
	 */
	void closeEvent(QCloseEvent *event);

	/**
	 * Checks if the active geometry selection is mismatched with the cache storage
	 * and updates the cache if necessary.
	 *
	 * @return	``true`` if the cache was updated as a result of this operation,
	 * 		``false`` otherwise.
	 */
	bool checkActiveGeometryAndUpdateCache();

	/**
	 * Updates the internal cache for the original point positions. Use this when
	 * you are "saving" the current state of the mesh for restore operations.
	 */
	void updateOriginalPointPositions();

	/**
	 * Resets the GUI slider widgets to their default values.
	 */
	void resetSliders();

	/**
	 * Same as ``resetSliders``, except that the GUI sliders will be reset while
	 * preserving the existing edits made to the mesh.
	 */
	void resetSlidersWithoutAffectingGeometry();

	/**
	 * Checks if there is geometry selected in the active session and brings up a
	 * GUI dialog to warn the user if there isn't.
	 *
	 * @return 	``true``if there is geometry selected, ``false`` otherwise.
	 */
	bool checkActiveGeometrySelection();

	/**
	 * Resets the geometry to its original positions using the internal cache.

	 *
	 * @return 	The status code.
	 */
	BendDeformerStatus resetGeometryPositions();

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

	/**
	 * This callback is triggered when the user adjusts the bend direction angle in the UI.
	 *
	 * @param angle 	The value selected in the UI field.
	 */
	void setBendDirectionAngleCB(int angle);

	/**
	 * This callback is triggered when the user adjusts the start bend direction angle in the UI.
	 *
	 * @param angle	The value selected in the UI field.
	 */
	void setBendDirectionStartAngleCB(int angle);

	/**
	 * This callback is triggered when the user adjusts the end bend direction angle in the UI.
	 *
	 * @param angle	The value selected in the UI field.
	 */
	void setBendDirectionEndAngleCB(int angle);

	/**
	 * This callback is triggered when the user adjusts the start bend angle in the UI.
	 *
	 * @param angle	The value selected in the UI field.
	 */
	void setBendRangeStartAngleCB(int angle);

	/**
	 * This callback is triggered when the user adjusts the end bend angle in the UI.
	 *
	 * @param angle	The value selected in the UI field.
	 */
	void setBendRangeEndAngleCB(int angle);

	/**
	 * This callback is triggered when the user moves the radius slider in the UI.
	 *
	 * @param radius	The value selected in the UI slider. Serves as the radius
	 * 				of the resulting bend operation.
	 */
	void bendRadiusCB(int radius);

	/**
	 * This callback is triggered when the user adjusts the min. bend angle in the UI.
	 *
	 * @param radius	The value selected in the UI field.
	 */
	void setBendRadiusMinCB(int radius);

	/**
	 * This callback is triggered when the user adjusts the max. bend angle in the UI.
	 *
	 * @param radius	The value selected in the UI field.
	 */
	void setBendRadiusMaxCB(int radius);

	/**
	 * This callback is triggered when the user sets the bend angle in the UI.
	 *
	 * @param radius	The value selected in the UI field.
	 */
	void setBendRadiusCB(int radius);

	/**
	 * This callback is triggered when the **Apply** button is clicked in the UI.
	 * It is responsible for applying the results of the deformation to the mesh
	 * and updating the internal cached data of the UI.
	 */
	void applyCB();

	/**
	 * This callback is triggered when the **Reset** button is clicked in the UI.
	 * It is responsible for resetting the bend operation being performed, along
	 * with resetting the UI to its default state.
	 *
	 */
	void resetCB();
};


/**
 * Displays the Bend deformer UI.
 */
void showBendDeformerUI();


#endif /* MUDBOX_DEFORMERS_BEND_DEFORMER_H */
