/// \file SEPathlinesOwnershipTests.cpp
/// \brief Exercises real path updates, geometry replacement, and independent mesh conversion.
#include "SEPathlineOfCenterOfMassVisualModel.hpp"
#include "SBMesh.hpp"
#include "SBSurface.hpp"
#include <QApplication>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
void require(bool condition, const char* message) {
	if (!condition) throw std::runtime_error(message);
}
}


int main(int argc, char** argv) {
	qputenv("QT_QPA_PLATFORM", "offscreen");
	QApplication application(argc, argv);
	try {
		SBPointer<SBAtom> atom = new SBAtom(SBElement::Carbon, SBPosition3::zero);
		atom->create();
		SBNodeIndexer atoms; atoms.push_back(atom());
		SBPointer<SBPath> path = new SBPath("Ownership test", atoms, nullptr);
		path->create();
		SBNodeIndexer nodes; nodes.push_back(atom()); nodes.push_back(path());
		SBPointer<SEPathlineOfCenterOfMassVisualModel> model = new SEPathlineOfCenterOfMassVisualModel(nodes);
		SBPointer<SBMesh> retained;
		float retainedValue = 0;
		for (unsigned int steps : { 3u, 3u, 6u, 2u, 0u, 4u }) {
			auto* frames = steps ? new SBVector<SBVector<SBPosition3>*> : nullptr;
			for (unsigned int i = 0; i < steps; ++i) {
				auto* frame = new SBVector<SBPosition3>;
				frame->push_back(SBPosition3(SBQuantity::angstrom(double(i)), SBQuantity::angstrom(0), SBQuantity::angstrom(0)));
				frames->push_back(frame);
			}
			path->setPositionData(frames);
			SBPointer<SBMesh> mesh = model->createMesh();
			auto* cylinders = static_cast<SBCylinderArray*>(model->getGeometryArrayIndexer().getReferenceTarget(0));
			require(cylinders->getNumberOfGeometries() == (steps ? steps - 1 : 0), "Geometry count follows path changes");
			require(cylinders->getNumberOfPositions() == (steps ? 2 * (steps - 1) : 0), "Position count follows geometry count");
			if (!steps) {
				require(!mesh.isValid() && !cylinders->getPositionData(), "Empty path clears geometry and mesh");
				continue;
			}
			require(mesh.isValid(), "Populated trajectory produces a mesh");
			auto* surface = mesh->getSurfaceIndexer().getReferenceTarget(0);
			require(surface->getPositionData() != cylinders->getPositionData(), "Mesh storage is independent");
			require(std::abs(cylinders->getPositionData()[3] - float(SBQuantity::length(SBQuantity::angstrom(1)).getValue())) < 1e-5f, "Path coordinates use SDK length units");
			for (unsigned int i = 0; i < 3 * surface->getNumberOfTriangles(); ++i)
				require(surface->getIndexData()[i] < surface->getNumberOfPositions(), "Mesh indices are in bounds");
			if (!retained.isValid()) { retained = mesh; retainedValue = surface->getPositionData()[0]; }
			require(retained->getSurfaceIndexer().getReferenceTarget(0)->getPositionData()[0] == retainedValue, "Old mesh survives source refresh");
		}
		model = nullptr; path = nullptr;
		require(retained->getSurfaceIndexer().getReferenceTarget(0)->getPositionData()[0] == retainedValue, "Mesh survives source destruction");
		std::cout << "Pathlines ownership tests passed\n";
		return 0;
	} catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
