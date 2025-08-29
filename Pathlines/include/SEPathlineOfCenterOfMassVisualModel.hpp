#pragma once 

#include "SBMVisualModel.hpp"

#include "SBBaseEvent.hpp"
#include "SBDocumentEvent.hpp"
#include "SBStructuralEvent.hpp"
#include "SBAtom.hpp"
#include "SBPath.hpp"
#include "SBCylinderArray.hpp"

#include "SBGRenderOpenGLFunctions.hpp"


/// This class implements a visual model

class SB_EXPORT SEPathlineOfCenterOfMassVisualModel : public SBMVisualModel {

	SB_CLASS

public:

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
	void														setRadius(const SBQuantity::length& value);

	bool														hasRadiusRange() const;
	const SBQuantity::length&									getDefaultRadius() const;
	const SBQuantity::length&									getMinimumRadius() const;
	const SBQuantity::length&									getMaximumRadius() const;
	const SBQuantity::length&									getRadiusSingleStep() const;
	std::string													getRadiusSuffix() const;

	//@}

	void														update();

	/// \name Selections
	//@{

	void														selectAtoms();															///< Selects atoms
	void														selectPaths();															///< Selects paths

	virtual void												getNodes(SBNodeIndexer& nodeIndexer, SBNode::Type nodeType, bool selectedNodesOnly = false, const SBNodePredicate& visitPredicate = SBDDataGraphNode::All(), bool includeDependencies = false) const final;					///< Collects nodes into \p nodeIndexer, based on a \p nodeType, a selection status and a \p visitPredicate, with or without dependencies
	virtual void												getNodes(SBNodeIndexer& nodeIndexer, const SBNodePredicate& selectionPredicate = SBDDataGraphNode::All(), const SBNodePredicate& visitPredicate = SBDDataGraphNode::All(), bool includeDependencies = false) const final;	///< Collects nodes into \p nodeIndexer, based on a \p selectionPredicate and a \p visitPredicate, with or without dependencies

	virtual bool												hasNode(SBNode::Type nodeType, bool selectedNodesOnly = false, const SBNodePredicate& visitPredicate = SBDDataGraphNode::All(), bool includeDependencies = false) const final;					///< Checks for nodes based on a \p nodeType, a selection status and a \p visitPredicate, with or without dependencies
	virtual bool												hasNode(const SBNodePredicate& selectionPredicate = SBDDataGraphNode::All(), const SBNodePredicate& visitPredicate = SBDDataGraphNode::All(), bool includeDependencies = false) const final;	///< Checks for nodes based on a \p selectionPredicate and a \p visitPredicate, with or without dependencies

	//@}

private:
	
	static SB_OPENGL_FUNCTIONS*									gl;

	/// \name Positions data
	//@{

	std::vector< std::pair<SBPointer<SBPath>, std::vector<SBPosition3> > > vectorOfPathsWithPositions;									///< A vector of pairs of paths with a vector of positions along the path

	void														computePositionsAlongPaths();

	static SBPosition3											computePosition(const SBPath* path, const SBPointerIndexer<SBAtom>& atomIndexer, const unsigned int step);

	//@}

	/// \name Visual model properties
	//@{

	SBQuantity::length											radius				= SBQuantity::angstrom(0.3);						///< The radius of the pathline
	SBQuantity::length											defaultRadius		= SBQuantity::angstrom(0.3);						///< The default value of the pathline's radius
	SBQuantity::length											minimumRadius		= SBQuantity::angstrom(0.01);						///< The minimum value of the pathline's radius
	SBQuantity::length											maximumRadius		= SBQuantity::angstrom(1.0);						///< The maximum value of the pathline's radius
	SBQuantity::length											radiusSingleStep	= SBQuantity::angstrom(0.1);						///< The single step for the pathline's radius

	//@}

	/// \name Display data
	//@{

	SBPointer<SBCylinderArray>									cylinderArray{ nullptr };												///< Geometry array for path-tracing

	unsigned int												numberOfCylinders{ 0 };													///< The total number of cylinders
	unsigned int												numberOfPositionsForCylinders{ 0 };										///< The total number of positions for cylinders (2 per cylinder)
	
	unsigned int*												indexData{ nullptr };													///< this should be of size 2 * cylinder, it will say for each end point at which position it should be
	float*														positionData{ nullptr };												///< (X,Y,Z) coordinates of each position, 2 positions per cylinder
	float*														radiusData{ nullptr };													///< the radius of the two end points of the cylinder (can be different to make a cone)
	float*														colorData{ nullptr };													///< the RGBA code for each end point of a cylinder
	unsigned int*												flagData{ nullptr };													///< flags
	unsigned int*												nodeIndexData{ nullptr };												///< controls if the cylinder is highlighted, selected
	unsigned int*												capData{ nullptr };														///< whether the end of a cylinder is capped (1) or not (0)
	SBNodeMaterial**											materialData{ nullptr };												///< the material (for path-tracing)
	SBNode**													nodeData{ nullptr };													///< the associated node (for path-tracing)

	float														color[4]{ 1.0f, 0.0f, 0.0f, 1.0f };										///< The pathline color

	void														populateRadiusData();
	void														populateColorData();

	//@}

	SBPointerIndexer<SBAtom>									atomIndexer;															///< The indexer of pointers to atoms for which pathlines should be created
	SBPointerIndexer<SBPath>									pathIndexer;															///< The indexer of pointers to paths for which pathlines should be created

};


SB_REGISTER_TARGET_TYPE(SEPathlineOfCenterOfMassVisualModel, "SEPathlineOfCenterOfMassVisualModel", "DC73C879-57E5-E047-2659-AAE682368B16");
SB_DECLARE_BASE_TYPE(SEPathlineOfCenterOfMassVisualModel, SBMVisualModel);
