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

void Node0::readFile(const int& fileId)
{
    string fileName = "input" + to_string(fileId) + ".txt";
    ifstream file(fileName);

    if (!file.is_open())
        throw cRuntimeError("Cannot open input file: %s", fileName.c_str());

    vector<std::pair<std::string, std::string>> messages;
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        string prefix = line.substr(0, 4);
        string message = line.substr(5);

        messages.push_back({prefix, message});
    }

    file.close();

    EV << "Read " << messages.size() << " messages from " << fileName << ":\n";
       for (const auto& msg : messages) {
           EV << "Prefix: " << msg.first << ", Message: " << msg.second << "\n";
    }

    this->nodeMessages = messages;
}


void Node0::initialize()
{
    // TODO - Generated method body
    // make 3 pointers, first, last, and current
    int WS = par("WS").intValue();
    this->front = 0;
    this->end = WS - 1;
    this->current = 0;
    EV << "Initializing node: " << this->getName() << " with ID: " << par("id").intValue() << endl;
}
void Node0::sendMessage(CustomMessage_Base* msg, double time){
    prepareFrame(msg, "hi");
    this->sendDelayed(msg, time, "port1$o");
}
void Node0::parityCheck(string message, string parity){
    char parityChar = static_cast<char>(bitset<8>(parity).to_ulong());
    for(auto ch: message) parityChar ^= ch;
    if (parityChar == 0){
        EV << "NO ERROR!" << endl;
    }else{
        EV << "ERROR Exists" << endl;
    }
}
void Node0::recieveMessage(CustomMessage_Base* msg){
    string payload = msg->getPayload();
    string trailer = msg->getTrailer();
    EV << "Payload: the message bits: " << endl;
    EV << payload << endl;
    EV << "Trailer: the parity check bits: " << endl;
    EV << trailer << endl;
    parityCheck(payload, trailer);

}

int Node0::circularIncremet(int index)
{
    int WS = par("WS").intValue();
    return index%WS;
}

void Node0::goBackN(int startTime)
{
    // extract the messages from the pairs
    // Extract the second string using std::transform
    std::vector<std::string> message(this->nodeMessages.size());
    std::transform(this->nodeMessages.begin(), this->nodeMessages.end(), message.begin(),[](const std::pair<std::string, std::string>& message) {
        return message.second;
    });

    for(int i = this->current; i<this->end; i++)
    {
        int id = par("id").intValue();
        string port = 'port1$o';
        if (id==0)
        {
           port = port0$o;
        }
        CustomMessage_Base* to_be_sent = new CustomMessage_Base();
        prepareFrame(to_be_sent, message[i]);
        this->current++;
        if(startTime == -1)
        {
            sendDelayed(to_be_sent, 0.5, port);
        }
        // this is only sent when this is the first time to sent
        // so we get the start time from the coordinator
        else
        {
            sendDelayed(to_be_sent, 0.5 + startTime, port);
        }
    }
}

void Node0::handleMessage(cMessage *msg)
{
        CustomMessage_Base* recievedMessage = dynamic_cast<CustomMessage_Base *>(msg);
        if(recievedMessage == nullptr)
        {
            string message = msg->getName();
            std::istringstream iss(message);
            int senderId;
            double startTime;
            iss >> senderId >> startTime;
            par("sender") = 1;
            EV << "Node " << this->getName() << " is now the sender and will start at " << startTime << " seconds.\n";
            EV << "The node id = "<< par("id").intValue() << endl;
            readFile(par("id").intValue());
            CustomMessage_Base* messageToBeSent = new CustomMessage_Base("Sender");
            // start the go back N function at the required time
            goBackN(startTime);
        }
        else
        {
            int sender = par("sender"); // get the sender
            // if sender == 1, then I am the sender
            if(sender == 1){

            }
            // else, i am the receiver
            else{
                recieveMessage(recievedMessage);


            }
        }
}
