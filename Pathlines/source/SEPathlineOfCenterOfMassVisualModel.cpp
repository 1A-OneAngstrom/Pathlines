#include "SEPathlineOfCenterOfMassVisualModel.hpp"
#include "SAMSON.hpp"
#include "SBQuantity.hpp"
#include "SBDColorSchemeConstant.hpp"
#include "SBDDataGraphNodeMaterial.hpp"

#include <QOpenGLShaderProgram>

SB_OPENGL_FUNCTIONS* SEPathlineOfCenterOfMassVisualModel::gl = nullptr;

SEPathlineOfCenterOfMassVisualModel::SEPathlineOfCenterOfMassVisualModel() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

	if (!gl) gl = SAMSON::getOpenGLFunctions();

	cylinderArray = new SBCylinderArray(numberOfCylinders, numberOfPositionsForCylinders, indexData, positionData, radiusData, capData, colorData, materialData, nodeData, flagData, nodeIndexData, nullptr);
	getGeometryArrayIndexer().addReferenceTarget(cylinderArray());

	connectBaseSignalToSlot(this, SB_SLOT(&SEPathlineOfCenterOfMassVisualModel::onBaseEvent));

}

SEPathlineOfCenterOfMassVisualModel::SEPathlineOfCenterOfMassVisualModel(const SBNodeIndexer& nodeIndexer) : SEPathlineOfCenterOfMassVisualModel() {

	// SAMSON Element generator pro tip: implement this function if you want your visual model to be applied to a set of data graph nodes.
	// You might want to connect to various signals and handle the corresponding events. For example, if your visual model represents a sphere positioned at
	// the center of mass of a group of atoms, you might want to connect to the atoms' base signals (e.g. to update the center of mass when an atom is erased) and
	// the atoms' structural signals (e.g. to update the center of mass when an atom is moved).

	// the input nodeIndexer may contain both atoms and paths

	SBNodeIndexer temporaryPathIndexer;
	SBNodeIndexer temporaryAtomIndexer;

	SB_FOR(SBNode* node, nodeIndexer) {

		node->getNodes(temporaryPathIndexer, SBNode::Path);							// get Path nodes from the nodeIndexer and add them to the temporary indexer
		node->getNodes(temporaryAtomIndexer, SBNode::Atom);							// get Atom nodes from the nodeIndexer and add them to the temporary indexer

	}

	if (temporaryPathIndexer.size() == 0)											// if no paths selected
		SAMSON::getActiveDocument()->getNodes(temporaryPathIndexer, SBNode::Path);	// get all paths from the active document

	if (temporaryAtomIndexer.size() == 0)											// if no atoms selected
		SAMSON::getActiveDocument()->getNodes(temporaryAtomIndexer, SBNode::Atom);	// get all atoms from the active document

	SB_FOR(SBNode* node, temporaryPathIndexer)
		pathIndexer.addReferenceTarget(node);										// add to the internal indexer references to the chosen paths

	SB_FOR(SBNode* node, temporaryAtomIndexer)
		atomIndexer.addReferenceTarget(node);										// add to the internal indexer references to the chosen atoms

	QString pathsStr(" path");
	if (pathIndexer.size() != 1) pathsStr += "s";
	SAMSON::setStatusMessage(QString("Visual model: added ") + QString::number(pathIndexer.size()) + pathsStr + QString(" of the center of mass"), 0);

}

SEPathlineOfCenterOfMassVisualModel::~SEPathlineOfCenterOfMassVisualModel() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

	disconnectBaseSignalFromSlot(this, SB_SLOT(&SEPathlineOfCenterOfMassVisualModel::onBaseEvent));

	// Note: data arrays are deleted via the geometry array (cylinderArray)

}

bool SEPathlineOfCenterOfMassVisualModel::isSerializable() const {

	// SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc.
	// Please refer to the SDK documentation for more information.
	// Modify the line below to "return true;" if you want this visual model be serializable (hence copyable, savable, etc.)

	return true;
	
}

