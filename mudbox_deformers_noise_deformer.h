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

	int octavesMin;
	int octavesMax;
	int defaultOctaves;

	mudbox::SubdivisionLevel *activeSubdivLevel;

	std::vector<mudbox::Vector> origPtPositions;

	QSpinBox *spinBoxWeight;
	QSpinBox *spinBoxWeightMin;
	QSpinBox *spinBoxWeightMax;

	QSlider *sliderWeight;

	QSpinBox *spinBoxOctaves;
	QSpinBox *spinBoxOctavesMin;
	QSpinBox *spinBoxOctavesMax;

	QSlider *sliderOctaves;

	void resetSliders();

	NoiseDeformerStatus resetGeometryPositions();

	void updateOriginalPointPositions();

	void checkActiveGeometrySelection();

	void resetSlidersWithoutAffectingGeometry();

	bool checkActiveGeometryAndUpdateCache();

	void closeEvent(QCloseEvent *event);

	NoiseDeformerStatus deform(float weight, int octaves);

public slots:
	void applyCB();

	void resetCB();

	void weightChangedCB(int weight);

	void setWeightCB(int weight);

	void setMinWeightCB(int weight);

	void setMaxWeightCB(int weight);

	void octavesChangedCB(int octaves);

	void setOctavesCB(int octaves);

	void setMinOctavesCB(int octaves);

	void setMaxOctavesCB(int octaves);
};


/**
 * Displays the Noise deformer UI.
 */
void showNoiseDeformerUI();


#endif /* MUDBOX_DEFORMERS_NOISE_DEFORMER_H */
