/*

Copyright (c) 2005-2017, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef TESTIMMERSEDBOUNDARYFORCES_HPP_
#define TESTIMMERSEDBOUNDARYFORCES_HPP_

// Needed for test framework
#include "AbstractCellBasedTestSuite.hpp"

#include "CellsGenerator.hpp"
#include "CheckpointArchiveTypes.hpp"
#include "FileComparison.hpp"
#include "ImmersedBoundaryHoneycombMeshGenerator.hpp"
#include "NoCellCycleModel.hpp"
#include "SmartPointers.hpp"

// Immersed boundary forces tested in this test suite
#include "ImmersedBoundaryLinearInteractionForce.hpp"
#include "ImmersedBoundaryLinearMembraneForce.hpp"
#include "ImmersedBoundaryMorseInteractionForce.hpp"
#include "ImmersedBoundaryMorseMembraneForce.hpp"

// This test is never run in parallel
#include "FakePetscSetup.hpp"

#include "Debug.hpp"

class TestImmersedBoundaryForces : public AbstractCellBasedTestSuite
{
public:

    void TestImmersedBoundaryLinearInteractionForceMethods() throw (Exception)
    {
        // Create a small 1x1 mesh
        ImmersedBoundaryHoneycombMeshGenerator gen(1, 1, 3, 0.1, 0.3);
        ImmersedBoundaryMesh<2, 2>* p_mesh = gen.GetMesh();
        p_mesh->SetNumGridPtsXAndY(32);

        // Create a minimal cell population
        std::vector<CellPtr> cells;
        CellsGenerator<NoCellCycleModel, 2> cells_generator;
        cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumElements());
        ImmersedBoundaryCellPopulation<2> cell_population(*p_mesh, cells);
        cell_population.SetInteractionDistance(0.01);

        // Create two nodes and put them in a vector of pairs
        Node<2> node0(0, true, 0.1, 0.1);
        Node<2> node1(0, true, 0.2, 0.1);
        std::vector<std::pair<Node<2>*, Node<2>*> > node_pair;
        node_pair.push_back(std::pair<Node<2>*, Node<2>*>(&node0, &node1));

        // Put the nodes in different elements so force calculation is triggered
        node0.AddElement(0);
        node1.AddElement(1);


        node0.ClearAppliedForce();
        node1.ClearAppliedForce();

        PRINT_VECTOR(node0.rGetAppliedForce());

        ImmersedBoundaryLinearInteractionForce<2> force;
        force.AddImmersedBoundaryForceContribution(node_pair, cell_population);

        PRINT_VECTOR(node0.rGetAppliedForce());
    }

    void TestArchivingOfImmersedBoundaryLinearInteractionForce() throw (Exception)
    {
        EXIT_IF_PARALLEL; // Beware of processes overwriting the identical archives of other processes
        OutputFileHandler handler("archive", false);
        std::string archive_filename = handler.GetOutputDirectoryFullPath() + "ImmersedBoundaryLinearInteractionForce.arch";

        {
            ImmersedBoundaryLinearInteractionForce<2> force;

            std::ofstream ofs(archive_filename.c_str());
            boost::archive::text_oarchive output_arch(ofs);

            // Set member variables
            force.SetSpringConst(1.23);
            force.SetRestLength(2.34);
            force.SetLaminaSpringConstMult(3.45);
            force.SetLaminaRestLengthMult(4.56);

            // Serialize via pointer to most abstract class possible
            AbstractImmersedBoundaryForce<2>* const p_force = &force;
            output_arch << p_force;
        }

        {
            AbstractImmersedBoundaryForce<2>* p_force;

            // Create an input archive
            std::ifstream ifs(archive_filename.c_str(), std::ios::binary);
            boost::archive::text_iarchive input_arch(ifs);

            // Restore from the archive
            input_arch >> p_force;
            ImmersedBoundaryLinearInteractionForce<2>* p_derived_force = static_cast<ImmersedBoundaryLinearInteractionForce<2>*>(p_force);

            // Check member variables have been correctly archived
            TS_ASSERT_DELTA(p_derived_force->GetSpringConst(), 1.23, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetRestLength(), 2.34, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetLaminaSpringConstMult(), 3.45, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetLaminaRestLengthMult(), 4.56, 1e-6);

            // Tidy up
            delete p_force;
        }
    }

    void TestImmersedBoundaryMorseInteractionForceMethods() throw (Exception)
    {
        ///\todo Test this class
    }

    void TestArchivingOfImmersedBoundaryMorseInteractionForce() throw (Exception)
    {
        EXIT_IF_PARALLEL; // Beware of processes overwriting the identical archives of other processes
        OutputFileHandler handler("archive", false);
        std::string archive_filename = handler.GetOutputDirectoryFullPath() + "ImmersedBoundaryLinearInteractionForce.arch";

        {
            ImmersedBoundaryMorseInteractionForce<2> force;

            std::ofstream ofs(archive_filename.c_str());
            boost::archive::text_oarchive output_arch(ofs);

            // Set member variables
            force.SetWellDepth(1.23);
            force.SetRestLength(2.34);
            force.SetLaminaWellDepthMult(3.45);
            force.SetLaminaRestLengthMult(4.56);
            force.SetWellWidth(5.67);

            // Serialize via pointer to most abstract class possible
            AbstractImmersedBoundaryForce<2>* const p_force = &force;
            output_arch << p_force;
        }

        {
            AbstractImmersedBoundaryForce<2>* p_force;

            // Create an input archive
            std::ifstream ifs(archive_filename.c_str(), std::ios::binary);
            boost::archive::text_iarchive input_arch(ifs);

            // Restore from the archive
            input_arch >> p_force;
            ImmersedBoundaryMorseInteractionForce<2>* p_derived_force = static_cast<ImmersedBoundaryMorseInteractionForce<2>*>(p_force);

            // Check member variables have been correctly archived
            TS_ASSERT_DELTA(p_derived_force->GetWellDepth(), 1.23, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetRestLength(), 2.34, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetLaminaWellDepthMult(), 3.45, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetLaminaRestLengthMult(), 4.56, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetWellWidth(), 5.67, 1e-6);

            // Tidy up
            delete p_force;
        }
    }

    void TestImmersedBoundaryLinearMembraneForce() throw (Exception)
    {
        ///\todo Test this class
    }

    void TestArchivingOfImmersedBoundaryLinearMembraneForce() throw (Exception)
    {
        EXIT_IF_PARALLEL; // Beware of processes overwriting the identical archives of other processes
        OutputFileHandler handler("archive", false);
        std::string archive_filename = handler.GetOutputDirectoryFullPath() + "ImmersedBoundaryLinearMembraneForce.arch";

        {
            ImmersedBoundaryLinearMembraneForce<2> force;

            std::ofstream ofs(archive_filename.c_str());
            boost::archive::text_oarchive output_arch(ofs);

            // Set member variables
            force.SetElementSpringConst(1.23);
            force.SetElementRestLength(2.34);
            force.SetLaminaSpringConst(3.45);
            force.SetLaminaRestLength(4.56);

            // Serialize via pointer to most abstract class possible
            AbstractImmersedBoundaryForce<2>* const p_force = &force;
            output_arch << p_force;
        }

        {
            AbstractImmersedBoundaryForce<2>* p_force;

            // Create an input archive
            std::ifstream ifs(archive_filename.c_str(), std::ios::binary);
            boost::archive::text_iarchive input_arch(ifs);

            // Restore from the archive
            input_arch >> p_force;
            ImmersedBoundaryLinearMembraneForce<2>* p_derived_force = static_cast<ImmersedBoundaryLinearMembraneForce<2>*>(p_force);

            // Check member variables have been correctly archived
            TS_ASSERT_DELTA(p_derived_force->GetElementSpringConst(), 1.23, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetElementRestLength(), 2.34, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetLaminaSpringConst(), 3.45, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetLaminaRestLength(), 4.56, 1e-6);

            // Tidy up
            delete p_force;
        }
    }

    void TestImmersedBoundaryMorseMembraneForce() throw (Exception)
    {
        ///\todo Test this class
    }

    void TestArchivingOfImmersedBoundaryMorseMembraneForce() throw (Exception)
    {
        EXIT_IF_PARALLEL; // Beware of processes overwriting the identical archives of other processes
        OutputFileHandler handler("archive", false);
        std::string archive_filename = handler.GetOutputDirectoryFullPath() + "ImmersedBoundaryMorseMembraneForce.arch";

        {
            ImmersedBoundaryMorseMembraneForce<2> force;

            std::ofstream ofs(archive_filename.c_str());
            boost::archive::text_oarchive output_arch(ofs);

            // Set member variables
            force.SetElementWellDepth(1.23);
            force.SetElementRestLength(2.34);
            force.SetLaminaWellDepth(3.45);
            force.SetLaminaRestLength(4.56);
            force.SetWellWidth(5.67);

            // Serialize via pointer to most abstract class possible
            AbstractImmersedBoundaryForce<2>* const p_force = &force;
            output_arch << p_force;
        }

        {
            AbstractImmersedBoundaryForce<2>* p_force;

            // Create an input archive
            std::ifstream ifs(archive_filename.c_str(), std::ios::binary);
            boost::archive::text_iarchive input_arch(ifs);

            // Restore from the archive
            input_arch >> p_force;
            ImmersedBoundaryMorseMembraneForce<2>* p_derived_force = static_cast<ImmersedBoundaryMorseMembraneForce<2>*>(p_force);

            // Check member variables have been correctly archived
            TS_ASSERT_DELTA(p_derived_force->GetElementWellDepth(), 1.23, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetElementRestLength(), 2.34, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetLaminaWellDepth(), 3.45, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetLaminaRestLength(), 4.56, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetLaminaRestLength(), 4.56, 1e-6);
            TS_ASSERT_DELTA(p_derived_force->GetWellWidth(), 5.67, 1e-6);

            // Tidy up
            delete p_force;
        }
    }

    void TestImmersedBoundaryForceOutputParameters()
    {
        EXIT_IF_PARALLEL;
        std::string output_directory = "TestForcesOutputParameters";
        OutputFileHandler output_file_handler(output_directory, false);

        // Test with ImmersedBoundaryLinearInteractionForce
        {
            ImmersedBoundaryLinearInteractionForce<2> cell_cell_force;
            cell_cell_force.SetSpringConst(1.23);
            cell_cell_force.SetRestLength(2.34);
            cell_cell_force.SetLaminaSpringConstMult(3.45);
            cell_cell_force.SetLaminaRestLengthMult(4.56);

            TS_ASSERT_EQUALS(cell_cell_force.GetIdentifier(), "ImmersedBoundaryLinearInteractionForce-2");

            out_stream cell_cell_force_parameter_file = output_file_handler.OpenOutputFile("ib_linear_int.parameters");
            cell_cell_force.OutputImmersedBoundaryForceParameters(cell_cell_force_parameter_file);
            cell_cell_force_parameter_file->close();

            {
                FileFinder generated_file = output_file_handler.FindFile("ib_linear_int.parameters");
                FileFinder reference_file("cell_based/test/data/TestForces/ib_linear_int.parameters",
                                          RelativeTo::ChasteSourceRoot);
                FileComparison comparer(generated_file, reference_file);
                TS_ASSERT(comparer.CompareFiles());
            }
        }

        // Test with ImmersedBoundaryMorseInteractionForce
        {
            ImmersedBoundaryMorseInteractionForce<2> cell_cell_force;
            cell_cell_force.SetWellDepth(1.23);
            cell_cell_force.SetRestLength(2.34);
            cell_cell_force.SetLaminaWellDepthMult(3.45);
            cell_cell_force.SetLaminaRestLengthMult(4.56);
            cell_cell_force.SetWellWidth(5.67);

            TS_ASSERT_EQUALS(cell_cell_force.GetIdentifier(), "ImmersedBoundaryMorseInteractionForce-2");

            out_stream cell_cell_force_parameter_file = output_file_handler.OpenOutputFile("ib_morse_int.parameters");
            cell_cell_force.OutputImmersedBoundaryForceParameters(cell_cell_force_parameter_file);
            cell_cell_force_parameter_file->close();

            {
                FileFinder generated_file = output_file_handler.FindFile("ib_morse_int.parameters");
                FileFinder reference_file("cell_based/test/data/TestForces/ib_morse_int.parameters",
                                          RelativeTo::ChasteSourceRoot);
                FileComparison comparer(generated_file, reference_file);
                TS_ASSERT(comparer.CompareFiles());
            }
        }

        // Test with ImmersedBoundaryLinearMembraneForce
        {
            ImmersedBoundaryLinearMembraneForce<2> membrane_force;
            membrane_force.SetElementSpringConst(5.67);
            membrane_force.SetElementRestLength(6.78);
            membrane_force.SetLaminaSpringConst(7.89);
            membrane_force.SetLaminaRestLength(8.91);

            TS_ASSERT_EQUALS(membrane_force.GetIdentifier(), "ImmersedBoundaryLinearMembraneForce-2");

            out_stream membrane_force_parameter_file = output_file_handler.OpenOutputFile("ib_linear_mem.parameters");
            membrane_force.OutputImmersedBoundaryForceParameters(membrane_force_parameter_file);
            membrane_force_parameter_file->close();

            {
                FileFinder generated_file = output_file_handler.FindFile("ib_linear_mem.parameters");
                FileFinder reference_file("cell_based/test/data/TestForces/ib_linear_mem.parameters",
                                          RelativeTo::ChasteSourceRoot);
                FileComparison comparer(generated_file, reference_file);
                TS_ASSERT(comparer.CompareFiles());
            }
        }

        // Test with ImmersedBoundaryMorseMembraneForce
        {
            ImmersedBoundaryMorseMembraneForce<2> membrane_force;
            membrane_force.SetElementWellDepth(1.23);
            membrane_force.SetElementRestLength(2.34);
            membrane_force.SetLaminaWellDepth(3.45);
            membrane_force.SetLaminaRestLength(4.56);
            membrane_force.SetWellWidth(5.67);

            TS_ASSERT_EQUALS(membrane_force.GetIdentifier(), "ImmersedBoundaryMorseMembraneForce-2");

            out_stream membrane_force_parameter_file = output_file_handler.OpenOutputFile("ib_morse_mem.parameters");
            membrane_force.OutputImmersedBoundaryForceParameters(membrane_force_parameter_file);
            membrane_force_parameter_file->close();

            {
                FileFinder generated_file = output_file_handler.FindFile("ib_morse_mem.parameters");
                FileFinder reference_file("cell_based/test/data/TestForces/ib_morse_mem.parameters",
                                          RelativeTo::ChasteSourceRoot);
                FileComparison comparer(generated_file, reference_file);
                TS_ASSERT(comparer.CompareFiles());
            }
        }
    }
};

#endif /*TESTIMMERSEDBOUNDARYFORCES_HPP_*/