void SEPathlineOfCenterOfMassVisualModel::serialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber, const SBVersionNumber& classVersionNumber) const {

	// Serialization of the parent class

	SBMVisualModel::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

	// SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc.
	// Please refer to the SDK documentation for more information.

	// Write parameters

	serializer->writeDoubleElement("radius", static_cast<double>(radius.getValue()));

	// In case of serializing for an old version of the Element for SAMSON SDK version number below 0.8.0, write additional parameters.
	// See the Serialization section in Developers guide for more information.

	if (sdkVersionNumber < SBVersionNumber(0, 8, 0)) {

		serializer->writeUnsignedCharElement("colorRed",   static_cast<unsigned char>(255 * color[0]));
		serializer->writeUnsignedCharElement("colorGreen", static_cast<unsigned char>(255 * color[1]));
		serializer->writeUnsignedCharElement("colorBlue",  static_cast<unsigned char>(255 * color[2]));

	}

	// Serialize referenced atoms

	unsigned int nodeIndex = 0; // the index of the node in the indexer

	// Write the number of atoms to which this visual model is applied
	serializer->writeUnsignedIntElement("numberOfAtoms", atomIndexer.size());

	// Write indices of the atoms to which this visual model is applied

	SB_FOR(SBPointer<SBAtom> atom, atomIndexer) {

		if (nodeIndexer.getIndex(atom(), nodeIndex)) {

			// the atom is indexed

			serializer->writeBoolElement("atomIsIndexed", true);
			serializer->writeUnsignedIntElement("atomIndex", nodeIndex);

		}
		else {

			// the atom is not indexed, the user must be copying just the visual model
			// so we serialize the atom's address itself

			serializer->writeBoolElement("atomIsIndexed", false);
			serializer->writeUnsignedLongLongElement("atomIndex", (unsigned long long)atom());

		}

	}

	// Serialize referenced paths

	// Write the number of paths to which this visual model is applied
	serializer->writeUnsignedIntElement("numberOfPaths", pathIndexer.size());

	// Write indices of the paths to which this visual model is applied

	SB_FOR(SBPointer<SBPath> path, pathIndexer) {

		if (nodeIndexer.getIndex(path(), nodeIndex)) {

			// the path is indexed

			serializer->writeBoolElement("pathIsIndexed", true);
			serializer->writeUnsignedIntElement("pathIndex", nodeIndex);

		}
		else {

			// the path is not indexed, the user must be copying just the visual model
			// so we serialize the path's address itself

			serializer->writeBoolElement("pathIsIndexed", false);
			serializer->writeUnsignedLongLongElement("pathIndex", (unsigned long long)path());

		}

	}

}

void SEPathlineOfCenterOfMassVisualModel::unserialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber, const SBVersionNumber& classVersionNumber) {

	// Unserialization of the parent class

	SBMVisualModel::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

	// SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc.
	// Please refer to the SDK documentation for more information.

	// Read parameters

	radius = SBQuantity::length(serializer->readDoubleElement());

	// In case of unserializing from a file saved with an old version of the Element for SAMSON SDK version number below 0.8.0, read additional serialized parameters.
	// See the Serialization section in Developers guide for more information.

	if (sdkVersionNumber < SBVersionNumber(0, 8, 0)) {

		color[0] = static_cast<float>(serializer->readUnsignedCharElement()) / 255.0f;
		color[1] = static_cast<float>(serializer->readUnsignedCharElement()) / 255.0f;
		color[2] = static_cast<float>(serializer->readUnsignedCharElement()) / 255.0f;

	}

	// Unserialize referenced atoms

	bool isNodeIndexed;
	unsigned int nodeIndex = 0; // the index of the node in the indexer

	// Read the number of atoms to which this visual model is applied
	const unsigned int numberOfAtoms = serializer->readUnsignedIntElement();

	// Read indices of the atoms to which this visual model is applied and
	// add these node into the atom indexer of the visual model

	for (unsigned int i = 0; i < numberOfAtoms; ++i) {

		isNodeIndexed = serializer->readBoolElement();
		if (isNodeIndexed) {

			// the atom was serialized too

			nodeIndex = serializer->readUnsignedIntElement();
			atomIndexer.addReferenceTarget(nodeIndexer[nodeIndex]);

		}
		else {

			// the atom was not serialized, it must still exist in memory

			atomIndexer.addReferenceTarget((SBAtom*)serializer->readUnsignedLongLongElement());

		}

	}

	// Unserialize referenced paths

	// Read the number of paths to which this visual model is applied
	const unsigned int numberOfPaths = serializer->readUnsignedIntElement();

	// Read indices of the paths to which this visual model is applied and
	// add these node into the path indexer of the visual model

	for (unsigned int i = 0; i < numberOfPaths; ++i) {

		isNodeIndexed = serializer->readBoolElement();
		if (isNodeIndexed) {

			// the path was serialized too

			nodeIndex = serializer->readUnsignedIntElement();
			pathIndexer.addReferenceTarget(nodeIndexer[nodeIndex]);

		}
		else {

			// the path was not serialized, it must still exist in memory

			pathIndexer.addReferenceTarget((SBPath*)serializer->readUnsignedLongLongElement());

		}

	}

}

