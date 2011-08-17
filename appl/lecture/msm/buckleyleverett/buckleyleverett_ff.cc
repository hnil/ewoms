/*****************************************************************************
 *   Copyright (C) 2010 by Markus Wolff, Klaus Mosthaf                       *
 *   Copyright (C) 2007-2008 by Bernd Flemisch                               *
 *   Copyright (C) 2008-2009 by Andreas Lauser                               *
 *   Institute of Hydraulic Engineering                                      *
 *   University of Stuttgart, Germany                                        *
 *   email: <givenname>.<name>@iws.uni-stuttgart.de                          *
 *                                                                           *
 *   This program is free software: you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation, either version 2 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 *****************************************************************************/
#include "config.h"

#include "buckleyleverettproblem.hh"

#include <dune/grid/common/gridinfo.hh>

#include <dune/common/exceptions.hh>
#include <dune/common/mpihelper.hh>
#include <dune/common/parametertreeparser.hh>

#include <iostream>
#include <boost/format.hpp>

#include <iomanip>

//#include "dumux/decoupled/2p/old_fractionalflow/variableclass2p.hh"
//#include "dumux/decoupled/2p/old_fractionalflow/define2pmodel.hh"
//#include "buckleyleverettproblem.hh"
//#include "impes_buckleyleverett_analytic.hh"
//#include "dumux/timedisc/expliciteulerstep.hh"
//#include "dumux/timedisc/timeloop.hh"

void usage(const char *progname)
{
    std::cout << boost::format("usage: %s InputFileName\n")%progname;
    exit(1);
}


////////////////////////
// the main function
////////////////////////
int main(int argc, char** argv)
{
    try
    {
        typedef TTAG(BuckleyLeverettProblem) TypeTag;
        typedef GET_PROP_TYPE(TypeTag, PTAG(Scalar)) Scalar;
        typedef GET_PROP_TYPE(TypeTag, PTAG(Grid)) Grid;
        typedef GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;
        typedef GET_PROP_TYPE(TypeTag, PTAG(TimeManager)) TimeManager;
        typedef Dune::FieldVector<Scalar, Grid::dimensionworld> GlobalPosition;
        typedef typename GET_PROP(TypeTag, PTAG(ParameterTree)) Params;

        static const int dim = Grid::dimension;

        // initialize MPI, finalize is done automatically on exit
        Dune::MPIHelper::instance(argc, argv);

        ////////////////////////////////////////////////////////////
        // parse the command line arguments
        ////////////////////////////////////////////////////////////
        if (argc != 2)
            usage(argv[0]);

        std::string inputFileName;
        inputFileName = argv[1];


        ////////////////////////////////////////////////////////////
        // Read Input file
        ////////////////////////////////////////////////////////////

        Dune::ParameterTreeParser::readINITree(inputFileName, Params::tree());

        double discretizationLength = Params::tree().get<double>("Geometry.discretizationLength");

        // define the problem dimensions
        Dune::FieldVector<Scalar, dim> lowerLeft(0);
        Dune::FieldVector<Scalar, dim> upperRight(300);
        upperRight[1] = 75;

        int cellNumberX = static_cast<int>(300/discretizationLength);
//        int cellNumberY = static_cast<int>(75/discretizationLength);
        int cellNumberY = 1;

        Dune::FieldVector<int, dim> cellNumbers(cellNumberX);
        cellNumbers[1] = cellNumberY;

        double tEnd = Params::tree().get<double>("Problem.tEnd");
        double dt = tEnd;

        // create a grid object
        Grid grid(cellNumbers, lowerLeft, upperRight);
        Dune::gridinfo(grid);

        ////////////////////////////////////////////////////////////
        // instantiate and run the concrete problem
        ////////////////////////////////////////////////////////////

        typedef GET_PROP_TYPE(TypeTag, PTAG(WettingPhase)) WettingPhase;
        typedef GET_PROP_TYPE(TypeTag, PTAG(NonwettingPhase)) NonwettingPhase;

        WettingPhase::Component::setViscosity(Params::tree().get<double>("Fluid.viscosityW"));
        NonwettingPhase::Component::setViscosity(Params::tree().get<double>("Fluid.viscosityNW"));

        WettingPhase::Component::setDensity(Params::tree().get<double>("Fluid.densityW"));
        NonwettingPhase::Component::setDensity(Params::tree().get<double>("Fluid.densityNW"));

        TimeManager timeManager;
        Problem problem(timeManager, grid.leafView(), lowerLeft, upperRight);

        timeManager.init(problem, 0, dt, tEnd);
        timeManager.run();
        return 0;
    }
    catch (Dune::Exception &e) {
        std::cerr << "Dune reported error: " << e << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception thrown!\n";
        throw;
    }

// OLD STUFF -> PROBLEM
//        // IMPES parameters
//        int iterFlag = 0;
//        int nIter = 30;
//        double maxDefect = 1e-5;
//
//        // plotting parameters
//        const char* fileName = "buckleyleverett";
//        int modulo = 1;
//
//        typedef Dumux::FVVelocity2P<GridView, Scalar, VariableType,
//                ProblemType> DiffusionType;
//        DiffusionType diffusion(gridView, problem, modelDef);
//
//        typedef Dumux::FVSaturation2P<GridView, Scalar, VariableType,
//                ProblemType> TransportType;
//        TransportType transport(gridView, problem, modelDef);
//
//        typedef Dune::IMPESBLAnalytic<GridView, DiffusionType, TransportType,
//                VariableType> IMPESType;
//        IMPESType impes(diffusion, transport, iterFlag, nIter, maxDefect);
//        Dumux::ExplicitEulerStep<GridType, IMPESType> timestep;
//        Dumux::TimeLoop<GridView, IMPESType> timeloop(gridView, tStart, tEnd, fileName,
//                modulo, cFLFactor, maxDt, firstDt, timestep);
//        Dune::Timer timer;
//        timer.reset();
//        timeloop.execute(impes, false);
//        std::cout << "timeloop.execute took " << timer.elapsed() << " seconds"
//                << std::endl;
//        //printvector(std::cout, *fractionalflow, "saturation", "row", 200, 1);
}
