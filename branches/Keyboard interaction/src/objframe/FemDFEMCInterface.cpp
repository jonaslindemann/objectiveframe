#include "FemDFEMCInterface.h"

#ifdef HAVE_CORBA
#include <OB/CORBA.h>
#include <OB/CosNaming.h>
#include <dfemc.h>

#ifdef HAVE_CONFIG_H
#include <OB/Config.h>
#endif
#else
#include <config.h>
#endif

#ifdef HAVE_IOSTREAM
#include <iostream>
using namespace std;
#else
#include <iostream.h>
#endif

#ifdef HAVE_CMATH
#include <cmath>
#else
#include <math.h>
#endif

#include <FemElementSet.h>
#include <FemNodeSet.h>
#include <FemMaterialSet.h>
#include <FemNodeBCSet.h>
#include <FemNodeLoadSet.h>
#include <FemElementLoadSet.h>
#include <FemBeamLoad.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFemDFEMCInterface::CFemDFEMCInterface()
{
    m_beamModel = NULL;
    m_maxNodeValue = -1.0e300;
}

CFemDFEMCInterface::~CFemDFEMCInterface()
{

}

void CFemDFEMCInterface::setBeamModel(CFemBeamModel *model)
{
    m_beamModel = model;
}

void CFemDFEMCInterface::execute()
{
#ifdef HAVE_CORBA
    if (m_beamModel!=NULL)
    {
        try
        {
            //
            // Counter variables
            //

            int i, j, k, dofCount;

            //////////////////////////////////////////////////////
            // Initialize ORB
            //

            CORBA::ORB_var orb = CORBA::ORB_init(m_argc,m_argv);

            //
            // Get initial naming context
            //

            CORBA::Object_var nameRef;
            try
            {
                nameRef = orb -> resolve_initial_references("NameService");
            }
            catch(const CORBA::ORB::InvalidName&)
            {
                cerr << m_argv[0] << ": can't resolve `NameService'" << endl;
                return;
            }

            CosNaming::NamingContext_var inc;
            inc = CosNaming::NamingContext::_narrow(nameRef);
            assert(!CORBA::is_nil(inc));

            //
            // Get model factory reference
            //

            CORBA::Object_var systemFactoryRef;
            CosNaming::Name name;
            name.length(3);
            name[0].id = CORBA::string_dup("dfemc");
            name[1].id = CORBA::string_dup("factories");
            name[2].id = CORBA::string_dup("FemSystemFactory");

            try {
                systemFactoryRef = inc->resolve(name);
            } catch (const CosNaming::NamingContext::NotFound &) {
                cerr << "No name for model factory." << endl;
                throw 0;
            } catch (const CORBA::Exception &e) {
                cerr << "Resolve failed: " << e << endl;
                throw 0;
            }

            //////////////////////////////////////////////////////
            // Create a FemSystem object
            //

            //
            // Try to narrow DFEMC::FemModel
            //

            DFEMC::FemSystemFactory_var dfemcSystemFactory;
            try
            {
                dfemcSystemFactory = DFEMC::FemSystemFactory::_narrow(systemFactoryRef);
            }
            catch (const CORBA::SystemException& se)
            {
                cerr << "Cannot narrow FemSystemFactory reference: " << se << endl;
                throw 0;
            }

            if (CORBA::is_nil(dfemcSystemFactory))
            {
                cerr << "FemSystemFactory is nil." << endl;
                throw 0;
            }

            //////////////////////////////////////////////////////
            // Get all object references
            //

            //
            // Create a FemSystem object
            //

            DFEMC::FemSystem_var dfemcSystem;
            dfemcSystem = dfemcSystemFactory->createSystem();

            if (CORBA::is_nil(dfemcSystem))
            {
                cerr << "FemSystem is nil." << endl;
                throw 0;
            }

            //
            // Get a FemModel
            //

            DFEMC::FemModel_var dfemcModel = dfemcSystem->getFemModel();

            if (CORBA::is_nil(dfemcModel))
            {
                cerr << "FemModel is nil." << endl;
                throw 0;
            }

            DFEMC::FemNodeSet_var dfemcNodeSet = dfemcModel->getNodeSet();

            if (CORBA::is_nil(dfemcNodeSet))
            {
                cerr << "FemNodeSet is nil." << endl;
                throw 0;
            }

            DFEMC::FemElementSet_var dfemcElementSet = dfemcModel->getElementSet();

            if (CORBA::is_nil(dfemcElementSet))
            {
                cerr << "FemElementSet is nil." << endl;
                throw 0;
            }

            DFEMC::FemMaterialSet_var dfemcMaterialSet = dfemcModel->getMaterialSet();

            if (CORBA::is_nil(dfemcMaterialSet))
            {
                cerr << "FemMaterialSet is nil." << endl;
                throw 0;
            }

            DFEMC::FemNodeBCSet_var dfemcNodeBCSet = dfemcModel->getNodeBCSet();

            if (CORBA::is_nil(dfemcNodeBCSet))
            {
                cerr << "FemNodeBCSet is nil." << endl;
                throw 0;
            }

            DFEMC::FemNodeLoadSet_var dfemcNodeLoadSet = dfemcModel->getNodeLoadSet();

            if (CORBA::is_nil(dfemcNodeLoadSet))
            {
                cerr << "FemNodeLoadSet is nil." << endl;
                throw 0;
            }

            DFEMC::FemElementLoadSet_var dfemcElementLoadSet = dfemcModel->getElementLoadSet();

            if (CORBA::is_nil(dfemcElementLoadSet))
            {
                cerr << "FemElementLoadSet is nil." << endl;
                throw 0;
            }

            //
            // Get a FemCalc calculation control object
            //

            DFEMC::FemCalc_var dfemcCalc = dfemcSystem->getFemCalc();

            if (CORBA::is_nil(dfemcCalc))
            {
                cerr << "FemCalc is nil." << endl;
                throw 0;
            }

            //////////////////////////////////////////////////////
            // Define model in terms of DFEMC
            //

            double E,G,A,Iy,Iz,Kv;
            double x, y, z;
            double ex, ey, ez;

            CFemBeamModel* femModel = m_beamModel;

            if (femModel==NULL)
                return;

            CFemElementSet* elementSet = femModel->getElementSet();
            CFemNodeSet* nodeSet = femModel->getNodeSet();
            CFemMaterialSet* materialSet = femModel->getMaterialSet();
            CFemNodeBCSet* bcSet = femModel->getNodeBCSet();
            CFemNodeLoadSet* nodeLoadSet = femModel->getNodeLoadSet();
            CFemElementLoadSet* elementLoadSet = femModel->getElementLoadSet();

            nodeSet->enumerateNodes();
            materialSet->enumerateMaterials();
            elementSet->enumerateElements();

            //////////////////////////////////////////////////////
            // Write problem information
            //

            dfemcNodeSet->setSize(nodeSet->getSize());
            dfemcElementSet->setSize(elementSet->getSize());
            dfemcMaterialSet->setSize(materialSet->getSize());
            dfemcNodeLoadSet->setSize(nodeLoadSet->getSize());
            dfemcNodeBCSet->setSize(bcSet->getSize());
            dfemcElementLoadSet->setSize(elementLoadSet->getSize());

            //////////////////////////////////////////////////////
            // Write materials
            //

            DFEMC::FemMaterial_var dfemcMaterial = dfemcMaterialSet->getMaterial();

            dfemcMaterialSet->first();

            for (i=0; i<materialSet->getSize(); i++)
            {
                CFemBeamMaterial* material = (CFemBeamMaterial*) materialSet->getMaterial(i);
                material->getProperties(E,G,A,Iy,Iz,Kv);

                dfemcMaterial->setProperty(1, E);
                dfemcMaterial->setProperty(2, G);
                dfemcMaterial->setProperty(3, A);
                dfemcMaterial->setProperty(4, Iy);
                dfemcMaterial->setProperty(5, Iz);
                dfemcMaterial->setProperty(6, Kv);
                dfemcMaterialSet->next();
            }

            //////////////////////////////////////////////////////
            // Write nodes
            //

            DFEMC::FemNode_var dfemcNode = dfemcNodeSet->getNode();

            dfemcNodeSet->first();

            for (i=0; i<nodeSet->getSize(); i++)
            {
                CFemNode* node = nodeSet->getNode(i);
                node->getCoord(x, y, z);
                dfemcNode->setCoord(x, y, z);
                dfemcNodeSet->next();
            }

            //////////////////////////////////////////////////////
            // Write elements
            //

            DFEMC::FemElement_var dfemcElement = dfemcElementSet->getElement();

            dfemcElementSet->first();

            for (i=0; i<elementSet->getSize(); i++)
            {
                CFemBeam* element = (CFemBeam*) elementSet->getElement(i);
                dfemcElement->addNode(element->getNode(0)->getNumber());
                dfemcElement->addNode(element->getNode(1)->getNumber());
                dfemcElement->setMaterial(element->getMaterial()->getNumber());
                dfemcElementSet->next();
            }

            //////////////////////////////////////////////////////
            // Write boundary conditions
            //

            DFEMC::FemNodeBC_var dfemcNodeBC = dfemcNodeBCSet->getBC();

            dfemcNodeBCSet->first();

            for (i=0; i<bcSet->getSize(); i++)
            {
                CFemNodeBC* nodeBC = (CFemNodeBC*)bcSet->getBC(i);

                for (j=1; j<=6; j++)
                {
                    if  (nodeBC->isPrescribed(j))
                        dfemcNodeBC->prescribe(j, nodeBC->getPrescribedValue(j));
                }

                for (j=0; j<nodeBC->getNodeSize(); j++)
                {
                    CFemNode* node = nodeBC->getNode(j);
                    dfemcNodeBC->addNode(node->getNumber());
                }

                dfemcNodeBCSet->next();
            }

            //////////////////////////////////////////////////////
            // Write node loads
            //

            DFEMC::FemNodeLoad_var dfemcNodeLoad = dfemcNodeLoadSet->getLoad();

            dfemcNodeLoadSet->first();

            for (i=0; i<nodeLoadSet->getSize(); i++)
            {
                CFemNodeLoad* nodeLoad = (CFemNodeLoad*)nodeLoadSet->getLoad(i);

                dfemcNodeLoad->setValue(nodeLoad->getValue());
                nodeLoad->getDirection(ex, ey, ez);
                dfemcNodeLoad->setDirection(ex, ey, ez);

                for (j=0; j<nodeLoad->getNodeSize(); j++)
                {
                    CFemNode* node = nodeLoad->getNode(j);
                    dfemcNodeLoad->addNode(node->getNumber());
                }

                dfemcNodeLoadSet->next();
            }

            //////////////////////////////////////////////////////
            // Write element loads
            //

            DFEMC::FemElementLoad_var dfemcElementLoad = dfemcElementLoadSet->getLoad();

            dfemcElementLoadSet->first();

            for (i=0; i<elementLoadSet->getSize(); i++)
            {
                CFemBeamLoad* elementLoad = (CFemBeamLoad*)elementLoadSet->getLoad(i);

                dfemcElementLoad->setValue(elementLoad->getValue());
                elementLoad->getLocalDirection(ex, ey, ez);
                dfemcElementLoad->setDirection(ex, ey, ez);

                for (j=0; j<elementLoad->getElementsSize(); j++)
                {
                    CFemElement* element = elementLoad->getElement(j);
                    dfemcElementLoad->addElement(element->getNumber());
                }

                dfemcElementLoadSet->next();
            }

            //
            // Start calculation
            //

            dfemcCalc->execute();

            //////////////////////////////////////////////////////
            // Retrieve calculation results
            //

            //
            // Retrieve node values
            //

            dfemcNodeSet->first();
            int valueSize = dfemcNode->getValueSize();
            m_maxNodeValue = -1.0e300;
            double nodeValue;

            for (i=0; i<nodeSet->getSize(); i++)
            {
                CFemNode* node = nodeSet->getNode(i);
                node->setValueSize(valueSize);
                for (j=1; j<=valueSize; j++)
                {
                    nodeValue = dfemcNode->getValue(j);
                    if (fabs(nodeValue)>m_maxNodeValue)
                        m_maxNodeValue = nodeValue;
                    node->setValue(j-1, nodeValue);
                }
                dfemcNodeSet->next();
            }

            //
            // Retrieve element values
            //

            dfemcElementSet->first();
            valueSize = dfemcElement->getValueSize();

            for (i=0; i<elementSet->getSize(); i++)
            {
                CFemBeam* element = (CFemBeam*) elementSet->getElement(i);
                element->setValueSize(valueSize);
                for (j=1; j<=valueSize; j++)
                    element->setValue(j-1, dfemcElement->getValue(j));
                dfemcElementSet->next();
            }

            //////////////////////////////////////////////////////
            // Release FEM system
            //

            dfemcSystem->remove();

            //////////////////////////////////////////////////////
            // Destroy orb
            //

            try {
                if (!CORBA::is_nil(orb))
                    orb->destroy();
            }
            catch (const CORBA::Exception& e)
            {
                cerr << "Uncaught CORBA exception: "
                     << e << endl;
            }

        }
        catch (const CORBA::Exception& e)
        {
            cerr << "Uncaught CORBA exception: "
                 << e << endl;
        }
    }
#endif
}

void CFemDFEMCInterface::setArguments(int argc, char **argv)
{
    m_argc = argc;
    m_argv = argv;
}

double CFemDFEMCInterface::getMaxNodeValue()
{
    return m_maxNodeValue;
}