void SEPathlineOfCenterOfMassVisualModel::onErase() {

	// SAMSON Element generator pro tip: modify this function when you need to perform special tasks when your visual model is erased (e.g. disconnect from nodes you are connected to).
	// Important: this function must be undoable (i.e. only call undoable functions or add an undo command to the undo stack)

}

SB_HOLD_SET_IMPLEMENTATION(SEPathlineOfCenterOfMassVisualModel, SBQuantity::length, Radius, "radius");

const SBDQuantity::length&	SEPathlineOfCenterOfMassVisualModel::getRadius() const { return radius; }
void						SEPathlineOfCenterOfMassVisualModel::setRadius(const SBQuantity::length& value) {

	SBQuantity::length newValue = value;
	if (hasRadiusRange()) {

		if (newValue < getMinimumRadius()) newValue = getMinimumRadius();
		else if (newValue > getMaximumRadius()) newValue = getMaximumRadius();

	}

	if (this->radius != newValue) {

		SB_HOLD_SET(SEPathlineOfCenterOfMassVisualModel, Radius, getRadius(), newValue, this);

		this->radius = newValue;
		populateRadiusData();
		update();

	}

}
bool						SEPathlineOfCenterOfMassVisualModel::hasRadiusRange() const { return true; }
const SBQuantity::length&	SEPathlineOfCenterOfMassVisualModel::getDefaultRadius() const { return defaultRadius; }
const SBQuantity::length&	SEPathlineOfCenterOfMassVisualModel::getMinimumRadius() const { return minimumRadius; }
const SBQuantity::length&	SEPathlineOfCenterOfMassVisualModel::getMaximumRadius() const { return maximumRadius; }
const SBQuantity::length&	SEPathlineOfCenterOfMassVisualModel::getRadiusSingleStep() const { return radiusSingleStep; }
std::string					SEPathlineOfCenterOfMassVisualModel::getRadiusSuffix() const {
	
	// the suffix for SBQuantity types are determined automatically
	return std::string("");

}

void SEPathlineOfCenterOfMassVisualModel::update() {

	SAMSON::requestViewportUpdate();

}

void SEPathlineOfCenterOfMassVisualModel::selectAtoms() {

	if (atomIndexer.size()) {

		SAMSON::beginHolding("Select atoms");

		SAMSON::getActiveDocument()->clearSelection();

		SB_FOR(SBNode * atom, atomIndexer) atom->setSelectionFlag(true);

		SAMSON::endHolding();

	}

}

void SEPathlineOfCenterOfMassVisualModel::selectPaths() {

	if (pathIndexer.size()) {

		SAMSON::beginHolding("Select paths");

		SAMSON::getActiveDocument()->clearSelection();

		SB_FOR(SBNode * path, pathIndexer) path->setSelectionFlag(true);

		SAMSON::endHolding();

	}

}

