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

#ifndef _PROJECT_V1_NODE0_H
#define _PROJECT_V1_NODE0_H

#include <omnetpp.h>
#include <string>
#include "CustomMessage_m.h"
#include <bitset>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace omnetpp;
using namespace std;
/**
 * TODO - Generated class
 */
class Node0 : public cSimpleModule
{
private:
    vector<std::pair<std::string, std::string>> nodeMessages;
    int front;
    int end;
    int current;

    void prepareFrame(CustomMessage_Base* sendingMessage, string input);
    string preparePayload(string input);
    string prepareTrailer(string payload);
    void sendMessage(CustomMessage_Base* msg, double time);
    void recieveMessage(CustomMessage_Base* msg);
    void parityCheck(string message, string parity);
    void readFile(const int& fileId);
    void circularIncremet(int header);
    void goBackN(int startTime);

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
