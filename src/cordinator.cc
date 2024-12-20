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

    istringstream iss(line);
    char temp;
    int nodeId;
    double startingTime;
    iss >> temp >> nodeId >> temp >> startingTime;

    return {nodeId, startingTime};
}



void Cordinator::initialize()
{
    // TODO - Generated method body
    auto config = readCoordinatorFile();
    string info = to_string(config.first) + " " + to_string(config.second);

    cMessage *startMsg = new cMessage(info.c_str());
    EV << "The info to be sent is  : " << info << endl;

    if(config.first == 1){
        send(startMsg, "port2");
    }else{
        send(startMsg, "port1");
    }


}

void Cordinator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

