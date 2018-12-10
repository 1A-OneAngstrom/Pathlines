#pragma once


#include "SBGDataGraphNodeProperties.hpp" 
#include "ui_SEPathlineOfCenterOfMassVisualModelProperties.h"
#include "SBPointer.hpp"
#include "SEPathlineOfCenterOfMassVisualModel.hpp"


/// This class implements the property window of the visual model

// SAMSON Element generator pro tip: add GUI functionality in this class. The non-GUI functionality should go in the SEPathlineOfCenterOfMassVisualModel class

// SAMSON Element generator pro tip: if you do not want to have a property window associated to you visual model, do not expose it in the descriptor. 
// Precisely, remove the line SB_ELEMENT_CLASS(SEPathlineOfCenterOfMassVisualModelProperties); from the file SEPathlineOfCenterOfMassDescriptor.cpp

class SEPathlineOfCenterOfMassVisualModelProperties : public SBGDataGraphNodeProperties {

	SB_CLASS
	Q_OBJECT

public:

	/// \name Constructors and destructors
	//@{

	SEPathlineOfCenterOfMassVisualModelProperties();																					///< Constructs a widget to show the properties of the visual model
	virtual ~SEPathlineOfCenterOfMassVisualModelProperties();																			///< Destructs the widget

	//@}

	/// \name Identity
	//@{

	virtual SBCContainerUUID									getUUID() const;														///< Returns the widget UUID
	virtual QString												getName() const;														///< Returns the widget name (used as a title for the embedding window)
	virtual QPixmap												getLogo() const;														///< Returns the widget logo
	int															getFormat() const;														///< Returns the widget format
	virtual QString												getCitation() const;													///< Returns the citation information

	//@}

	///\name Settings
	//@{

	void														loadSettings(SBGSettings* settings);									///< Load GUI settings
	void														saveSettings(SBGSettings* settings);									///< Save GUI settings

	//@}

	///\name Setup
	//@{

	virtual bool												setup();																///< Initializes the properties widget
	virtual bool												setup(SBNode* node);													///< Initializes the properties widget

	//@}

	class Observer : public SBCReferenceTarget {

		Observer(SEPathlineOfCenterOfMassVisualModelProperties* properties);
		virtual ~Observer();

		friend class SEPathlineOfCenterOfMassVisualModelProperties;

		void														onBaseEvent(SBBaseEvent* baseEvent);								///< Base event management
		void														onVisualEvent(SBVisualEvent* visualEvent);							///< Visual event management

		SEPathlineOfCenterOfMassVisualModelProperties* properties;

	};

private:

	friend class SEPathlineOfCenterOfMassVisualModel;
	Ui::SEPathlineOfCenterOfMassVisualModelPropertiesClass		ui;
	SBPointer<SEPathlineOfCenterOfMassVisualModel>				visualModel;

	SBPointer<Observer>											observer;

};

SB_REGISTER_TYPE(SEPathlineOfCenterOfMassVisualModelProperties, "SEPathlineOfCenterOfMassVisualModelProperties", "0DD055D1-716F-67CA-ED3A-61885680BDD1");
SB_DECLARE_BASE_TYPE(SEPathlineOfCenterOfMassVisualModelProperties, SBGDataGraphNodeProperties);
