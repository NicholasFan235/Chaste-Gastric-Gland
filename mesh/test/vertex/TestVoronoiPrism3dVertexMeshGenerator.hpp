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

#ifndef TESTVORONOIPRISM3DVERTEXMESHGENERATOR_HPP_
#define TESTVORONOIPRISM3DVERTEXMESHGENERATOR_HPP_

#include <cxxtest/TestSuite.h>

#include "VoronoiPrism3dVertexMeshGenerator.hpp"

#include "PetscSetupAndFinalize.hpp"

#include "RandomNumberGenerator.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;

//a shortcut for the lazy me
#define RESEED                                                             \
    RandomNumberGenerator* p_rand_gen = RandomNumberGenerator::Instance(); \
    p_rand_gen->Reseed(0);

class TestVoronoiPrism3dVertexMeshGenerator : public CxxTest::TestSuite
{
public:
    void TestSimplestMeshForRelaxation() throw(Exception)
    {
        //test to visualise relaxation
        const std::vector<unsigned> relax_steps = list_of(0)(2)(6)(15);
        const unsigned x = 10;
        const unsigned y = 10;

        for (unsigned index = 0; index < relax_steps.size(); ++index)
        {
            RESEED;
            unsigned relax_step = relax_steps[index];
            VoronoiPrism3dVertexMeshGenerator generator(x, y, 1, relax_step);
            MutableVertexMesh<3, 3>* p_mesh = generator.GetMesh();

            VertexMeshWriter<3, 3> vertex_mesh_writer("TestVoronoiPrism3dVertexMesh", "10x10 relaxation step: "
                                                          + boost::lexical_cast<std::string>(relax_step),
                                                      false);
            vertex_mesh_writer.WriteVtkUsingMeshWithCellId(*p_mesh);
        }
    }

    void TestSimpleMesh() throw(Exception)
    {
        // Generate a mesh that is 20 cells wide in x, 12 cells wide in y, 2 unit high in Z,
        // with 4 Lloyd's relaxation steps and target average element apical area 1.23
        RESEED;
        VoronoiPrism3dVertexMeshGenerator generator(20, 12, 2, 4, 1.23);
        MutableVertexMesh<3, 3>* p_mesh = generator.GetMesh();

        TS_ASSERT_EQUALS(p_mesh->GetNumNodes(), 1100u);
        TS_ASSERT_EQUALS(p_mesh->GetNumElements(), 240u);

        // Check average cell area is correct
        double average_voloume = 0.0;
        for (unsigned elem_idx = 0; elem_idx < p_mesh->GetNumElements(); elem_idx++)
        {
            average_voloume += p_mesh->GetVolumeOfElement(elem_idx);
        }
        average_voloume /= double(p_mesh->GetNumElements());

        TS_ASSERT_DELTA(average_voloume, 1.23 * 2, 1e-6);

        VertexMeshWriter<3, 3> vertex_mesh_writer("TestVoronoiPrism3dVertexMesh",
                                                  "20x12x1 4relax ApicalArea1.23", false);
        vertex_mesh_writer.WriteVtkUsingMeshWithCellId(*p_mesh);
    }

    void TestBoundaryNodes() throw(Exception)
    {
        // Generate a mesh that is 3 cells wide in x, 2 cells wide in y, 1 unit high in z, with 3 Lloyd's
        // relaxation steps and target average element area 100.0
        RESEED;
        VoronoiPrism3dVertexMeshGenerator generator(3, 2, 5, 3, 100.0);
        MutableVertexMesh<3, 3>* p_mesh = generator.GetMesh();

        // Check basic mesh properties are correct
        TS_ASSERT_EQUALS(p_mesh->GetNumNodes(), 46u);
        TS_ASSERT_EQUALS(p_mesh->GetNumElements(), 6u);

        // Check if all nodes are boundary nodes
        unsigned num_boundary_nodes = 0;
        for (unsigned node_idx = 0; node_idx < p_mesh->GetNumNodes(); ++node_idx)
        {
            if (p_mesh->GetNode(node_idx)->IsBoundaryNode())
            {
                num_boundary_nodes++;
            }
        }
        TS_ASSERT_EQUALS(num_boundary_nodes, 40u);

        TS_ASSERT_EQUALS(p_mesh->GetNode(0)->IsBoundaryNode(), false);
        TS_ASSERT_EQUALS(p_mesh->GetNode(11)->IsBoundaryNode(), false);
        TS_ASSERT_EQUALS(p_mesh->GetNode(13)->IsBoundaryNode(), false);
        TS_ASSERT_EQUALS(p_mesh->GetNode(23)->IsBoundaryNode(), false);
        TS_ASSERT_EQUALS(p_mesh->GetNode(34)->IsBoundaryNode(), false);
        TS_ASSERT_EQUALS(p_mesh->GetNode(36)->IsBoundaryNode(), false);
    }

