#include "SEPathlineOfCenterOfMassVisualModel.hpp"
#include "SAMSON.hpp"
#include "SBQuantity.hpp"
#include "SBDColorSchemeConstant.hpp"

//#include <QOpenGLShaderProgram>

SEPathlineOfCenterOfMassVisualModel::SEPathlineOfCenterOfMassVisualModel() {

	// SAMSON Element generator pro tip: this default constructor is called when unserializing the node, so it should perform all default initializations.

}

SEPathlineOfCenterOfMassVisualModel::SEPathlineOfCenterOfMassVisualModel(const SBNodeIndexer& nodeIndexer) {

	// SAMSON Element generator pro tip: implement this function if you want your visual model to be applied to a set of data graph nodes.
	// You might want to connect to various signals and handle the corresponding events. For example, if your visual model represents a sphere positioned at
	// the center of mass of a group of atoms, you might want to connect to the atoms' base signals (e.g. to update the center of mass when an atom is erased) and
	// the atoms' structural signals (e.g. to update the center of mass when an atom is moved).

	radius = SBQuantity::length(30);	// set a default radius
	colorRed   = 255;					// set a default color
	colorGreen = 0;
	colorBlue  = 0;

	// nodeIndexer may contain both atoms and paths

	SBNodeIndexer temporaryPathIndexer;
	SBNodeIndexer temporaryAtomIndexer;

	SB_FOR(SBNode* node, nodeIndexer) {

		node->getNodes(temporaryPathIndexer, SBNode::IsType(SBNode::Path));							// get Path nodes from the nodeIndexer and add them to the temporaru indexer
		node->getNodes(temporaryAtomIndexer, SBNode::IsType(SBNode::Atom));							// get Atom nodes from the nodeIndexer and add them to the temporaru indexer

	}

	if (temporaryPathIndexer.size() == 0)															// if no Paths selected
		SAMSON::getActiveDocument()->getNodes(temporaryPathIndexer, SBNode::IsType(SBNode::Path));	// get all Paths from the active Document

	if (temporaryAtomIndexer.size() == 0)															// if no Atoms selected
		SAMSON::getActiveDocument()->getNodes(temporaryAtomIndexer, SBNode::IsType(SBNode::Atom));	// get all Atoms from the active Document

	SB_FOR(SBNode* node, temporaryPathIndexer)
		pathIndexer.addReferenceTarget(node);														// add to the internal indexer references to the chosen Paths

	SB_FOR(SBNode* node, temporaryAtomIndexer)
		atomIndexer.addReferenceTarget(node);

	temporaryPathIndexer.clear();
	temporaryAtomIndexer.clear();

	connectBaseSignalToSlot(this, SB_SLOT(&SEPathlineOfCenterOfMassVisualModel::onBaseEvent));

	QString pathsStr(" paths");
	if (pathIndexer.size() == 1) pathsStr = " path";
	SAMSON::setStatusMessage(QString("Visual model: added ") + QString::number(pathIndexer.size()) + pathsStr + QString(" of the center of mass"), 0);

}

SEPathlineOfCenterOfMassVisualModel::~SEPathlineOfCenterOfMassVisualModel() {

	// SAMSON Element generator pro tip: disconnect from signals you might have connected to.

	disconnectBaseSignalFromSlot(this, SB_SLOT(&SEPathlineOfCenterOfMassVisualModel::onBaseEvent));

#ifdef _WIN32

#elif __APPLE__

#elif __linux__
	if (getMaterial())
		removeMaterial();
#endif

}

 bool SEPathlineOfCenterOfMassVisualModel::isSerializable() const {

	// SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc. 
	// Please refer to the SDK documentation for more information.
	// Modify the line below to "return true;" if you want this visual model be serializable (hence copyable, savable, etc.)

	return false;
	
}

void SEPathlineOfCenterOfMassVisualModel::serialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber, const SBVersionNumber& classVersionNumber) const {

	SBMVisualModel::serialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);

	// SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc. 
	// Please refer to the SDK documentation for more information.
	// Complete this function to serialize your visual model.

}

