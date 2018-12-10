#pragma once 

#include "SBMVisualModel.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "SBAtom.hpp"
#include "SBPath.hpp"


/// This class implements a visual model

class SEPathlineOfCenterOfMassVisualModel : public SBMVisualModel {

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

	virtual bool												isSerializable() const;													///< Returns true when the class is serializable

	virtual void												serialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber = SB_SDK_VERSION_NUMBER, const SBVersionNumber& classVersionNumber = SBVersionNumber(1, 0, 0)) const;		///< Serializes the node
	virtual void												unserialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber = SB_SDK_VERSION_NUMBER, const SBVersionNumber& classVersionNumber = SBVersionNumber(1, 0, 0));			///< Unserializes the node

	//@}

	/// \name Topology
	//@{

	virtual	void												eraseImplementation();													///< Erases the visual model

	//@}

	/// \name Rendering
	//@{

	virtual void												display();																///< Displays the visual model
	virtual void												displayForShadow();														///< Displays the visual model for shadow purposes
	virtual void												displayForSelection();													///< Displays the visual model for selection purposes

	virtual void												expandBounds(SBIAPosition3& bounds) const;								///< Expands the bounds to make sure the visual model fits inside them

	virtual void												collectAmbientOcclusion(const SBPosition3& boxOrigin, const SBPosition3& boxSize, unsigned int nCellsX, unsigned int nCellsY, unsigned int nCellsZ, float* ambientOcclusionData);		///< To collect ambient occlusion data

	//@}

	/// \name Events
	//@{

	virtual void												onBaseEvent(SBBaseEvent* baseEvent);									///< Handles base events
	virtual void												onDocumentEvent(SBDocumentEvent* documentEvent);						///< Handles document events
	virtual void												onStructuralEvent(SBStructuralEvent* documentEvent);					///< Handles structural events

	//@}

	/// \name Getter/setter functions
	//@{

	const SBQuantity::length&									getRadius() const;
	void														setRadius(const SBQuantity::length& r);

	const unsigned char&										getRed() const;
	const unsigned char&										getGreen() const;
	const unsigned char&										getBlue() const;
	void														setRed(const unsigned char& c);
	void														setGreen(const unsigned char& c);
	void														setBlue(const unsigned char& c);

	//@}

	void														update();

private:

	SBQuantity::length											radius;																	///< radius of the pathline
	unsigned char												colorRed;																///< red channel of the pathline color
	unsigned char												colorGreen;																///< green channel of the pathline color
	unsigned char												colorBlue;																///< blue channel of the pathline color
	SBPointerIndexer<SBAtom>									atomIndexer;															///< indexer of pointers to atoms for which pathlines should be created
	SBPointerIndexer<SBPath>									pathIndexer;															///< indexer of pointers to paths for which pathlines should be created

};


SB_REGISTER_TARGET_TYPE(SEPathlineOfCenterOfMassVisualModel, "SEPathlineOfCenterOfMassVisualModel", "DC73C879-57E5-E047-2659-AAE682368B16");
SB_DECLARE_BASE_TYPE(SEPathlineOfCenterOfMassVisualModel, SBMVisualModel);
