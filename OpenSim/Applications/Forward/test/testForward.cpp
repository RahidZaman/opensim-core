/*
* Copyright (c)  2005, Stanford University. All rights reserved. 
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
* 
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
// forward.cpp
// author:  Frank C. Anderson, Ajay Seth

// INCLUDE
#include <string>
#include <iostream>
#include <sstream>
#include <OpenSim/Simulation/Control/Controller.h>
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/Model/LoadModel.h>
#include <OpenSim/Simulation/Model/AnalysisSet.h>
#include <OpenSim/Tools/ForwardTool.h>
#include <OpenSim/Analyses/Kinematics.h>
#include <OpenSim/Analyses/Actuation.h>
#include <OpenSim/Analyses/PointKinematics.h>
#include <OpenSim/Analyses/BodyKinematics.h>
#include <OpenSim/Auxiliary/auxiliaryTestFunctions.h>

using namespace OpenSim;
using namespace std;

void testPendulum();	// test manager/integration process
void testArm26();	// now add computation of controls and generation of muscle forces
void testGait2354();    //finally include applied ground reactions forces 
void testGait2354WithController();

int main() {
    try {
        testPendulum();	// test manager/integration process
		cout << "Pendulum test PASSED " << endl;

		testArm26();	// now add computation of controls and generation of muscle forces
		cout << "arm26 test PASSED " << endl;

		testGait2354();    //finally include applied ground reactions forces 
		cout << "gait2354 test PASSED " << endl;

		testGait2354WithController();
		cout << "gait2354 with correction controller test PASSED " << std::endl;
	}
    catch (const Exception& e) {
        e.print(cerr);
        return 1;
    }
    cout << "Done" << endl;
    return 0;
}

void testPendulum() {
	ForwardTool forward("setup_pend.xml");cout << 1 << endl;
	forward.run();cout << 2 << endl;
	forward.print("check.xml");
	Storage storage("Results/pendulum_states.sto");
	ASSERT(storage.getFirstTime() == 0.0);
	ASSERT(storage.getLastTime() == 1.0);
	ASSERT(storage.getSize() > 10);

	// Since the pendulum is only swinging through small angles, it should be very
	// close to a simple harmonic oscillator.

	double previousTime = -1.0;
	double k = sqrt(9.80665000/0.5);
	for (int j = 0; j < storage.getSize(); ++j) {
		StateVector* state = storage.getStateVector(j);
		double time = state->getTime();
		ASSERT(time > previousTime);
		previousTime = time;
		ASSERT_EQUAL(0.1*cos(k*time), state->getData()[0], 1.5e-3);
		ASSERT_EQUAL(0.0, state->getData()[1], 1e-5);
		ASSERT_EQUAL(0.0, state->getData()[2], 1e-5);
		ASSERT_EQUAL(-k*0.1*sin(k*time), state->getData()[3], 1.5e-3);
		ASSERT_EQUAL(0.0, state->getData()[4], 1e-5);
		ASSERT_EQUAL(0.0, state->getData()[5], 1e-5);
	}
	ASSERT(previousTime == 1.0);
}


void testArm26() {
    ForwardTool forward("arm26_Setup_Forward.xml");
	forward.run();
	forward.print("check.xml");
	Storage results("Results/arm26_states.sto");
	Storage* standard = new Storage();
    string statesFileName("std_arm26_states.sto");
    forward.loadStatesStorage( statesFileName, standard );
	StateVector* state = results.getStateVector(0);
	double time = state->getTime();
	Array<double> data;
	data.setSize(state->getSize());
	standard->getDataAtTime(time, state->getSize(), data);
	for (int j = 0; j < state->getSize(); ++j) {
		stringstream message;
		message << "t=" << time <<" state# "<< j << " " << standard->getColumnLabels()[j+1] << " std=" << data[j] <<"  computed=" << state->getData()[j];
        ASSERT_EQUAL(data[j], state->getData()[j], 2.5e-3, __FILE__, __LINE__, "ASSERT_EQUAL FAILED " + message.str());
		cout << "ASSERT_EQUAL PASSED " << message.str();
    }

	int i = results.getSize()-1;
    state = results.getStateVector(i);
    time = state->getTime();
	data.setSize(state->getSize());
	standard->getDataAtTime(time, state->getSize(), data);
	for (int j = 0; j < state->getSize(); ++j) {
        stringstream message;
		message << "t=" << time <<" state# "<< j << " " << standard->getColumnLabels()[j+1] << " std=" << data[j] <<"  computed=" << state->getData()[j];
        ASSERT_EQUAL(data[j], state->getData()[j], 2.5e-3, __FILE__, __LINE__, "ASSERT_EQUAL FAILED " + message.str());
		cout << "ASSERT_EQUAL PASSED " << message.str();
	}
}

void testGait2354() {
    //ExternalLoads ext;
	//ext.print("ExternalLoads.xml");
	ForwardTool forward("setup_gait2354.xml");
	forward.run();
	forward.print("check.xml");
	Storage results("Results/gait2354_states.sto");
	Storage* standard = new Storage();
    string statesFileName("std_gait2354_states.sto");
    forward.loadStatesStorage( statesFileName, standard );

	Array<double> data;
	StateVector* state = results.getStateVector(0);
	double time = state->getTime();
	data.setSize(state->getSize());
	standard->getDataAtTime(time, state->getSize(), data);

	// At initial time all states should be identical except for locked joints may vary slightly due to 
	// differences in OpenSim's integrator and SimTK's
	int nc = forward.getModel().getNumCoordinates();
	for (int j = 0; j < state->getSize(); ++j) {
		stringstream message;
		message << "t=" << time <<" state# "<< j << " " << standard->getColumnLabels()[j+1] << " std=" << data[j] <<"  computed=" << state->getData()[j];
        ASSERT_EQUAL(data[j], state->getData()[j], 5e-2, __FILE__, __LINE__, "ASSERT_EQUAL FAILED " + message.str());
		cout << "ASSERT_EQUAL PASSED " << message.str();
	}

	int i = results.getSize()-1;
	state = results.getStateVector(i);
	time = state->getTime();
	standard->getDataAtTime(time, state->getSize(), data);

	// NOTE: Gait model is running forward open-loop. We cannot expect all the states to
	// be "bang on" and we expect a gradual drift in the coordinates.  Check to see that
	// coordinates haven't drifted too far off.
	for (int j = 0; j < nc; ++j) {
	    stringstream message;
		message << "t=" << time <<" state# "<< j << " " << standard->getColumnLabels()[j+1] << " std=" << data[j] <<"  computed=" << state->getData()[j];
        ASSERT_EQUAL(data[j], state->getData()[j], 5e-1, __FILE__, __LINE__, "ASSERT_EQUAL FAILED " + message.str());
		cout << "ASSERT_EQUAL PASSED " << message.str();
	}
}

void testGait2354WithController() {
    ForwardTool forward("subject01_Setup_Forward_Controller.xml");
	forward.run();
	forward.print("check.xml");
	Storage results("ResultsSimpleFeedbackController/subject01_walk1_states.sto");
	Storage* standard = new Storage();
    string statesFileName("subject01_walk1_states.sto");
    forward.loadStatesStorage( statesFileName, standard );

	Array<double> data;
	int i = results.getSize() - 1;
	StateVector* state = results.getStateVector(i);
	double time = state->getTime();
	data.setSize(state->getSize());
	standard->getDataAtTime(time, state->getSize(), data);
	int nc = forward.getModel().getNumCoordinates();
	for (int j = 0; j < nc; ++j) {      
	    stringstream message;
		message << "t=" << time <<" state# "<< j << " " << standard->getColumnLabels()[j+1] << " std=" << data[j] <<"  computed=" << state->getData()[j];
        ASSERT_EQUAL(data[j], state->getData()[j], 5e-2, __FILE__, __LINE__, "ASSERT_EQUAL FAILED " + message.str());
		cout << "ASSERT_EQUAL PASSED " << message.str();
	}
}
