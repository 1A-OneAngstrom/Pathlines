/// \headerfile SEPathlineOfCenterOfMassVisualModelDescriptor.hpp "SEPathlineOfCenterOfMassVisualModelDescriptor.hpp"
#include "SEPathlineOfCenterOfMassVisualModelDescriptor.hpp"

/// \headerfile SEPathlineOfCenterOfMassVisualModelPropertiesDescriptor.hpp "SEPathlineOfCenterOfMassVisualModelPropertiesDescriptor.hpp"
#include "SEPathlineOfCenterOfMassVisualModelPropertiesDescriptor.hpp"



// Describe the SAMSON Element

SB_ELEMENT_DESCRIPTION("Visual model for a pathline of the center of mass of selected nodes based on a Path");
SB_ELEMENT_DOCUMENTATION("Resource/Documentation/doc.html");
SB_ELEMENT_VERSION_NUMBER("0.7.0");

SB_ELEMENT_CLASSES_BEGIN;

	SB_ELEMENT_CLASS(SEPathlineOfCenterOfMassVisualModel);
	//SB_ELEMENT_CLASS(SEPathlineOfCenterOfMassVisualModelProperties); // this visual model has no properties

SB_ELEMENT_CLASSES_END;

SB_ELEMENT_CATEGORIES_BEGIN;

	SB_ELEMENT_CATEGORY(SBClass::Category::Visualization);

SB_ELEMENT_CATEGORIES_END;