void SEPathlineOfCenterOfMassVisualModel::unserialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber, const SBVersionNumber& classVersionNumber) {

	SBMVisualModel::unserialize(serializer, nodeIndexer, sdkVersionNumber, classVersionNumber);
	
	// SAMSON Element generator pro tip: serialization is used in SAMSON to e.g. save documents, copy nodes, etc. 
	// Please refer to the SDK documentation for more information.
	// Complete this function to unserialize your visual model.

}

void SEPathlineOfCenterOfMassVisualModel::eraseImplementation() {

	// SAMSON Element generator pro tip: modify this function when you need to perform special tasks when your visual model is erased (e.g. disconnect from nodes you are connected to).
	// Important: this function must be undoable (i.e. only call undoable functions or add an undo command to the undo stack)

}

const SBDQuantity::length& SEPathlineOfCenterOfMassVisualModel::getRadius() const { return radius; }
void SEPathlineOfCenterOfMassVisualModel::setRadius(const SBQuantity::length& r) { radius = r; update(); }

const unsigned char& SEPathlineOfCenterOfMassVisualModel::getRed()		const { return colorRed; }
const unsigned char& SEPathlineOfCenterOfMassVisualModel::getGreen()	const { return colorGreen; }
const unsigned char& SEPathlineOfCenterOfMassVisualModel::getBlue()		const { return colorBlue; }
void SEPathlineOfCenterOfMassVisualModel::setRed  (const unsigned char& c)	{ colorRed   = c; update(); }
void SEPathlineOfCenterOfMassVisualModel::setGreen(const unsigned char& c)	{ colorGreen = c; update(); }
void SEPathlineOfCenterOfMassVisualModel::setBlue (const unsigned char& c)	{ colorBlue  = c; update(); }

void SEPathlineOfCenterOfMassVisualModel::update() {

	SAMSON::requestViewportUpdate();

}

