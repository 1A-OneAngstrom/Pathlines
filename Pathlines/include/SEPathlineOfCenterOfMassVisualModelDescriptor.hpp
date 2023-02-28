/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SEPathlineOfCenterOfMassVisualModel.hpp "SEPathlineOfCenterOfMassVisualModel.hpp"
#include "SEPathlineOfCenterOfMassVisualModel.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SEPathlineOfCenterOfMassVisualModel);

	SB_CLASS_TYPE(SBCClass::VisualModel);
	SB_CLASS_PUBLIC_NAME("Pathline of the center of mass");
	SB_CLASS_DESCRIPTION("Pathline of the center of mass");
	SB_CLASS_TOOL_TIP("<b>Add a Pathline visual model</b><br/><br/>"
		"The Pathline model allows you to visualize the movement of the center of mass of a system along a Path (trajectory).<br><br>"
		"The Pathline model is created based on the current selection, or from the whole document if nothing is selected.<br/><br/>"
		"A visual model is saved as a node in the document.");
	SB_CLASS_VERSION_NUMBER("1.0.0");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SEPathlineOfCenterOfMassVisualModel);
		SB_CONSTRUCTOR_1(SEPathlineOfCenterOfMassVisualModel, const SBNodeIndexer&);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

		SB_ATTRIBUTE_READ_WRITE_RESET_RANGE(const SBQuantity::length&, SEPathlineOfCenterOfMassVisualModel, Radius, "Radius", "Geometry");
		SB_ATTRIBUTE_READ_ONLY(SBDDataGraphNodeMaterial*, SEPathlineOfCenterOfMassVisualModel, Material, "Material", "Display");

	SB_INTERFACE_END;

SB_CLASS_END(SEPathlineOfCenterOfMassVisualModel);
