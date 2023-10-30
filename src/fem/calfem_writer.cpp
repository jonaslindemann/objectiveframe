#include <ofem/calfem_writer.h>

#include <filesystem>
#include <iostream>
#include <map>

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <ofem/beam_load.h>
#include <ofem/beam_model.h>

#include <logger.h>

#include "base64.h"

using namespace ofem;
using namespace std;

namespace fs = std::filesystem;

const std::string solver_template_part1 = R"CF(# -*- coding: iso-8859-15 -*-
'''
ObjectiveFrame generated CALFEM Python code.
'''

import numpy as np
import calfem.core as cfc

# --- Import model

)CF";

const std::string solver_template_part2 = R"CF(
# --- Extract element coordinates

ex_bm, ey_bm, ez_bm = cfc.coord_extract(edof_beams, coords, dofs_beams)
ex_br, ey_br, ez_br = cfc.coord_extract(edof_bars, coords, dofs_bars)

# --- Setup global matrices

n_dofs = np.max(edof_beams)
n_beams = ex_bm.shape[0]
n_bars = ex_br.shape[0]

K = np.zeros((n_dofs, n_dofs))
f = np.zeros((n_dofs, 1))

# --- Assemble beams

for elx, ely, elz, dofs, epi, eo in zip(ex_bm, ey_bm, ez_bm, edof_beams, beam_mat_idx, beam_orientation):
    Ke = cfc.beam3e(elx, ely, elz, eo, ep[epi])
    K = cfc.assem(dofs, K, Ke)

# --- Assemble bars

for elx, ely, elz, dofs, epi in zip(ex_br, ey_br, ez_br, edof_bars, bar_mat_idx):
    E = ep[epi][0]
    A = ep[epi][2]
    Ke = cfc.bar3e(elx, ely, elz, [E, A])
    K = cfc.assem(dofs, K, Ke)

# --- Setting up loads and boundary conds

bc = []
bc_vals = []

for ni, bi in node_bc_idx:
    bc_prescr_dofs = node_bc[bi]
    bc_values = node_bc_val[bi]
    bc_dofs = dofs_beams[ni-1]
    for i, p_dof in enumerate(bc_prescr_dofs):
        if p_dof == 1:
            bc.append(bc_dofs[i])
            bc_vals.append(bc_values[i])

for ni, li in node_load_idx:
    load_values = node_load[li]
    load_dofs = dofs_beams[ni-1][0:3]
    f[load_dofs-1, 0] += load_values

bc = np.array(bc)
bc_vals = np.array(bc_vals)

# --- Solve equation system

a, r = cfc.solveq(K, f, bc, bc_vals)

# --- Extract element forces

ed_beams = cfc.extract_ed(edof_beams, a)
ed_bars = cfc.extract_ed(edof_bars, a)

beam_forces = []
bar_forces = np.zeros(n_bars)

for elx, ely, elz, ed, epi, eo in zip(ex_bm, ey_bm, ez_bm, ed_beams, beam_mat_idx, beam_orientation):
    es, edi, eci = cfc.beam3s(elx, ely, elz, eo, ep[epi], ed, n=10)
    beam_forces.append([es, edi, eci])

bi = 0

for elx, ely, elz, ed, epi in zip(ex_br, ey_br, ez_br, ed_bars, beam_mat_idx):
    E = ep[epi][0]
    A = ep[epi][2]
    N = cfc.bar3s(elx, ely, elz, [E, A], ed)
    bar_forces[bi] = N
    bi+=1

print(beam_forces)
print(bar_forces)
)CF";

CalfemWriter::CalfemWriter(const std::string fname, bool flipYZ) : InputFileWriter(fname), m_flipYZ{flipYZ}
{
}

void ofem::CalfemWriter::doAfterSave()
{
    fs::path python_data_filename(this->filename());

    std::string parent_path = python_data_filename.parent_path().string();
    std::string base_filename = python_data_filename.stem().string();
    std::string solver_filename = base_filename + "_solver.py";

    fs::path solver_path = python_data_filename.parent_path() /= fs::path(solver_filename);

    std::string full_solver_filename = solver_path.string();

    fstream f;
    f.open(full_solver_filename, ios::out);
    f << solver_template_part1;
    f << "from " << base_filename << " import *"
      << "\n";
    f << solver_template_part2;
    f.close();
}

void ofem::CalfemWriter::writeHeader(std::ostream &out)
{
    out << "# -*- coding: iso-8859-15 -*-\n";
    out << "'''\n";
    out << "ObjectiveFrame generated CALFEM Python code.\n";
    out << "'''\n\n";
}

void ofem::CalfemWriter::writeImports(std::ostream &out)
{
    out << "import numpy as np\n";
    out << "import calfem.core as cfc\n\n";
}

void ofem::CalfemWriter::beginArr(std::ostream &out, std::string name)
{
    out << name << " = np.array([\n";
}

void ofem::CalfemWriter::beginArr1D(std::ostream &out, std::string name)
{
    out << name << " = np.array(\n";
}

