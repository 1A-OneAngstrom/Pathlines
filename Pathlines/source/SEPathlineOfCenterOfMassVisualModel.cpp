#include "SEPathlineOfCenterOfMassVisualModel.hpp"
#include "SAMSON.hpp"
#include "SBQuantity.hpp"
#include "SBDColorSchemeConstant.hpp"
#include "SBDDataGraphNodeMaterial.hpp"

//#include <QOpenGLShaderProgram>

SEPathlineOfCenterOfMassVisualModel::SEPathlineOfCenterOfMassVisualModel() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

}

SEPathlineOfCenterOfMassVisualModel::SEPathlineOfCenterOfMassVisualModel(const SBNodeIndexer& nodeIndexer) {

	// SAMSON Element generator pro tip: implement this function if you want your visual model to be applied to a set of data graph nodes.
	// You might want to connect to various signals and handle the corresponding events. For example, if your visual model represents a sphere positioned at
	// the center of mass of a group of atoms, you might want to connect to the atoms' base signals (e.g. to update the center of mass when an atom is erased) and
	// the atoms' structural signals (e.g. to update the center of mass when an atom is moved).

	// nodeIndexer may contain both atoms and paths

	SBNodeIndexer temporaryPathIndexer;
	SBNodeIndexer temporaryAtomIndexer;

	SB_FOR(SBNode* node, nodeIndexer) {

		node->getNodes(temporaryPathIndexer, SBNode::Path);							// get Path nodes from the nodeIndexer and add them to the temporary indexer
		node->getNodes(temporaryAtomIndexer, SBNode::Atom);							// get Atom nodes from the nodeIndexer and add them to the temporary indexer

	}

	if (temporaryPathIndexer.size() == 0)											// if no Paths selected
		SAMSON::getActiveDocument()->getNodes(temporaryPathIndexer, SBNode::Path);	// get all Paths from the active Document

	if (temporaryAtomIndexer.size() == 0)											// if no Atoms selected
		SAMSON::getActiveDocument()->getNodes(temporaryAtomIndexer, SBNode::Atom);	// get all Atoms from the active Document

	SB_FOR(SBNode* node, temporaryPathIndexer)
		pathIndexer.addReferenceTarget(node);										// add to the internal indexer references to the chosen Paths

	SB_FOR(SBNode* node, temporaryAtomIndexer)
		atomIndexer.addReferenceTarget(node);

	temporaryPathIndexer.clear();
	temporaryAtomIndexer.clear();

	connectBaseSignalToSlot(this, SB_SLOT(&SEPathlineOfCenterOfMassVisualModel::onBaseEvent));

	QString pathsStr(" path");
	if (pathIndexer.size() != 1) pathsStr += "s";
	SAMSON::setStatusMessage(QString("Visual model: added ") + QString::number(pathIndexer.size()) + pathsStr + QString(" of the center of mass"), 0);

}

SEPathlineOfCenterOfMassVisualModel::~SEPathlineOfCenterOfMassVisualModel() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

	disconnectBaseSignalFromSlot(this, SB_SLOT(&SEPathlineOfCenterOfMassVisualModel::onBaseEvent));

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

		serializer->writeUnsignedCharElement("colorRed",   static_cast<unsigned char>(255 * colorRed));
		serializer->writeUnsignedCharElement("colorGreen", static_cast<unsigned char>(255 * colorGreen));
		serializer->writeUnsignedCharElement("colorBlue",  static_cast<unsigned char>(255 * colorBlue));

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

		colorRed   = static_cast<float>(serializer->readUnsignedCharElement()) / 255.0f;
		colorGreen = static_cast<float>(serializer->readUnsignedCharElement()) / 255.0f;
		colorBlue  = static_cast<float>(serializer->readUnsignedCharElement()) / 255.0f;

	}

	// Unserialize referenced atoms

	bool isNodeIndexed;
	unsigned int nodeIndex = 0; // the index of the node in the indexer

	// Read the number of atoms to which this visual model is applied
	unsigned int numberOfAtoms = serializer->readUnsignedIntElement();

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
	unsigned int numberOfPaths = serializer->readUnsignedIntElement();

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

