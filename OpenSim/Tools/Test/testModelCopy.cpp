// testModelCopy.cpp
// Author:  Ayman Habib
/*
* Copyright (c) 2010, Stanford University. All rights reserved. 
* Use of the OpenSim software in source form is permitted provided that the following
* conditions are met:
* 	1. The software is used only for non-commercial research and education. It may not
*     be used in relation to any commercial activity.
* 	2. The software is not distributed or redistributed.  Software distribution is allowed 
*     only through https://simtk.org/home/opensim.
* 	3. Use of the OpenSim software or derivatives must be acknowledged in all publications,
*      presentations, or documents describing work in which OpenSim or derivatives are used.
* 	4. Credits to developers may not be removed from executables
*     created from modifications of the source.
* 	5. Modifications of source code must retain the above copyright notice, this list of
*     conditions and the following disclaimer. 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
*  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
*  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
*  SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
*  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR BUSINESS INTERRUPTION) OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
*  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>
#include <OpenSim/Common/IO.h>
#include <OpenSim/Common/Exception.h>
#include <OpenSim/Simulation/Manager/Manager.h>
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Common/LoadOpenSimLibrary.h>
#include <OpenSim/Auxiliary/auxiliaryTestFunctions.h>
#include "SimTKsimbody.h"
#include "SimTKmath.h"

using namespace OpenSim;
using namespace std;

void testCopyModel(string fileName);

int main()
{
	try {
		LoadOpenSimLibrary("osimActuators");
		testCopyModel("arm26.osim");
		testCopyModel("snowboard.osim");
		testCopyModel("Neck3dof_point_constraint.osim");
	}
	catch (const Exception& e) {
        e.print(cerr);
        return 1;
    }
    cout << "Done" << endl;
	return 0;
}

void testCopyModel(string fileName)
{
	Model *model = new Model(fileName);
	SimTK::State defaultState = model->initSystem();
	Model *modelCopy = new Model(*model);
	// At this point properties should all match. assert that
	ASSERT(*model==*modelCopy);
	delete model;
	SimTK::State& defaultStateOfCopy = modelCopy->initSystem();
	// Compare state
	defaultState.getY().dump("defaultState:Y");
	ASSERT ((defaultState.getY()-defaultStateOfCopy.getY()).norm() < 1e-7);
	defaultState.getZ().dump("defaultState:Z");
	ASSERT ((defaultState.getZ()-defaultStateOfCopy.getZ()).norm() < 1e-7);
	
	//  Now delete original model and make sure copy can stand
	Model *newModel = (Model*)modelCopy->copy();
	// Compare state again
	delete modelCopy;
	SimTK::State& defaultStateOfCopy2 = newModel->initSystem();
	// Compare state
	ASSERT ((defaultState.getY()-defaultStateOfCopy2.getY()).norm() < 1e-7);
	ASSERT ((defaultState.getZ()-defaultStateOfCopy2.getZ()).norm() < 1e-7);
	delete newModel;
}