void SEPathlineOfCenterOfMassVisualModel::getNodes(SBNodeIndexer& nodeIndexer, SBNode::Type nodeType, bool selectedNodesOnly, const SBNodePredicate& visitPredicate, bool includeDependencies) const {

	if (!visitPredicate(this)) return;

	if ((getType() == nodeType) && (!selectedNodesOnly || isSelected())) nodeIndexer.push_back(const_cast<SEPathlineOfCenterOfMassVisualModel*>(this));

	if (includeDependencies) {

		SB_FOR(SBNode * node, atomIndexer) node->getNodes(nodeIndexer, nodeType, selectedNodesOnly, visitPredicate, includeDependencies);
		SB_FOR(SBNode * node, pathIndexer) node->getNodes(nodeIndexer, nodeType, selectedNodesOnly, visitPredicate, includeDependencies);

	}

}

void SEPathlineOfCenterOfMassVisualModel::getNodes(SBNodeIndexer& nodeIndexer, const SBNodePredicate& selectionPredicate, const SBNodePredicate& visitPredicate, bool includeDependencies) const {

	if (!visitPredicate(this)) return;

	if (selectionPredicate(this)) nodeIndexer.push_back(const_cast<SEPathlineOfCenterOfMassVisualModel*>(this));

	if (includeDependencies) {

		SB_FOR(SBNode * node, atomIndexer) node->getNodes(nodeIndexer, selectionPredicate, visitPredicate, includeDependencies);
		SB_FOR(SBNode * node, pathIndexer) node->getNodes(nodeIndexer, selectionPredicate, visitPredicate, includeDependencies);

	}

}

bool SEPathlineOfCenterOfMassVisualModel::hasNode(SBNode::Type nodeType, bool selectedNodesOnly, const SBNodePredicate& visitPredicate, bool includeDependencies) const {

	if (!visitPredicate(this)) return false;

	if ((getType() == nodeType) && (!selectedNodesOnly || isSelected())) return true;

	if (includeDependencies) {

		SB_FOR(SBNode * node, atomIndexer) if (node->hasNode(nodeType, selectedNodesOnly, visitPredicate, includeDependencies)) return true;
		SB_FOR(SBNode * node, pathIndexer) if (node->hasNode(nodeType, selectedNodesOnly, visitPredicate, includeDependencies)) return true;

	}

	return false;

}

bool SEPathlineOfCenterOfMassVisualModel::hasNode(const SBNodePredicate& selectionPredicate, const SBNodePredicate& visitPredicate, bool includeDependencies) const {

	if (!visitPredicate(this)) return false;

	if (selectionPredicate(this)) return true;

	if (includeDependencies) {

		SB_FOR(SBNode * node, atomIndexer) if (node->hasNode(selectionPredicate, visitPredicate, includeDependencies)) return true;
		SB_FOR(SBNode * node, pathIndexer) if (node->hasNode(selectionPredicate, visitPredicate, includeDependencies)) return true;

	}

	return false;

}