const SBDQuantity::length&	SEPathlineOfCenterOfMassVisualModel::getRadius() const { return radius; }
void						SEPathlineOfCenterOfMassVisualModel::setRadius(const SBQuantity::length& radius) {

	if (this->radius != radius) {

		if      (hasRadiusRange() && radius < getMinimumRadius()) this->radius = getMinimumRadius();
		else if (hasRadiusRange() && radius > getMaximumRadius()) this->radius = getMaximumRadius();
		else this->radius = radius;

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

void SEPathlineOfCenterOfMassVisualModel::display(SBNode::RenderingPass renderingPass) {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. This is the main function of your visual model. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

	if (renderingPass != SBNode::RenderingPass::OpaqueGeometry &&
		renderingPass != SBNode::RenderingPass::ShadowingGeometry &&
		renderingPass != SBNode::RenderingPass::SelectableGeometry)
		return;

	if (pathIndexer.size() == 0) return;
	if (atomIndexer.size() == 0) return;

	const unsigned int nodeIndex = getNodeIndex();

	// get color from the material if it is applied

	if (getMaterial()) {

		if (getMaterial()->getColorScheme()) {

			float color[4];
			getMaterial()->getColorScheme()->getColor(color);
			colorRed   = color[0];
			colorGreen = color[1];
			colorBlue  = color[2];

		}

	}

	SB_FOR(SBPath* path, pathIndexer) {

		if (path->isErased()) continue;

		const unsigned int numberOfSteps = path->getNumberOfSteps();
		if (numberOfSteps == 0) continue;

		const SBPointerIndexer<SBAtom>* pathAtomIndexer = path->getAtomIndexer();
		if (pathAtomIndexer->size() == 0) continue;

		// create an indexer with atoms which are present both in the path and in the user chosen atomIndexer

		SBPointerIndexer<SBAtom> temporaryAtomIndexer;
		SB_FOR(SBAtom* node, atomIndexer) {

			if (node->isErased()) continue;
			if (pathAtomIndexer->hasIndex(node))
				temporaryAtomIndexer.addReferenceTarget(node);

		}
		const unsigned int numberOfAtoms = temporaryAtomIndexer.size();
		if (numberOfAtoms == 0) continue;

		// cylinders

		const unsigned int nCylinders = numberOfSteps - 1;
		const unsigned int nPositionsForCylinders = 2 * nCylinders;					// the two end points
		unsigned int* indexDataForCylinders = new unsigned int[2 * nCylinders];		// this should be of size 2 * cylinder, it will say for each end point at which position it should be
		float *positionDataForCylinders = new float[3 * nPositionsForCylinders];	// all the coordinates X Y Z of each positions,
		float *radiusDataForCylinders = new float[2 * nCylinders];					// the radius of the two end points of the cylinder (can be different to make a cone)
		float *colorDataForCylinders = nullptr;										// the ARGB code for each end point of a cylinder
		unsigned int *flagDataForCylinders = nullptr;								// flags
		unsigned int *nodeIndexDataForCylinders = nullptr;							// controls if the cylinder is highlighted, selected


		if (renderingPass == SBNode::RenderingPass::OpaqueGeometry) {

			colorDataForCylinders = new float[4 * 2 * nCylinders];
			flagDataForCylinders = new unsigned int[2 * nCylinders];

		}

		if (renderingPass == SBNode::RenderingPass::SelectableGeometry)
			nodeIndexDataForCylinders = new unsigned int[2 * nCylinders];

		SBPosition3 position2 = computePosition(path, temporaryAtomIndexer, 0);
		SBPosition3 position1 = position2;

		unsigned int is2, is6, is8;

		for (unsigned int is = 0; is < numberOfSteps - 1; is++) {

			is2 = 2 * is;
			is6 = 6 * is;
			is8 = 8 * is;

			position1 = position2;
			position2 = computePosition(path, temporaryAtomIndexer, is + 1);

			positionDataForCylinders[is6    ] = static_cast<float>(position1.v[0].getValue());
			positionDataForCylinders[is6 + 1] = static_cast<float>(position1.v[1].getValue());
			positionDataForCylinders[is6 + 2] = static_cast<float>(position1.v[2].getValue());
			positionDataForCylinders[is6 + 3] = static_cast<float>(position2.v[0].getValue());
			positionDataForCylinders[is6 + 4] = static_cast<float>(position2.v[1].getValue());
			positionDataForCylinders[is6 + 5] = static_cast<float>(position2.v[2].getValue());

			radiusDataForCylinders[is2    ] = static_cast<float>(radius.getValue());
			radiusDataForCylinders[is2 + 1] = static_cast<float>(radius.getValue());

			indexDataForCylinders[is2    ] = is2;
			indexDataForCylinders[is2 + 1] = is2 + 1;

			if (renderingPass == SBNode::RenderingPass::OpaqueGeometry) {

				colorDataForCylinders[is8    ] = colorRed;
				colorDataForCylinders[is8 + 1] = colorGreen;
				colorDataForCylinders[is8 + 2] = colorBlue;
				colorDataForCylinders[is8 + 3] = 1.0f;
				colorDataForCylinders[is8 + 4] = colorRed;
				colorDataForCylinders[is8 + 5] = colorGreen;
				colorDataForCylinders[is8 + 6] = colorBlue;
				colorDataForCylinders[is8 + 7] = 1.0f;

				flagDataForCylinders[is2    ] = path->getInheritedFlags() | getInheritedFlags();
				flagDataForCylinders[is2 + 1] = path->getInheritedFlags() | getInheritedFlags();

			}

			if (renderingPass == SBNode::RenderingPass::SelectableGeometry) {

				nodeIndexDataForCylinders[is2 + 0] = nodeIndex;
				nodeIndexDataForCylinders[is2 + 1] = nodeIndex;

			}

		}

		if (renderingPass == SBNode::RenderingPass::OpaqueGeometry) {

			// display opaque geometry

			// enable Alpha blending in case of enabled transparency
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//glEnable(GL_DEPTH_TEST);

			SAMSON::displayLineSweptSpheres(nCylinders, nPositionsForCylinders, indexDataForCylinders, positionDataForCylinders, radiusDataForCylinders,
											colorDataForCylinders, flagDataForCylinders, false, true);

			// disable Alpha blending
			//glEnable(GL_DEPTH_TEST);
			//glDisable(GL_BLEND);

		}
		else if (renderingPass == SBNode::RenderingPass::ShadowingGeometry) {

			// display for shadows

			SAMSON::displayLineSweptSpheres(nCylinders, nPositionsForCylinders, indexDataForCylinders, positionDataForCylinders, radiusDataForCylinders,
											nullptr, nullptr, true, true);

		}
		else if (renderingPass == SBNode::RenderingPass::SelectableGeometry) {

			// display for selection

			SAMSON::displayLineSweptSpheresSelection(nCylinders, nPositionsForCylinders, indexDataForCylinders, positionDataForCylinders, radiusDataForCylinders, nodeIndexDataForCylinders);

		}

		if (indexDataForCylinders) delete[] indexDataForCylinders;
		if (positionDataForCylinders) delete[] positionDataForCylinders;
		if (radiusDataForCylinders) delete[] radiusDataForCylinders;
		if (colorDataForCylinders) delete[] colorDataForCylinders;
		if (flagDataForCylinders) delete[] flagDataForCylinders;

		if (nodeIndexDataForCylinders) delete[] nodeIndexDataForCylinders;


		temporaryAtomIndexer.clear();

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
