/**
 * @file   mudbox_deformers_bend_deformer.h
 * @brief
 */

#ifndef MUDBOX_DEFORMERS_BEND_DEFORMER_H
#define MUDBOX_DEFORMERS_BEND_DEFORMER_H

#include <QtGui/QWidget>
#include <QtCore/QString>

#include <ssmath/ss_platform.h>

globalVar const char BEND_DEFORMER_NAME[] = "BendDeformer";

struct BendDeformer : public QWidget
{
	Q_OBJECT

public:
	static BendDeformer *existingWidget;

	static const QString objName;

	BendDeformer(QWidget *parent, Qt::WindowFlags flags);

	~BendDeformer();

	void bendCB(int weight);

};


void showBendDeformerUI();


#endif /* MUDBOX_DEFORMERS_BEND_DEFORMER_H */
