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

#ifndef __PROJECT_V1_CORDINATOR_H_
#define __PROJECT_V1_CORDINATOR_H_

#include <omnetpp.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Cordinator : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
  private:
    void createNodes();
    pair<int, double> readCoordinatorFile();
    void initSender(int senderId, int startTime);
};

#endif
