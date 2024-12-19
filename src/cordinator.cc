//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "cordinator.h"

Define_Module(Cordinator);

pair<int, double> Cordinator::readCoordinatorFile()
{
    string fileName = "coordinator.txt";
    ifstream file(fileName);

    if (!file.is_open())
        throw cRuntimeError("Cannot open input file: %s", fileName.c_str());

    string line;
    getline(file, line);
    file.close();

    // Parse the line (assuming format: [Node_id starting_time])
    istringstream iss(line);
    int nodeId;
    double startingTime;
    iss >> nodeId >> startingTime;

    return {nodeId, startingTime};
}

void Cordinator::createNodes()
{
    for (int i = 0; i < 2; ++i)
    {
        // Get the module type for Node0
        cModuleType *nodeType = cModuleType::get("node0");

        if (!nodeType)
            throw cRuntimeError("Module type 'Node0' not found!");

        // Create the Node0 module
        string nodeName = "Node" + std::to_string(i);
        cModule *node = nodeType->create(nodeName.c_str(), this);

        // Set the parameters
        node->par("id") = i;
        node->par("sender") = 0; // Example: Default sender is Node0

        node->finalizeParameters();
        node->buildInside();

        this->gate("port2")->connectTo(node->gate("port2"));
        }
    }
}

void Cordinator::initialize()
{
    // TODO - Generated method body
}

void Cordinator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