void SEPathlineOfCenterOfMassVisualModel::display(SBNode::RenderingPass renderingPass) {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. This is the main function of your visual model. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

	if (renderingPass != SBNode::RenderingPass::OpaqueGeometry &&
		renderingPass != SBNode::RenderingPass::TransparentGeometry &&
		renderingPass != SBNode::RenderingPass::ShadowingGeometry &&
		renderingPass != SBNode::RenderingPass::SelectableGeometry)
		return;

	if (pathIndexer.size() == 0) return;
	if (atomIndexer.size() == 0) return;

	// compute positions along paths, if necessary

	if (vectorOfPathsWithPositions.size() == 0)
		computePositionsAlongPaths();

	unsigned int newNumberOfCylinders = 0;
	for (const auto& it : std::as_const(vectorOfPathsWithPositions)) {

		SBPath* path = it.first();
		newNumberOfCylinders += path->getNumberOfSteps() - 1;	// the number of cylinders between steps

	}

	if (numberOfCylinders != newNumberOfCylinders) {

		numberOfCylinders = newNumberOfCylinders;
		numberOfPositionsForCylinders = 2 * newNumberOfCylinders;

		// re-allocate data arrays

		delete[] indexData;
		delete[] positionData;
		delete[] radiusData;
		delete[] colorData;
		delete[] flagData;
		delete[] nodeIndexData;
		delete[] capData;
		delete[] materialData;
		delete[] nodeData;

		indexData = new unsigned int[numberOfPositionsForCylinders]();
		positionData = new float[3 * numberOfPositionsForCylinders]();
		radiusData = new float[numberOfPositionsForCylinders]();
		colorData = new float[4 * numberOfPositionsForCylinders]();
		flagData = new unsigned int[numberOfPositionsForCylinders]();
		nodeIndexData = new unsigned int[numberOfPositionsForCylinders]();
		capData = new unsigned int[numberOfPositionsForCylinders]();
		materialData = new SBNodeMaterial*[numberOfPositionsForCylinders];
		nodeData = new SBNode*[numberOfPositionsForCylinders];

		if (cylinderArray.isValid()) {

			cylinderArray->setNumberOfGeometries(numberOfCylinders);
			cylinderArray->setNumberOfPositions(numberOfPositionsForCylinders);
			cylinderArray->setPositionData(positionData);
			cylinderArray->setIndexData(indexData);
			cylinderArray->setColorData(colorData);
			cylinderArray->setMaterialData(materialData);
			cylinderArray->setNodeData(nodeData);
			cylinderArray->setRadiusData(radiusData);
			cylinderArray->setCapData(capData);
			cylinderArray->setFlagData(flagData);
			cylinderArray->setNodeIndexData(nodeIndexData);

		}

		// populate data arrays

		std::iota(indexData, indexData + numberOfPositionsForCylinders, 0);
		std::fill_n(capData, numberOfPositionsForCylinders, 0);							// 1 - close the caps; 0 - don't close the caps
		
		populateRadiusData();
		populateColorData();

		unsigned int shift = 0;

		for (const auto& it : std::as_const(vectorOfPathsWithPositions)) {

			SBPath* path = it.first();
			const std::vector<SBPosition3>& positions = it.second;
			if (!path || positions.size() == 0) continue;

			const unsigned int numberOfSteps = path->getNumberOfSteps();

			std::fill_n(nodeData + 2 * shift, 2 * (numberOfSteps - 1), path);

			for (unsigned int step = 0; step < numberOfSteps - 1; step++) {

				const unsigned int i6 = 6 * (step + shift);

				positionData[i6 + 0] = static_cast<float>(positions[step].v[0].getValue());
				positionData[i6 + 1] = static_cast<float>(positions[step].v[1].getValue());
				positionData[i6 + 2] = static_cast<float>(positions[step].v[2].getValue());
				positionData[i6 + 3] = static_cast<float>(positions[step + 1].v[0].getValue());
				positionData[i6 + 4] = static_cast<float>(positions[step + 1].v[1].getValue());
				positionData[i6 + 5] = static_cast<float>(positions[step + 1].v[2].getValue());

			}

			shift += numberOfSteps - 1;

		}

	}

	// populate data arrays

	unsigned int shift = 0;

	for (const auto& it : std::as_const(vectorOfPathsWithPositions)) {

		SBPath* path = it.first();
		const std::vector<SBPosition3>& positions = it.second;
		if (!path || positions.size() == 0) continue;

		const unsigned int numberOfSteps = path->getNumberOfSteps();

		std::fill_n(flagData + 2 * shift, 2 * (numberOfSteps - 1), path->getInheritedFlags() | getInheritedFlags());
		std::fill_n(nodeIndexData + 2 * shift, 2 * (numberOfSteps - 1), path->getNodeIndex());

		shift += numberOfSteps - 1;

	}

	const float inheritedOpacity = getInheritedOpacity();

	if ((renderingPass == SBNode::RenderingPass::OpaqueGeometry) && (inheritedOpacity == 1.0f)) {

		// display opaque geometry

		SAMSON::displayLineSweptSpheres(numberOfCylinders, numberOfPositionsForCylinders, indexData, positionData, radiusData,
			colorData, flagData, false);

	}
	else if ((renderingPass == SBNode::RenderingPass::TransparentGeometry) && (inheritedOpacity != 1.0f)) {

		// display transparent geometry

		if (gl) {

			gl->glColorMask(false, false, false, false);

			SAMSON::displayLineSweptSpheres(numberOfCylinders, numberOfPositionsForCylinders, indexData, positionData, radiusData,
				colorData, flagData, false, SBSpatialTransform::identity, inheritedOpacity);

			gl->glColorMask(true, true, true, true);

			SAMSON::displayLineSweptSpheres(numberOfCylinders, numberOfPositionsForCylinders, indexData, positionData, radiusData,
				colorData, flagData, false, SBSpatialTransform::identity, inheritedOpacity);

		}

	}
	else if (renderingPass == SBNode::RenderingPass::ShadowingGeometry) {

		// display for shadows

		SAMSON::displayLineSweptSpheres(numberOfCylinders, numberOfPositionsForCylinders, indexData, positionData, radiusData,
			colorData, flagData, true);

	}
	else if (renderingPass == SBNode::RenderingPass::SelectableGeometry) {

		// display for selection

		SAMSON::displayLineSweptSpheresSelection(numberOfCylinders, numberOfPositionsForCylinders, indexData, positionData, radiusData, nodeIndexData);

	}

}

