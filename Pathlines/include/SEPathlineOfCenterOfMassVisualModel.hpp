#pragma once 

#include "SBMVisualModel.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "SBAtom.hpp"
#include "SBPath.hpp"


/// This class implements a visual model

class SB_EXPORT SEPathlineOfCenterOfMassVisualModel : public SBMVisualModel {

	SB_CLASS

public :

	/// \name Constructors and destructors
	//@{

	SEPathlineOfCenterOfMassVisualModel();																								///< Builds a visual model
	SEPathlineOfCenterOfMassVisualModel(const SBNodeIndexer& nodeIndexer);																///< Builds a visual model
	virtual ~SEPathlineOfCenterOfMassVisualModel();																						///< Destructs the visual model

	//@}

	/// \name Serialization
	//@{

	virtual bool												isSerializable() const override;										///< Returns true when the class is serializable

	virtual void												serialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber = SB_SDK_VERSION_NUMBER, const SBVersionNumber& classVersionNumber = SBVersionNumber(1, 0, 0)) const override;		///< Serializes the node
	virtual void												unserialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber = SB_SDK_VERSION_NUMBER, const SBVersionNumber& classVersionNumber = SBVersionNumber(1, 0, 0)) override;			///< Unserializes the node

	//@}

	/// \name Topology
	//@{

	virtual	void												onErase() override;														///< Erases the visual model

	//@}

	/// \name Rendering
	//@{

	virtual void												display(SBNode::RenderingPass renderingPass) override;					///< Displays the visual model

	virtual void												expandBounds(SBIAPosition3& bounds) const override;						///< Expands the bounds to make sure the visual model fits inside them

	virtual void												collectAmbientOcclusion(const SBPosition3& boxOrigin, const SBPosition3& boxSize, unsigned int nCellsX, unsigned int nCellsY, unsigned int nCellsZ, float* ambientOcclusionData) override;		///< To collect ambient occlusion data

	//@}

	/// \name Events
	//@{

	virtual void												onBaseEvent(SBBaseEvent* baseEvent);									///< Handles base events
	virtual void												onDocumentEvent(SBDocumentEvent* documentEvent);						///< Handles document events
	virtual void												onStructuralEvent(SBStructuralEvent* documentEvent);					///< Handles structural events

	//@}

	/// \name Radius
	//@{

	const SBQuantity::length&									getRadius() const;
	void														setRadius(const SBQuantity::length& r);

	bool														hasRadiusRange() const;
	const SBQuantity::length&									getDefaultRadius() const;
	const SBQuantity::length&									getMinimumRadius() const;
	const SBQuantity::length&									getMaximumRadius() const;
	const SBQuantity::length&									getRadiusSingleStep() const;
	std::string													getRadiusSuffix() const;

	//@}

	void														update();

private:

	SBPosition3													computePosition(const SBPath* path, const SBPointerIndexer<SBAtom>& atomIndexer, const unsigned int step);

	SBQuantity::length											radius				= SBQuantity::angstrom(0.3);						///< radius of the pathline
	SBQuantity::length											defaultRadius		= SBQuantity::angstrom(0.3);						///< default value of the pathline's radius
	SBQuantity::length											minimumRadius		= SBQuantity::angstrom(0.01);						///< minimum value of the pathline's radius
	SBQuantity::length											maximumRadius		= SBQuantity::angstrom(0.8);						///< maximum value of the pathline's radius
	SBQuantity::length											radiusSingleStep	= SBQuantity::angstrom(0.1);						///< single step fpr the pathline's radius

	float														colorRed	= 1.0f;														///< red channel of the pathline color
	float														colorGreen	= 0.0f;														///< green channel of the pathline color
	float														colorBlue	= 0.0f;														///< blue channel of the pathline color

	SBPointerIndexer<SBAtom>									atomIndexer;															///< indexer of pointers to atoms for which pathlines should be created
	SBPointerIndexer<SBPath>									pathIndexer;															///< indexer of pointers to paths for which pathlines should be created

};


SB_REGISTER_TARGET_TYPE(SEPathlineOfCenterOfMassVisualModel, "SEPathlineOfCenterOfMassVisualModel", "DC73C879-57E5-E047-2659-AAE682368B16");
SB_DECLARE_BASE_TYPE(SEPathlineOfCenterOfMassVisualModel, SBMVisualModel);
