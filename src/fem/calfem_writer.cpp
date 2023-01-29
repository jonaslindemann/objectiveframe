#include <ofem/calfem_writer.h>

#include <iostream>
#include <map>

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <ofem/beam_load.h>
#include <ofem/beam_model.h>

using namespace ofem;
using namespace std;

CalfemWriter::CalfemWriter(const std::string fname)
    : InputFileWriter(fname)
{
}

void ofem::CalfemWriter::writeHeader(std::ostream& out)
{
    out << "# -*- coding: iso-8859-15 -*-\n";
    out << "'''\n";
    out << "ObjectiveFrame generated CALFEM Python code.\n";
    out << "'''\n\n";
}

void ofem::CalfemWriter::writeImports(std::ostream& out)
{
    out << "import numpy as np\n";
    out << "import calfem.core as cfc\n\n";
}

void ofem::CalfemWriter::beginArr(std::ostream& out, std::string name)
{
    out << name << " = np.array([\n";
}

void ofem::CalfemWriter::beginArr1D(std::ostream& out, std::string name)
{
    out << name << " = np.array(\n";
}

void ofem::CalfemWriter::arrRow(std::ostream& out, std::vector<float> v)
{
    out << "\t" << fmt::format("{::g}", v) << ",\n";
}

void ofem::CalfemWriter::arrRow(std::ostream& out, std::vector<int> v)
{
    out << "\t" << fmt::format("{::d}", v) << ",\n";
}

void ofem::CalfemWriter::arrRow(std::ostream& out, std::vector<double> v)
{
    out << "\t" << fmt::format("{::g}", v) << ",\n";
}

void ofem::CalfemWriter::endArr(std::ostream& out)
{
    out << "])\n";
}

void ofem::CalfemWriter::endArr1D(std::ostream& out)
{
    out << ")\n";
}

