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
	int weight;

	int octavesMin;
	int octavesMax;
	int octaves;

	QSpinBox *spinBoxWeight;
	QSpinBox *spinBoxWeightMin;
	QSpinBox *spinBoxWeightMax;

	QSlider *sliderWeight;

	QSpinBox *spinBoxOctaves;
	QSpinBox *spinBoxOctavesMin;
	QSpinBox *spinBoxOctavesMax;

	QSlider *sliderOctaves;

	void applyCB();

	void resetCB();
};



/**
 * Displays the Noise deformer UI.
 */
void showNoiseDeformerUI();


#endif /* MUDBOX_DEFORMERS_NOISE_DEFORMER_H */
