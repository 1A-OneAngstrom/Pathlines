#include "SEPathlineOfCenterOfMassVisualModelProperties.hpp"
#include "SEPathlineOfCenterOfMassVisualModel.hpp"
#include "SAMSON.hpp"
#include "SBGWindow.hpp"

SEPathlineOfCenterOfMassVisualModelProperties::SEPathlineOfCenterOfMassVisualModelProperties() {

	visualModel = 0;
	ui.setupUi( this );
	observer = new Observer(this);

}

SEPathlineOfCenterOfMassVisualModelProperties::~SEPathlineOfCenterOfMassVisualModelProperties() {

	if (!visualModel.isValid()) return;

	visualModel->disconnectBaseSignalFromSlot(observer(), SB_SLOT(&SEPathlineOfCenterOfMassVisualModelProperties::Observer::onBaseEvent));
	visualModel->disconnectVisualSignalFromSlot(observer(), SB_SLOT(&SEPathlineOfCenterOfMassVisualModelProperties::Observer::onVisualEvent));

}

void SEPathlineOfCenterOfMassVisualModelProperties::loadSettings( SBGSettings *settings ) {

	if ( settings == 0 ) return;
	
	// SAMSON Element generator pro tip: complete this function so this property window can save its GUI state from one session to the next

}

void SEPathlineOfCenterOfMassVisualModelProperties::saveSettings( SBGSettings *settings ) {

	if ( settings == 0 ) return;

	// SAMSON Element generator pro tip: complete this function so this property window can save its GUI state from one session to the next

}

SBCContainerUUID SEPathlineOfCenterOfMassVisualModelProperties::getUUID() const { return SBCContainerUUID( "4C63CF61-22B9-F037-FC82-87B7F2A990B9" );}

QPixmap SEPathlineOfCenterOfMassVisualModelProperties::getLogo() const { 
	
	// SAMSON Element generator pro tip: this icon will be visible in the GUI title bar. 
	// Modify it to better reflect the purpose of your visual model.

	return QPixmap(QString::fromStdString(SB_ELEMENT_PATH + "/Resource/Icons/SEPathlineOfCenterOfMassVisualModelPropertiesIcon.png"));

}

QString SEPathlineOfCenterOfMassVisualModelProperties::getName() const { 

	// SAMSON Element generator pro tip: this string will be the GUI title. 
	// Modify this function to have a user-friendly description of your visual model inside SAMSON

	return "SEPathlineOfCenterOfMassVisualModel properties"; 

}

int SEPathlineOfCenterOfMassVisualModelProperties::getFormat() const {

	// SAMSON Element generator pro tip: modify these default settings to configure the window
	//
	// SBGWindow::Savable : let users save and load interface settings (implement loadSettings and saveSettings)
	// SBGWindow::Lockable : let users lock the window on top
	// SBGWindow::Resizable : let users resize the window
	// SBGWindow::Citable : let users obtain citation information (implement getCitation)

	return (SBGWindow::Savable | SBGWindow::Lockable | SBGWindow::Resizable | SBGWindow::Citable);

}

QString SEPathlineOfCenterOfMassVisualModelProperties::getCitation() const {

	// SAMSON Element generator pro tip: modify this function to add citation information

	return
		"If you use this visual model in your work, please cite: <br/>"
		"<br/>"
		"[1] <a href=\"https://www.samson-connect.net\">https://www.samson-connect.net</a><br/>";

}

bool SEPathlineOfCenterOfMassVisualModelProperties::setup() {

	SBNodeIndexer nodeIndexer;
	SB_FOR(SBNode* node, *SAMSON::getActiveDocument()->getSelectedNodes()) node->getNodes(nodeIndexer, SBNode::GetClass() == std::string("SEPathlineOfCenterOfMassVisualModel") && SBNode::GetElement() == std::string("SEPathlineOfCenterOfMass") && SBNode::GetElementUUID() == SBUUID(SB_ELEMENT_UUID));

	if (nodeIndexer.size() == 1) {

		visualModel = static_cast<SEPathlineOfCenterOfMassVisualModel*>((nodeIndexer)[0]);
		visualModel->connectBaseSignalToSlot(observer(), SB_SLOT(&SEPathlineOfCenterOfMassVisualModelProperties::Observer::onBaseEvent));
		visualModel->connectVisualSignalToSlot(observer(), SB_SLOT(&SEPathlineOfCenterOfMassVisualModelProperties::Observer::onVisualEvent));

		return true;

	}

	return false;

}

bool SEPathlineOfCenterOfMassVisualModelProperties::setup(SBNode* node) {

	if (node->getProxy()->getName() != "SEPathlineOfCenterOfMassVisualModel") return false;
	if (node->getProxy()->getElement() != "SEPathlineOfCenterOfMass") return false;
	if (node->getProxy()->getElementUUID() != SBUUID(SB_ELEMENT_UUID)) return false;

	visualModel = static_cast<SEPathlineOfCenterOfMassVisualModel*>(node);
	visualModel->connectBaseSignalToSlot(observer(), SB_SLOT(&SEPathlineOfCenterOfMassVisualModelProperties::Observer::onBaseEvent));
	visualModel->connectVisualSignalToSlot(observer(), SB_SLOT(&SEPathlineOfCenterOfMassVisualModelProperties::Observer::onVisualEvent));

	return true;

}

SEPathlineOfCenterOfMassVisualModelProperties::Observer::Observer(SEPathlineOfCenterOfMassVisualModelProperties* properties) { this->properties = properties; }
SEPathlineOfCenterOfMassVisualModelProperties::Observer::~Observer() {}

void SEPathlineOfCenterOfMassVisualModelProperties::Observer::onBaseEvent(SBBaseEvent* baseEvent) {

	if (baseEvent->getType() == SBBaseEvent::NodeEraseBegin) properties->hide();

}

void SEPathlineOfCenterOfMassVisualModelProperties::Observer::onVisualEvent(SBVisualEvent* visualEvent) {

	if (visualEvent->getType() == SBVisualEvent::VisualModelChanged) {
		
		// SAMSON Element generator pro tip: modify this function if the property window
		// needs to be updated when the VisualModelChanged event is sent
				
	}

}
