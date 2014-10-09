/* -------------------------------------------------------------------------- *
 *                        OpenSim:  ModelComponent.cpp                        *
 * -------------------------------------------------------------------------- *
 * The OpenSim API is a toolkit for musculoskeletal modeling and simulation.  *
 * See http://opensim.stanford.edu and the NOTICE file for more information.  *
 * OpenSim is developed at Stanford University and supported by the US        *
 * National Institutes of Health (U54 GM072970, R24 HD065690) and by DARPA    *
 * through the Warrior Web program.                                           *
 *                                                                            *
 * Copyright (c) 2005-2013 Stanford University and the Authors                *
 * Author(s): Ajay Seth, Michael Sherman                                      *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0.         *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */

// INCLUDES
#include "OpenSim/Simulation/Model/ModelComponent.h"
#include "OpenSim/Simulation/Model/Model.h"

using namespace SimTK;

namespace OpenSim {

//==============================================================================
//                             MODEL COMPONENT
//==============================================================================
ModelComponent::ModelComponent() : _model(NULL) 
{
    constructProperty_GeometrySet();
}

ModelComponent::ModelComponent(const std::string& fileName, bool updFromXMLNode)
:   Component(fileName, updFromXMLNode), _model(NULL)
{
    constructProperty_GeometrySet();
}

ModelComponent::ModelComponent(SimTK::Xml::Element& element) 
:   Component(element), _model(NULL)
{
    constructProperty_GeometrySet();
}

const Model& ModelComponent::getModel() const
{
    if(!_model)
        throw Exception("ModelComponent::getModel(): component '" + getName() +
        "' of type " + getConcreteClassName() + " does not belong to a model. "
            "Have you called Model::initSystem()?"); 
    return *_model;
}

Model& ModelComponent::updModel()
{
    if(!_model)
        throw Exception("ModelComponent::updModel(): component '" + getName() +
        "' of type " + getConcreteClassName() + " does not belong to a model. "
            "Have you called Model::initSystem()?");
    return *_model;
}


void ModelComponent::extendConnect(Component &root)
{
    Super::extendConnect(root);
	Model* model = dynamic_cast<Model*>(&root);
	// Allow (model) component to include its own subcomponents
	// before calling the base method which automatically invokes
	// connect all the subcomponents.
	if (model)
		connectToModel(*model);
}


// Base class implementation of virtual method.
void ModelComponent::connectToModel(Model& model)
{
    _model = &model;
    extendConnectToModel(model);
}

// Base class implementation of virtual method.
void ModelComponent::generateDecorations
    (bool                                        fixed, 
    const ModelDisplayHints&                    hints,
    const SimTK::State&                         state,
    SimTK::Array_<SimTK::DecorativeGeometry>&   appendToThis) const 
{
    for(unsigned int i=0; i < _components.size(); i++){
		ModelComponent *mc = dynamic_cast<ModelComponent*>(_components[i]);
        mc->generateDecorations(fixed,hints,state,appendToThis);
	}
}

void ModelComponent::adoptGeometry(OpenSim::Geometry* geom) {
    append_GeometrySet(*geom);
    return;
}


const SimTK::DefaultSystemSubsystem& ModelComponent::
getDefaultSubsystem() const
{   return getModel().getDefaultSubsystem(); }

const SimTK::DefaultSystemSubsystem& ModelComponent::
updDefaultSubsystem()
{   return updModel().updDefaultSubsystem(); }

} // end of namespace OpenSim
