#include "mudbox_deformers_spherify_deformer.h"
#include <QtCore/Qt>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>

using mudbox::Kernel;
using mudbox::etValueChanged;
using mudbox::Interface;


// NOTE: (sonictk) For Mudbox RTTI system
IMPLEMENT_CLASS(SpherifyDeformer, TreeNode, "SpherifyDeformer")

const char *SpherifyDeformer::displayName = "SpherifyDeformer";


SpherifyDeformer::SpherifyDeformer() : spherifyWeight(this, "spherifyWeight")
{

}

QWidget *SpherifyDeformer::CreatePropertiesWindow(QWidget *parent)
{
	QWidget *propertiesWidget = TreeNode::CreatePropertiesWindow(parent);

	// TODO: (sonictk) Figure out why Mudbox keeps references to these widgets and
	// where, since stack alloc crashes after widget closure
	QVBoxLayout *mainLayout = new QVBoxLayout;
	QHBoxLayout *btnLayout = new QHBoxLayout;

	QLabel *weightLabel = new QLabel("Weight");
	QGroupBox *grpSettings = new QGroupBox("Spherify settings");
	QVBoxLayout *grpSettingsLayout = new QVBoxLayout;
	QHBoxLayout *weightLayout = new QHBoxLayout;
	QSlider *sliderWeight = new QSlider(Qt::Horizontal);
	sliderWeight->setRange(0, 100);

	QPushButton *applyBtn = new QPushButton("Apply");
	QPushButton *resetBtn = new QPushButton("Reset");
	QPushButton *closeBtn = new QPushButton("Close");

	QWidget *widgetHolder = new QWidget;

	propertiesWidget->setWindowTitle("Apply spherify deformer");

	weightLayout->addWidget(weightLabel);
	weightLayout->addWidget(sliderWeight);
	grpSettingsLayout->addLayout(weightLayout);
	grpSettings->setLayout(grpSettingsLayout);

	btnLayout->addWidget(applyBtn);
	btnLayout->addWidget(resetBtn);
	btnLayout->addWidget(closeBtn);

	mainLayout->addWidget(grpSettings);
	mainLayout->addLayout(btnLayout);

	// TODO: (sonictk) Figure out why without the widget holder, nothing shows up
	// TODO: (sonictk) Figure out why there's blank space at the top of the widget
	widgetHolder->setLayout(mainLayout);

	propertiesWidget->connect(closeBtn,
							  SIGNAL(released()),
							  propertiesWidget,
							  SLOT(reject()));

	propertiesWidget->layout()->addWidget(widgetHolder);

	propertiesWidget->show();

	return propertiesWidget;
}


void SpherifyDeformer::SetDisplayName(const QString &displayName)
{
	SetDisplayName(QString(displayName));
}


void SpherifyDeformer::OnNodeEvent(const Attribute &attribute, NodeEventType eventType)
{
	if (attribute == spherifyWeight && eventType == etValueChanged) {
		Kernel()->Interface()->MessageBox(Interface::msgInformation,
										  QString("Test"),
										  QString("Test"));
	}
}
