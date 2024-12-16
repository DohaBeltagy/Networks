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

#include "node0.h"

Define_Module(Node0);


// This Function is for the sender
void Node0::prepareFrame(CustomMessage_Base* sendingMessage, string input){
    string payload = preparePayload(input);

    string trailer = prepareTrailer(payload);

    sendingMessage->setPayload(payload.c_str());
    sendingMessage->setTrailer(trailer.c_str());
    sendingMessage->setType(2);
    // Set Seq number here too.

    return;
}
string Node0::preparePayload(string input){

    char flag = '$';
    char escape = '/';

    // We need to check whether we have a escape or a flag inside the input message
    // if we do we need to escape it.
    for(int i = 0; i < input.size(); i++){
        if(i > 0 && (input[i] == flag || input[i] == escape)){
            input.insert(i, "/");
            i++;
        }
    }

    return flag + input + flag;

}
string Node0::prepareTrailer(string payload){
    /* We want our Parity Check to be a string of bits eg '0000 1010'
       1- fastest way is to xor all characters
       2- get the asci of the character
       3- convert it to binary
       4- send it
    */


    char parity = 0;
    for(auto ch : payload) parity ^= ch;

    bitset<8> binary(parity);

    return binary.to_string();

}


void Node0::initialize()
{
    // TODO - Generated method body
    string input = "example";
    CustomMessage_Base* msg = new CustomMessage_Base("Test Message");
    prepareFrame(msg, input);
    EV << msg->getPayload() << endl;
    EV << msg->getTrailer() << endl;
    EV << msg->getType() << endl;

}

void Node0::handleMessage(cMessage *msg)
{
    // TODO - Generated method body


}