void SEPathlineOfCenterOfMassVisualModel::populateRadiusData() {

	if (radiusData)
		std::fill_n(radiusData, numberOfPositionsForCylinders, static_cast<float>(radius.getValue()));

}

void SEPathlineOfCenterOfMassVisualModel::populateColorData() {

	// set to the default color

	color[0] = 1.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 1.0f;

	// get color from the material if it is applied

	SBNodeMaterial* material = getMaterial();
	SBNodeColorScheme* colorScheme = nullptr;

	if (material && material->getColorScheme()) {

		colorScheme = material->getColorScheme();
		colorScheme->getColor(color);
		color[3] = 1.0f;	// set the alpha channel to 1.0

	}

	// populate data arrays

	if (materialData)
		std::fill_n(materialData, numberOfPositionsForCylinders, material);

	if (!colorData) return;

	unsigned int shift = 0;
	unsigned int counter = 0;

	for (const auto& it : std::as_const(vectorOfPathsWithPositions)) {

		SBPath* path = it.first();
		const std::vector<SBPosition3>& positions = it.second;
		if (!path || positions.size() == 0) continue;

		// if a color scheme with a color palette has been applied to the visual model, then colorize each path in a different color along the palette
		if (colorScheme && colorScheme->hasPalette()) {

			colorScheme->getPalette()->getColor(color, static_cast<float>(counter + 0.5f) / vectorOfPathsWithPositions.size());
			color[3] = 1.0f;	// set the alpha channel to 1.0

		}

		const unsigned int numberOfSteps = path->getNumberOfSteps();

		for (unsigned int step = 0; step < numberOfSteps - 1; step++) {

			const unsigned int i8 = 8 * (step + shift);

			colorData[i8 + 0] = color[0];
			colorData[i8 + 1] = color[1];
			colorData[i8 + 2] = color[2];
			colorData[i8 + 3] = color[3];
			colorData[i8 + 4] = color[0];
			colorData[i8 + 5] = color[1];
			colorData[i8 + 6] = color[2];
			colorData[i8 + 7] = color[3];

		}

		shift += numberOfSteps - 1;
		++counter;

	}

}

void SEPathlineOfCenterOfMassVisualModel::computePositionsAlongPaths() {

	vectorOfPathsWithPositions.clear();

	SB_FOR(SBPath * path, pathIndexer) {

		if (!path) continue;
		if (path->isErased()) continue;

		const unsigned int numberOfSteps = path->getNumberOfSteps();
		if (numberOfSteps == 0) continue;

		const SBPointerIndexer<SBAtom>* pathAtomIndexer = path->getAtomIndexer();
		if (pathAtomIndexer->size() == 0) continue;

		// create an indexer with atoms which are present both in the path and in the user-chosen atomIndexer

		SBPointerIndexer<SBAtom> temporaryAtomIndexer;
		SB_FOR(SBAtom * node, atomIndexer) {

			if (node->isErased()) continue;
			if (pathAtomIndexer->hasIndex(node))
				temporaryAtomIndexer.addReferenceTarget(node);

		}
		if (temporaryAtomIndexer.size() == 0) continue;

		// compute a vector of positions along the path

		std::vector<SBPosition3> positions;

		for (unsigned int step = 0; step < numberOfSteps; step++)
			positions.push_back(computePosition(path, temporaryAtomIndexer, step));

		// add the path and its positions

		vectorOfPathsWithPositions.push_back({ path, std::move(positions) });

	}

}