void SEPathlineOfCenterOfMassVisualModel::display() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop. This is the main function of your visual model. 
	// Implement this function to display things in SAMSON, for example thanks to the utility functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

	if (pathIndexer.size() == 0) return;
	if (atomIndexer.size() == 0) return;

	SB_FOR(SBPath* path, pathIndexer) {

		if (path->isErased()) continue;

		const unsigned int numberOfSteps = path->getNumberOfSteps();
		if (numberOfSteps == 0) continue;

		const SBPointerIndexer<SBStructuralParticle>* pathAtomIndexer = path->getStructuralParticleIndexer();
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

		// spheres
		const unsigned int nSpheres = numberOfSteps;
		float* positionDataForSpheres = new float[3 * nSpheres];
		float* colorDataForSpheres = new float[4 * nSpheres];

		// cylinders
		const unsigned int nCylinders = numberOfSteps - 1;
		const unsigned int nPositionsForCylinders = 2 * nCylinders; // the two end points
		unsigned int* indexDataForCylinders = new unsigned int[2 * nCylinders]; // this should be of size 2 * cylinder, it will say for each end point at which position it should be
		float *positionDataForCylinders = new float[3 * nPositionsForCylinders]; // all the coordinates X Y Z of each positions,
		float *radiusDataForCylinders = new float[2 * nCylinders]; // the radius of the two end points of the cylinder (can be different to make a cone)
		float *colorDataForCylinders = new float[4 * 2 * nCylinders]; // the ARGB code for each end point of a cylinder
		unsigned int *flagDataForCylinders = new unsigned int[2 * nCylinders]; // controls if the cylinder is highlighted, selected

		for (unsigned int is = 0; is < numberOfSteps; is++) {

			positionDataForSpheres[3 * is + 0] = 0.0;
			positionDataForSpheres[3 * is + 1] = 0.0;
			positionDataForSpheres[3 * is + 2] = 0.0;

			for (unsigned int ia = 0; ia < numberOfAtoms; ia++) {

				SBPosition3 position;
				path->getPosition(is, temporaryAtomIndexer[ia], position);
				positionDataForSpheres[3 * is + 0] += (float)position.v[0].getValue();
				positionDataForSpheres[3 * is + 1] += (float)position.v[1].getValue();
				positionDataForSpheres[3 * is + 2] += (float)position.v[2].getValue();

			}

			positionDataForSpheres[3 * is + 0] /= (float)numberOfAtoms;
			positionDataForSpheres[3 * is + 1] /= (float)numberOfAtoms;
			positionDataForSpheres[3 * is + 2] /= (float)numberOfAtoms;

			colorDataForSpheres[4 * is + 0] = (float)colorRed   / 255.0f;
			colorDataForSpheres[4 * is + 1] = (float)colorGreen / 255.0f;
			colorDataForSpheres[4 * is + 2] = (float)colorBlue  / 255.0f;

			colorDataForSpheres[4 * is + 3] = 1.0f; // non-transparent

		}

		for (unsigned int is = 0; is < numberOfSteps - 1; is++) {

			positionDataForCylinders[6 * is + 0] = positionDataForSpheres[3 * is + 0];
			positionDataForCylinders[6 * is + 1] = positionDataForSpheres[3 * is + 1];
			positionDataForCylinders[6 * is + 2] = positionDataForSpheres[3 * is + 2];
			positionDataForCylinders[6 * is + 3] = positionDataForSpheres[3 * (is + 1) + 0];
			positionDataForCylinders[6 * is + 4] = positionDataForSpheres[3 * (is + 1) + 1];
			positionDataForCylinders[6 * is + 5] = positionDataForSpheres[3 * (is + 1) + 2];

			radiusDataForCylinders[2 * is + 0] = radius.getValue();
			radiusDataForCylinders[2 * is + 1] = radius.getValue();

			indexDataForCylinders[2 * is + 0] = 2 * is + 0;
			indexDataForCylinders[2 * is + 1] = 2 * is + 1;

			colorDataForCylinders[8 * is + 0] = colorDataForSpheres[4 * is + 0];
			colorDataForCylinders[8 * is + 1] = colorDataForSpheres[4 * is + 1];
			colorDataForCylinders[8 * is + 2] = colorDataForSpheres[4 * is + 2];
			colorDataForCylinders[8 * is + 3] = colorDataForSpheres[4 * is + 3];
			colorDataForCylinders[8 * is + 4] = colorDataForSpheres[4 * (is + 1) + 0];
			colorDataForCylinders[8 * is + 5] = colorDataForSpheres[4 * (is + 1) + 1];
			colorDataForCylinders[8 * is + 6] = colorDataForSpheres[4 * (is + 1) + 2];
			colorDataForCylinders[8 * is + 7] = colorDataForSpheres[4 * (is + 1) + 3];

			flagDataForCylinders[2* is + 0] = path->getInheritedFlags() | getInheritedFlags();
			flagDataForCylinders[2* is + 1] = path->getInheritedFlags() | getInheritedFlags();

		}

		// enable Alpha blending in case of enabled transparency
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glEnable(GL_DEPTH_TEST);

		SAMSON::displayLineSweptSpheres(nCylinders,
										nPositionsForCylinders,
										indexDataForCylinders,
										positionDataForCylinders,
										radiusDataForCylinders,
										colorDataForCylinders,
										flagDataForCylinders);

		// disable Alpha blending
//		glEnable(GL_DEPTH_TEST);
//		glDisable(GL_BLEND);

		delete[] positionDataForSpheres;
		delete[] colorDataForSpheres;

		delete[] indexDataForCylinders;
		delete[] positionDataForCylinders;
		delete[] radiusDataForCylinders;
		delete[] colorDataForCylinders;
		delete[] flagDataForCylinders;

		temporaryAtomIndexer.clear();

	}

}

void SEPathlineOfCenterOfMassVisualModel::displayForShadow() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to compute shadows. 
	// Implement this function so that your visual model can cast shadows to other objects in SAMSON, for example thanks to the utility
	// functions provided by SAMSON (e.g. displaySpheres, displayTriangles, etc.)

	display();

}

