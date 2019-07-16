/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SEPathlineOfCenterOfMassVisualModel.hpp "SEPathlineOfCenterOfMassVisualModel.hpp"
#include "SEPathlineOfCenterOfMassVisualModel.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SEPathlineOfCenterOfMassVisualModel);

	SB_CLASS_TYPE(SBCClass::VisualModel);
	SB_CLASS_DESCRIPTION("Pathline of the center of mass");
	SB_CLASS_VERSION_NUMBER("0.8.0");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SEPathlineOfCenterOfMassVisualModel);
		SB_CONSTRUCTOR_1(SEPathlineOfCenterOfMassVisualModel, const SBNodeIndexer&);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

		SB_ATTRIBUTE_READ_WRITE(const SBQuantity::length&, SEPathlineOfCenterOfMassVisualModel, Radius, "Radius", "Geometry");

	SB_INTERFACE_END;

SB_CLASS_END(SEPathlineOfCenterOfMassVisualModel);