void ofem::CalfemWriter::arrRow(std::ostream &out, std::vector<float> v)
{
    out << "\t" << fmt::format("{::g}", v) << ",\n";
}

void ofem::CalfemWriter::arrRow(std::ostream &out, std::vector<int> v)
{
    out << "\t" << fmt::format("{::d}", v) << ",\n";
}

void ofem::CalfemWriter::arrRow(std::ostream &out, std::vector<double> v)
{
    out << "\t" << fmt::format("{::g}", v) << ",\n";
}

void ofem::CalfemWriter::endArr(std::ostream &out)
{
    out << "])\n";
}

void ofem::CalfemWriter::endArr1D(std::ostream &out)
{
    out << ")\n";
}

void ofem::CalfemWriter::writeString(std::ostream &out, std::string name, const std::string &value)
{
    out << name << " = '''" << value << "'''\n";
}

void CalfemWriter::saveToStream(std::ostream &out)
{
    writeHeader(out);
    writeImports(out);

    long i, j;
    double E, G, A, Iy, Iz, Kv;
    double x, y, z;
    double ex, ey, ez;

    BeamModel *femModel = (BeamModel *)this->getFemModel();

    if (femModel == NULL)
        return;

    auto elementSet = femModel->getElementSet();
    auto nodeSet = femModel->getNodeSet();
    auto materialSet = femModel->getMaterialSet();
    auto bcSet = femModel->getNodeBCSet();
    auto nodeLoadSet = femModel->getNodeLoadSet();
    auto elementLoadSet = femModel->getElementLoadSet();

    nodeSet->resetNodeKind(nkNotConnected);
    elementSet->updateNodeKinds();

    nodeSet->enumerateNodes();
    materialSet->enumerateMaterials();
    elementSet->enumerateElements();
    nodeLoadSet->enumerateLoads();
    elementLoadSet->enumerateLoads();
    bcSet->enumerateBCs();
    // int nDofs = nodeSet->enumerateDofs() - 1;
    nodeSet->enumerateDofs();

    //
    // Write df3 file as pem encoded string
    //

    out << "\n# ---- Embedded df3 file\n" << endl;

    std::string df3_string = femModel->toString();
    std::string df3_pem_encoded = base64_encode_mime(df3_string);
    this->writeString(out, "df3_file", df3_pem_encoded);

    //
    // Write materials
    //

    out << "\n# ---- Materials\n" << endl;

    beginArr(out, "ep");

    for (i = 0; i < materialSet->getSize(); i++) {
        BeamMaterial *material = (BeamMaterial *)materialSet->getMaterial(i);
        material->getProperties(E, G, A, Iy, Iz, Kv);
        arrRow(out, vector<double>{E, G, A, Iy, Iz, Kv});
    }

    endArr(out);

    //
    // Write elements
    //

    out << "\n# ---- Beam material idx\n" << endl;

    std::vector<int> matIdx;

    beginArr1D(out, "beam_mat_idx");

    for (i = 0; i < elementSet->getSize(); i++) {
        Beam *element = (Beam *)elementSet->getElement(i);
        if (element->beamType() == btBeam)
            matIdx.emplace_back(element->getMaterial()->getNumber() - 1);
    }

    arrRow(out, matIdx);

    endArr1D(out);
    out << "\n";

    out << "\n# ---- Bar material idx\n" << endl;

    matIdx.clear();

    beginArr1D(out, "bar_mat_idx");

    for (i = 0; i < elementSet->getSize(); i++) {
        Beam *element = (Beam *)elementSet->getElement(i);
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

    out << "\n# ---- Beam Edof\n" << endl;

    beginArr(out, "edof_beams");

    for (i = 0; i < elementSet->getSize(); i++) {
        Beam *element = (Beam *)elementSet->getElement(i);
        if (element->beamType() == btBeam) {
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

    out << "\n# ---- Bar Edof\n" << endl;

    beginArr(out, "edof_bars");

    for (i = 0; i < elementSet->getSize(); i++) {
        Beam *element = (Beam *)elementSet->getElement(i);
        if (element->beamType() == btBar) {
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

    out << "\n# ----- Nodes\n" << endl;

    beginArr(out, "coords_beams");

    for (i = 0; i < nodeSet->getSize(); i++) {
        Node *node = nodeSet->getNode(i);
        if (node->getKind() == NodeKind::nk6Dof) {
            node->getCoord(x, y, z);

            if (m_flipYZ)
                arrRow(out, vector<double>{x, z, y});
            else
                arrRow(out, vector<double>{x, y, z});
        }
    }

    endArr(out);
    out << "\n";

    beginArr(out, "coords_bars");

    for (i = 0; i < nodeSet->getSize(); i++) {
        Node *node = nodeSet->getNode(i);
        if ((node->getKind() == NodeKind::nk6Dof) || (node->getKind() == NodeKind::nk3Dof)) {
            node->getCoord(x, y, z);

            if (m_flipYZ)
                arrRow(out, vector<double>{x, z, y});
            else
                arrRow(out, vector<double>{x, y, z});
        }
    }

    endArr(out);
    out << "\n";

    //
    // Write node dofs
    //

    beginArr(out, "dofs_beams");

    std::vector<int> dofs;

    for (i = 0; i < nodeSet->getSize(); i++) {
        auto node = nodeSet->getNode(i);

        if ((node->getKind() == NodeKind::nk3Dof) || (node->getKind() == NodeKind::nk6Dof)) {
            dofs.clear();

            if (node->getKind() == NodeKind::nk6Dof) {
                for (j = 0; j < 6; j++)
                    dofs.emplace_back(node->getDof(j)->getNumber());
            }
            else {
                for (j = 0; j < 3; j++)
                    dofs.emplace_back(node->getDof(j)->getNumber());
                for (j = 3; j < 6; j++)
                    dofs.emplace_back(-1);
            }

            arrRow(out, dofs);
        }
    }

    endArr(out);
    out << "\n";

    //
    // Write node dofs
    //

    beginArr(out, "dofs_bars");

    for (i = 0; i < nodeSet->getSize(); i++) {
        auto node = nodeSet->getNode(i);

        if ((node->getKind() == NodeKind::nk3Dof) || (node->getKind() == NodeKind::nk6Dof)) {
            dofs.clear();

            for (j = 0; j < 3; j++)
                dofs.emplace_back(node->getDof(j)->getNumber());

            arrRow(out, dofs);
        }
    }

    endArr(out);
    out << "\n";

    //
    // Write specified local z axis
    //

    out << "\n# ----- Local Z-axis\n" << endl;

    beginArr(out, "beam_orientation");

    for (i = 0; i < elementSet->getSize(); i++) {
        Beam *element = (Beam *)elementSet->getElement(i);
        if (element->beamType() == btBeam) {
            element->getOrientationZ(ex, ey, ez);
            if (m_flipYZ)
                arrRow(out, vector<double>{ex, ez, ey});
            else
                arrRow(out, vector<double>{ex, ey, ez});
        }
    }

    endArr(out);

    //
    // Write prescribed displacements X-direction
    //

    out << "\n# ----- Boundary conditions\n" << endl;

    map<int, int> node_bc_map;

    beginArr(out, "node_bc");

    for (i = 0; i < bcSet->getSize(); i++) {
        NodeBC *nodeBC = (NodeBC *)bcSet->getBC(i);

        vector<int> prescr;

        for (j = 1; j <= 6; j++) {
            if (nodeBC->isPrescribed(j))
                prescr.push_back(1);
            else
                prescr.push_back(0);
        }

        arrRow(out, prescr);

        for (j = 0; j < nodeBC->getNodeSize(); j++) {
            Node *node = nodeBC->getNode(j);
            node_bc_map[node->getNumber()] = i;
        }

        prescr.clear();
    }

    endArr(out);
    out << "\n";

    beginArr(out, "node_bc_val");

    for (i = 0; i < bcSet->getSize(); i++) {
        NodeBC *nodeBC = (NodeBC *)bcSet->getBC(i);

        vector<double> values;

        for (j = 1; j <= 6; j++) {
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

    for (auto &v : node_bc_map) {
        arrRow(out, vector<int>{v.first, v.second});
    }

    endArr(out);
    out << "\n";

    //
    // Write nodal loads
    //

    map<int, int> node_load_map;

    beginArr(out, "node_load");

    for (i = 0; i < nodeLoadSet->getSize(); i++) {
        NodeLoad *nodeLoad = (NodeLoad *)nodeLoadSet->getLoad(i);

        nodeLoad->getDirection(ex, ey, ez);
        double value = nodeLoad->getValue();

        if (m_flipYZ)
            arrRow(out, vector<double>{ex * value, ez * value, ey * value});
        else
            arrRow(out, vector<double>{ex * value, ey * value, ez * value});

        for (j = 0; j < (long)nodeLoad->getNodeSize(); j++) {
            Node *node = nodeLoad->getNode(j);
            node_load_map[node->getNumber()] = i;
        }
    }

    endArr(out);
    out << "\n";

    beginArr(out, "node_load_idx");

    for (auto &v : node_load_map) {
        arrRow(out, vector<int>{v.first, v.second});
    }

    endArr(out);
    out << "\n";

    return;

    //
    // Distributed loads
    //

    out << "# Distributed loads" << endl;

    for (i = 0; i < elementLoadSet->getSize(); i++) {
        BeamLoad *beamLoad = (BeamLoad *)elementLoadSet->getLoad(i);
        for (j = 0; j < beamLoad->getElementsSize(); j++) {
            Beam *beam = (Beam *)beamLoad->getElement(j);
            out << beam->getNumber() - 1 << " ";
            if (m_flipYZ)
                beamLoad->getLocalDirection(ex, ez, ey);
            else
                beamLoad->getLocalDirection(ex, ey, ez);

            out << -beamLoad->getValue() * ey << " ";
            out << -beamLoad->getValue() * ez << endl;
        }
    }
    out << -10 << endl;

    out << "# ---" << endl;
    out << -10 << endl;
}