    void TestConstructorExceptions() throw(Exception)
    {
        // Throws because first parameter < 2
        TS_ASSERT_THROWS_THIS(VoronoiPrism3dVertexMeshGenerator generator(1, 9, 2, 1.23),
                              "Need at least 2 by 2 cells");

        // Throws because second parameter < 2
        TS_ASSERT_THROWS_THIS(VoronoiPrism3dVertexMeshGenerator generator(9, 1, 2, 1.23),
                              "Need at least 2 by 2 cells");

        //Throws because third parameter <= 0.0
        TS_ASSERT_THROWS_THIS(VoronoiPrism3dVertexMeshGenerator generator(9, 9, -2, 1.23),
                              "Specified element height must be strictly positive");

        // Throws because fifth parameter <= 0.0
        TS_ASSERT_THROWS_THIS(VoronoiPrism3dVertexMeshGenerator generator(9, 9, 2, 4, -1.23),
                              "Specified target apical area must be strictly positive");
    }

    void TestGetPolygonDistributionAndAreaVariation() throw(Exception)
    {
        RESEED;
        unsigned num_x = 3;
        unsigned num_y = 4;
        double height_z = 5;
        unsigned num_relaxation_steps = 1;
        double apical_area = 1.23;

        VoronoiPrism3dVertexMeshGenerator generator(num_x, num_y, height_z, num_relaxation_steps, apical_area);
        MutableVertexMesh<3, 3>* p_mesh = generator.GetMesh();

        VertexMeshWriter<3, 3> vertex_mesh_writer("TestVoronoiPrism3dVertexMesh",
                                                  "ForPolygonDistribution", false);
        vertex_mesh_writer.WriteVtkUsingMeshWithCellId(*p_mesh);

        // Get the polgyon distribution and check it
        std::vector<double> polygon_dist = generator.GetPolygonDistribution();

        TS_ASSERT(polygon_dist.size() > 0);

        double cumulative_proportion = 0.0;
        for (unsigned poly_idx = 0; poly_idx < polygon_dist.size(); poly_idx++)
        {
            cumulative_proportion += polygon_dist[poly_idx];
        }

        TS_ASSERT_DELTA(cumulative_proportion, 1.0, 1e-6);

        // Get the area variation coefficient and check it
        double area_variation = generator.GetApicalAreaCoefficientOfVariation();
        TS_ASSERT_DELTA(area_variation, 0.263668, 1e-6);

        p_rand_gen->Reseed(0);
        VoronoiPrism3dVertexMeshGenerator generator2(num_x, num_y, height_z + 5, num_relaxation_steps, apical_area);

        TS_ASSERT_DELTA(area_variation, generator2.GetApicalAreaCoefficientOfVariation(), 1e-6);
    }

    void TestSetAndGetMethods() throw(Exception)
    {
        unsigned num_x = 3;
        unsigned num_y = 4;
        double height_z = 5;
        unsigned num_relaxation_steps = 1;
        double area = 1.23;

        VoronoiPrism3dVertexMeshGenerator generator(num_x, num_y, height_z, num_relaxation_steps, area);

        generator.SetMaxExpectedNumSidesPerPolygon(5);
        TS_ASSERT_EQUALS(generator.GetMaxExpectedNumSidesPerPolygon(), 5u);
    }

    void TestDummyClassCoverage()
    {
#if BOOST_VERSION < 105200

        TS_ASSERT_THROWS_THIS(VoronoiPrism3dVertexMeshGenerator generator,
                              "This is a dummy class. Build with Boost version 1.52 or above for functionality.");
        WARNING("Build with Boost version 1.52 or above for functionality.");
#endif // BOOST_VERSION < 105200
    }
};

#endif /*TESTVORONOIPRISM3DVERTEXMESHGENERATOR_HPP_*/