void SEPathlineOfCenterOfMassVisualModel::displayForSelection() {

	// SAMSON Element generator pro tip: this function is called by SAMSON during the main rendering loop in order to perform object picking.
	// Instead of rendering colors, your visual model is expected to write the index of a data graph node (obtained with getIndex()).
	// Implement this function so that your visual model can be selected (if you render its own index) or can be used to select other objects (if you render 
	// the other objects' indices), for example thanks to the utility functions provided by SAMSON (e.g. displaySpheresSelection, displayTrianglesSelection, etc.)

	if (pathIndexer.size() == 0) return;
	if (atomIndexer.size() == 0) return;

	const unsigned int nodeIndex = getNodeIndex();

	SB_FOR(SBPath* path, pathIndexer) {

		const unsigned int numberOfSteps = path->getNumberOfSteps();
		if (numberOfSteps == 0) continue;

		const SBPointerIndexer<SBStructuralParticle>* pathAtomIndexer = path->getStructuralParticleIndexer();
		if (pathAtomIndexer->size() == 0) continue;

		// create an indexer with atoms which are present both in the path and in the user chosen atomIndexer
		SBPointerIndexer<SBAtom> temporaryAtomIndexer;
		SB_FOR(SBAtom* node, atomIndexer) {
			if (pathAtomIndexer->hasIndex(node))
				temporaryAtomIndexer.addReferenceTarget(node);
		}
		const unsigned int numberOfAtoms = temporaryAtomIndexer.size();
		if (numberOfAtoms == 0) continue;

		// spheres
		unsigned int nSpheres = numberOfSteps;
		float* positionDataForSpheres = new float[3 * nSpheres];

		// cylinders
		unsigned int nCylinders = numberOfSteps - 1;
		unsigned int nPositionsForCylinders = 2 * nCylinders; // the two end points
		unsigned int* indexDataForCylinders = new unsigned int[2 * nCylinders]; // this should be of size 2 * cylinder, it will say for each end point at which position it should be
		float *positionDataForCylinders = new float[3 * nPositionsForCylinders]; // all the coordinates X Y Z of each positions,
		float *radiusDataForCylinders = new float[2 * nCylinders]; // the radius of the two end points of the cylinder (can be different to make a cone)
		unsigned int *nodeIndexDataForCylinders = new unsigned int[2 * nCylinders]; // controls if the cylinder is highlighted, selected

		for (unsigned int is = 0; is < numberOfSteps; is++) {

			positionDataForSpheres[3 * is + 0] = 0.0;
			positionDataForSpheres[3 * is + 1] = 0.0;
			positionDataForSpheres[3 * is + 2] = 0.0;

			for (unsigned int ia = 0; ia < numberOfAtoms; ia++) {

				SBPosition3 position;
				path->getPosition(is, temporaryAtomIndexer[ia], position);
				positionDataForSpheres[3 * is + 0] += (float)position.v[0].getValue();
				positionDataForSpheres[3 * is + 1] += (float)position.v[1].getValue();
				positionDataForSpheres[3 * is + 2] += (float)position.v[2].getValue();

			}

			positionDataForSpheres[3 * is + 0] /= (float)numberOfAtoms;
			positionDataForSpheres[3 * is + 1] /= (float)numberOfAtoms;
			positionDataForSpheres[3 * is + 2] /= (float)numberOfAtoms;

		}

		for (unsigned int is = 0; is < numberOfSteps - 1; is++) {

			positionDataForCylinders[6 * is + 0] = positionDataForSpheres[3 * is + 0];
			positionDataForCylinders[6 * is + 1] = positionDataForSpheres[3 * is + 1];
			positionDataForCylinders[6 * is + 2] = positionDataForSpheres[3 * is + 2];
			positionDataForCylinders[6 * is + 3] = positionDataForSpheres[3 * (is + 1) + 0];
			positionDataForCylinders[6 * is + 4] = positionDataForSpheres[3 * (is + 1) + 1];
			positionDataForCylinders[6 * is + 5] = positionDataForSpheres[3 * (is + 1) + 2];

			radiusDataForCylinders[2 * is + 0] = radius.getValue();
			radiusDataForCylinders[2 * is + 1] = radius.getValue();

			indexDataForCylinders[2 * is + 0] = 2 * is + 0;
			indexDataForCylinders[2 * is + 1] = 2 * is + 1;

			nodeIndexDataForCylinders[2* is + 0] = nodeIndex;
			nodeIndexDataForCylinders[2* is + 1] = nodeIndex;

		}

		SAMSON::displayLineSweptSpheresSelection(nCylinders,
										nPositionsForCylinders,
										indexDataForCylinders,
										positionDataForCylinders,
										radiusDataForCylinders,
										nodeIndexDataForCylinders);


		delete[] positionDataForSpheres;

		delete[] indexDataForCylinders;
		delete[] positionDataForCylinders;
		delete[] radiusDataForCylinders;
		delete[] nodeIndexDataForCylinders;

	}

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
