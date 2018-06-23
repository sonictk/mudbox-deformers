#ifndef MUDBOX_DEFORMERS_NOISE_DEFORMER_H
#define MUDBOX_DEFORMERS_NOISE_DEFORMER_H

#include <QtGui/QWidget>
#include <QtCore/QString>


struct NoiseDeformer : public QWidget
{
	Q_OBJECT

public:
	NoiseDeformer(QWidget *parent, Qt::WindowFlags flags);

	~NoiseDeformer();

	static NoiseDeformer *existingWidget;

	static const QString objName;
};


/**
 * Displays the Noise deformer UI.
 */
void showNoiseDeformerUI();


#endif /* MUDBOX_DEFORMERS_NOISE_DEFORMER_H */