SBPosition3 SEPathlineOfCenterOfMassVisualModel::computePosition(const SBPath* path, const SBPointerIndexer<SBAtom>& atomIndexer, const unsigned int step) {

	SBPosition3 avgPosition(SBQuantity::position(0.0));

	if (path) {

		for (unsigned int ia = 0; ia < atomIndexer.size(); ia++) {

			SBPosition3 position;
			path->getPosition(step, atomIndexer[ia], position);
			avgPosition += position;

		}

		avgPosition /= static_cast<float>(atomIndexer.size());

	}

	return avgPosition;

}

void SEPathlineOfCenterOfMassVisualModel::expandBounds(SBIAPosition3& bounds) const {

	// SAMSON Element generator pro tip: this function is called by SAMSON to determine the model's spatial bounds. 
	// When this function returns, the bounds interval vector should contain the visual model. 

}

void SEPathlineOfCenterOfMassVisualModel::collectAmbientOcclusion(const SBPosition3& boxOrigin, const SBPosition3& boxSize, unsigned int nCellsX, unsigned int nCellsY, unsigned int nCellsZ, float* ambientOcclusionData) {

	// SAMSON Element generator pro tip: this function is called by SAMSON to determine your model's influence on ambient occlusion.
	// Implement this function if you want your visual model to occlude other objects in ambient occlusion calculations.
	//
	// The ambientOcclusionData represents a nCellsX x nCellsY x nCellsZ grid of occlusion densities over the spatial region (boxOrigin, boxSize).
	// If your model represents geometry at position (x, y, z), then the occlusion density in corresponding grid nodes should be increased.
	//
	// Assuming x, y and z are given in length units (SBQuantity::length, SBQuantity::angstrom, etc.), the grid coordinates are:
	// SBQuantity::dimensionless xGrid = nCellsX * (x - boxOrigin.v[0]) / boxSize.v[0];
	// SBQuantity::dimensionless yGrid = nCellsY * (x - boxOrigin.v[1]) / boxSize.v[1];
	// SBQuantity::dimensionless zGrid = nCellsZ * (x - boxOrigin.v[2]) / boxSize.v[2];
	//
	// The corresponding density can be found at ambientOcclusionData[((int)zGrid.getValue() + 0)*nCellsY*nCellsX + ((int)yGrid.getValue() + 0)*nCellsX + ((int)xGrid.getValue() + 0)] (beware of grid bounds).
	// For higher-quality results, the influence of a point can be spread over neighboring grid nodes.

}

void SEPathlineOfCenterOfMassVisualModel::onBaseEvent(SBBaseEvent* baseEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle base events (e.g. when a node for which you provide a visual representation emits a base signal, such as when it is erased)

	switch (baseEvent->getType()) {

	case SBBaseEvent::MaterialAdded:
	case SBBaseEvent::MaterialChanged:
	case SBBaseEvent::MaterialRemoved:

		populateColorData();
		update();
		break;

	case SBBaseEvent::IndexChanged:

		update();
		break;

	default:

		break;

	}

}

void SEPathlineOfCenterOfMassVisualModel::onDocumentEvent(SBDocumentEvent* documentEvent) {

	// SAMSON Element generator pro tip: implement this function if you need to handle document events 

}

void SEPathlineOfCenterOfMassVisualModel::onStructuralEvent(SBStructuralEvent* documentEvent) {
	
	// SAMSON Element generator pro tip: implement this function if you need to handle structural events (e.g. when a structural node for which you provide a visual representation is updated)

}
