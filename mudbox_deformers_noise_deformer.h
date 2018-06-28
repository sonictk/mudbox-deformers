/**
 * @file   mudbox_deformers_noise_deformer.h
 * @brief  Noise Deformer UI.
 */

#ifndef MUDBOX_DEFORMERS_NOISE_DEFORMER_H
#define MUDBOX_DEFORMERS_NOISE_DEFORMER_H

#include <vector>

#include <QtCore/QString>
#include <QtGui/QCloseEvent>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>
#include <QtGui/QSlider>

#include <Mudbox/mudbox.h>


enum NoiseDeformerStatus
{
	NOISE_DEFORMER_STATUS_FAILURE = INT_MIN,
	NOISE_DEFORMER_STATUS_MISMATCHED_SUBDIV_LEVEL,
	NOISE_DEFORMER_STATUS_NO_ACTIVE_SUBDIV_LEVEL,
	NOISE_DEFORMER_STATUS_NO_GEOMETRY_SELECTED,
	NOISE_DEFORMER_STATUS_SUCCESS = 0
};



struct NoiseDeformer : public QWidget
{
	Q_OBJECT

public:
	NoiseDeformer(QWidget *parent, Qt::WindowFlags flags);

	~NoiseDeformer();

	static NoiseDeformer *existingWidget;

	static const QString objName;

	int weightMin;
	int weightMax;
	int defaultWeight;

	int frequencyMin;
	int frequencyMax;
	int defaultFrequency;

	mudbox::SubdivisionLevel *activeSubdivLevel;

	std::vector<mudbox::Vector> origPtPositions;

	QSpinBox *spinBoxWeight;
	QSpinBox *spinBoxWeightMin;
	QSpinBox *spinBoxWeightMax;

	QSlider *sliderWeight;

	QSpinBox *spinBoxFrequency;
	QSpinBox *spinBoxFrequencyMin;
	QSpinBox *spinBoxFrequencyMax;

	QSlider *sliderFrequency;

	void resetSliders();

	NoiseDeformerStatus resetGeometryPositions();

	void updateOriginalPointPositions();

	void checkActiveGeometrySelection();

	void resetSlidersWithoutAffectingGeometry();

	bool checkActiveGeometryAndUpdateCache();

	void closeEvent(QCloseEvent *event);

	void deformPoint(unsigned int index, float weight, int frequency);

	NoiseDeformerStatus deform(float weight, int frequency);

	NoiseDeformerStatus resetMesh();

public slots:
	void applyCB();

	void applyChangesToLayerCB();

	void resetCB();

	void weightChangedCB(int weight);

	void setWeightCB(int weight);

	void setMinWeightCB(int weight);

	void setMaxWeightCB(int weight);

	void frequencyChangedCB(int frequency);

	void setFrequencyCB(int frequency);

	void setMinFrequencyCB(int frequency);

	void setMaxFrequencyCB(int frequency);
};


/**
 * Displays the Noise deformer UI.
 */
void showNoiseDeformerUI();


#endif /* MUDBOX_DEFORMERS_NOISE_DEFORMER_H */