void CalfemWriter::saveToStream(std::ostream& out)
{
    writeHeader(out);
    writeImports(out);

    long i, j;
    double E, G, A, Iy, Iz, Kv;
    double x, y, z;
    double ex, ey, ez;

    BeamModel* femModel = (BeamModel*)this->getFemModel();

    if (femModel == NULL)
        return;

    BeamSet* elementSet = femModel->getElementSet();
    NodeSet* nodeSet = femModel->getNodeSet();
    MaterialSet* materialSet = femModel->getMaterialSet();
    NodeBCSet* bcSet = femModel->getNodeBCSet();
    NodeLoadSet* nodeLoadSet = femModel->getNodeLoadSet();
    ElementLoadSet* elementLoadSet = femModel->getElementLoadSet();

    nodeSet->resetNodeKind(nkNotConnected);
    elementSet->updateNodeKinds();

    nodeSet->enumerateNodes();
    materialSet->enumerateMaterials();
    elementSet->enumerateElements();
    nodeLoadSet->enumerateLoads();
    elementLoadSet->enumerateLoads();
    bcSet->enumerateBCs();
    int nDofs = nodeSet->enumerateDofs() - 1;

    //
    // Write materials
    //

    out << "\n# ---- Materials\n"
        << endl;

    beginArr(out, "ep");

    for (i = 0; i < materialSet->getSize(); i++)
    {
        BeamMaterial* material = (BeamMaterial*)materialSet->getMaterial(i);
        material->getProperties(E, G, A, Iy, Iz, Kv);
        arrRow(out, vector<double> { E, G, A, Iy, Iz, Kv });
    }

    endArr(out);

    //
    // Write elements
    //

    out << "\n# ---- Beam material idx\n"
        << endl;

    std::vector<int> matIdx;

    beginArr1D(out, "beam_mat_idx");

    for (i = 0; i < elementSet->getSize(); i++)
    {
        Beam* element = (Beam*)elementSet->getElement(i);
        if (element->beamType() == btBeam)
            matIdx.emplace_back(element->getMaterial()->getNumber() - 1);
    }

    arrRow(out, matIdx);

    endArr1D(out);
    out << "\n";

    out << "\n# ---- Bar material idx\n"
        << endl;

    matIdx.clear();

    beginArr1D(out, "bar_mat_idx");

    for (i = 0; i < elementSet->getSize(); i++)
    {
        Beam* element = (Beam*)elementSet->getElement(i);
        if (element->beamType() == btBar)
            matIdx.emplace_back(element->getMaterial()->getNumber() - 1);
    }

    arrRow(out, matIdx);

    endArr1D(out);
    out << "\n";

    //
    // Write Beam edof
    //

    vector<int> dofRow;

    out << "\n# ---- Beam Edof\n"
        << endl;

    beginArr(out, "edof_beams");

    for (i = 0; i < elementSet->getSize(); i++)
    {
        Beam* element = (Beam*)elementSet->getElement(i);
        if (element->beamType() == btBeam)
        {
            dofRow.clear();

            for (j = 0; j < 6; j++)
                dofRow.emplace_back(element->getNode(0)->getDof(j)->getNumber());

            for (j = 0; j < 6; j++)
                dofRow.emplace_back(element->getNode(1)->getDof(j)->getNumber());

            arrRow(out, dofRow);
        }
    }

    endArr(out);
    out << "\n";       

    //
    // Write Bar edof
    //

    out << "\n# ---- Bar Edof\n"
        << endl;

    beginArr(out, "edof_bars");

    for (i = 0; i < elementSet->getSize(); i++)
    {
        Beam* element = (Beam*)elementSet->getElement(i);
        if (element->beamType() == btBar)
        {
            dofRow.clear();

            for (j = 0; j < 3; j++)
                dofRow.emplace_back(element->getNode(0)->getDof(j)->getNumber());

            for (j = 0; j < 3; j++)
                dofRow.emplace_back(element->getNode(1)->getDof(j)->getNumber());

            arrRow(out, dofRow);
        }
    }

    endArr(out);
    out << "\n";
   
    //
    // Write node coordinates
    //

    out << "\n# ----- Nodes\n"
        << endl;

    beginArr(out, "coords");

    for (i = 0; i < nodeSet->getSize(); i++)
    {
        Node* node = nodeSet->getNode(i);
        node->getCoord(x, y, z);
        arrRow(out, vector<double> { x, y, z });
    }

    endArr(out);
    out << "\n";

    // 
    // Write node dofs
    //

    beginArr(out, "dofs_beams");

    std::vector<int> dofs;

    for (i = 0; i < nodeSet->getSize(); i++)
    {
        auto node = nodeSet->getNode(i);

        dofs.clear();

        for (j = 0; j < 6; j++)
            dofs.emplace_back(node->getDof(j)->getNumber());

        arrRow(out, dofs);
    }

    endArr(out);
    out << "\n";

    //
    // Write node dofs
    //

    beginArr(out, "dofs_bars");

    for (i = 0; i < nodeSet->getSize(); i++)
    {
        auto node = nodeSet->getNode(i);

        dofs.clear();

        for (j = 0; j < 3; j++)
            dofs.emplace_back(node->getDof(j)->getNumber());

        arrRow(out, dofs);
    }

    endArr(out);
    out << "\n";

    //
    // Write specified local z axis
    //

    out << "\n# ----- Local Z-axis\n"
        << endl;

    beginArr(out, "beam_orientation");

    for (i = 0; i < elementSet->getSize(); i++)
    {
        Beam* element = (Beam*)elementSet->getElement(i);
        if (element->beamType() == btBeam)
        {
            element->getOrientationZ(ex, ey, ez);
            arrRow(out, vector<double> { ex, ey, ez });
        }
    }

    endArr(out);

    //
    // Write prescribed displacements X-direction
    //

    out << "\n# ----- Boundary conditions\n"
        << endl;

    map<int, int> node_bc_map;

    beginArr(out, "node_bc");

    for (i = 0; i < bcSet->getSize(); i++)
    {
        NodeBC* nodeBC = (NodeBC*)bcSet->getBC(i);

        vector<int> prescr;

        for (j = 1; j <= 6; j++)
        {
            if (nodeBC->isPrescribed(j))
                prescr.push_back(1);
            else
                prescr.push_back(0);
        }

        arrRow(out, prescr);

        for (j = 0; j < nodeBC->getNodeSize(); j++)
        {
            Node* node = nodeBC->getNode(j);
            node_bc_map[node->getNumber()] = i;
        }

        prescr.clear();
    }

    endArr(out);
    out << "\n";

    beginArr(out, "node_bc_val");

    for (i = 0; i < bcSet->getSize(); i++)
    {
        NodeBC* nodeBC = (NodeBC*)bcSet->getBC(i);

        vector<double> values;

        for (j = 1; j <= 6; j++)
        {
            if (nodeBC->isPrescribed(j))
                values.push_back(nodeBC->getPrescribedValue(1));
            else
                values.push_back(0.0);
        }

        arrRow(out, values);

        values.clear();
    }

    endArr(out);
    out << "\n";

    beginArr(out, "node_bc_idx");

    for (auto& v : node_bc_map)
    {
        arrRow(out, vector<int> { v.first, v.second });
    }

    endArr(out);
    out << "\n";

    //
    // Write nodal loads
    //

    map<int, int> node_load_map;

    beginArr(out, "node_load");

    for (i = 0; i < nodeLoadSet->getSize(); i++)
    {
        NodeLoad* nodeLoad = (NodeLoad*)nodeLoadSet->getLoad(i);

        nodeLoad->getDirection(ex, ey, ez);
        double value = nodeLoad->getValue();

        arrRow(out, vector<double> { ex * value, ey * value, ez * value });

        for (j = 0; j < (long)nodeLoad->getNodeSize(); j++)
        {
            Node* node = nodeLoad->getNode(j);
            node_load_map[node->getNumber()] = i;
        }
    }

    endArr(out);
    out << "\n";

    beginArr(out, "node_load_idx");

    for (auto& v : node_load_map)
    {
        arrRow(out, vector<int> { v.first, v.second });
    }

    endArr(out);
    out << "\n";

    return;

    //
    // Distributed loads
    //

    out << "# Distributed loads" << endl;

    for (i = 0; i < elementLoadSet->getSize(); i++)
    {
        BeamLoad* beamLoad = (BeamLoad*)elementLoadSet->getLoad(i);
        for (j = 0; j < beamLoad->getElementsSize(); j++)
        {
            Beam* beam = (Beam*)beamLoad->getElement(j);
            out << beam->getNumber() - 1 << " ";
            beamLoad->getLocalDirection(ex, ey, ez);
            out << -beamLoad->getValue() * ey << " ";
            out << -beamLoad->getValue() * ez << endl;
        }
    }
    out << -10 << endl;

    out << "# ---" << endl;
    out << -10 << endl;
}
