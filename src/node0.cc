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
            
void Node0::sendMessage(CustomMessage_Base* msg){
    this->send(msg, "port1$o");
}

bool Node0::parityCheck(string message, string parity){
    char parityChar = static_cast<char>(bitset<8>(parity).to_ulong());
    for(auto ch: message) parityChar ^= ch;
    if (parityChar == 0){
        EV << "NO ERROR!" << endl;
        return true;
    }else{
        EV << "ERROR Exists" << endl;
        return false;
    }
}
void Node0::recieveMessage(CustomMessage_Base* msg){
    string payload = msg->getPayload();
    string trailer = msg->getTrailer();
    EV << "Payload: the message bits: " << endl;
    EV << payload << endl;
    EV << "Trailer: the parity check bits: " << endl;
    EV << trailer << endl;

    CustomMessage_Base* messageToBeSent = new CustomMessage_Base("Reciever");

    if(parityCheck(payload, trailer)){
        messageToBeSent->setType(1);
    }else{
        messageToBeSent->setType(2);
    }
    double randomVar = uniform(0, 1);
    double chance = par("LP").doubleValue();
    if(randomVar <= chance){
        EV << "Lost ACK" << endl;
        return;
    }
    EV << "ACK not Lost" << endl;
    double time = par("PT").doubleValue() + par("TD").doubleValue();
    this->scheduleAt(simTime() + time, messageToBeSent);

            }
        }
}
double Node0::getDelay(){
    return par("ED");
}

void Node0::duplicateMessage(CustomMessage_Base* msg, double time){
    CustomMessage_Base* duplicateMsg = new CustomMessage_Base("Sender_Duplicate");

    // Copy attributes manually
    duplicateMsg->setPayload(msg->getPayload());
    duplicateMsg->setTrailer(msg->getTrailer());
    duplicateMsg->setType(msg->getType());
    // Add other fields if needed

    // Schedule the duplicate message
    scheduleAt(simTime() + time, duplicateMsg);
}

void Node0::sendWithErrors(string message, string errorCode, double startTime){
    double delay = 0;

    double transmissionDelay = par("TD").doubleValue();
    double processingTime = par("PT").doubleValue();
    double duplicationDelay = par("DD").doubleValue();
    bool dup = false;
    CustomMessage_Base* msg = new CustomMessage_Base("Sender");
    prepareFrame(msg, message);
    if(errorCode[1] == '1'){
        return;
    }
    if(errorCode[3] == '1'){
        // Function add delay
        delay = getDelay();
        EV << "Did i get the delay " << delay << endl;
    }
    if(errorCode[0] == '1'){
       // Modify the bit
        string payload = msg->getPayload();
        payload[0] ^= (1 << 3);
        msg->setPayload(payload.c_str());

    }
    if(errorCode[2] == '1'){
        // set duplication to true;
        dup = true;
    }
    double sendingTime = startTime + processingTime + transmissionDelay + delay;
    scheduleAt(simTime() + sendingTime, msg);
    if(dup){
        this->duplicateMessage(msg, sendingTime + duplicationDelay);
    }

}

void Node0::handleMessage(cMessage *msg)
{
        // Check if it's a self message
        if(msg->isSelfMessage()){
            int sender = par("sender");
            this->send(msg, "port1$o");
            return;
        }
        CustomMessage_Base* recievedMessage = dynamic_cast<CustomMessage_Base *>(msg);
        if(recievedMessage == nullptr){
            string message = msg->getName();
            std::istringstream iss(message);
            int senderId;
            double startTime;
            iss >> senderId >> startTime;
            par("sender") = 1;
            EV << "Node " << this->getName() << " is now the sender and will start at " << startTime << " seconds.\n";
            cMessage *msg = new cMessage("timeout");
            // I am the sender so I will send for the first time once i receive from the coordinator
            string errorCode = "1000";
            EV << "The error code is: " << errorCode << endl;
            string messageToBeSent = "hello";
            sendWithErrors(messageToBeSent, errorCode, startTime);


        }else{
            int sender = par("sender"); // get the sender
            // if sender == 1, then I am the sender
            if(sender == 1){
                int type = recievedMessage->getType();
                if(type == 1){
                    EV << "Ack" << endl;
                }else if(type == 2){
                    EV << "NACK" << endl;
                }
            }
            // else, i am the receiver
            else{
                recieveMessage(recievedMessage);
                EV << recievedMessage->getPayload();

            }
        }
}
