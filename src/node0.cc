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
void Node0::prepareFrame(CustomMessage_Base* sendingMessage, string input, int seqNumber){
    string payload = preparePayload(input);

    string trailer = prepareTrailer(payload);

    sendingMessage->setPayload(payload.c_str());
    sendingMessage->setTrailer(trailer.c_str());
    sendingMessage->setType(2);
    sendingMessage->setHeader(seqNumber);
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
// returns the sequence numebr
int Node0::circularIncremet(int index)
{
    int WS = par("WS").intValue();
    return index%WS;
}

void Node0::goBackN(int startTime)
{
    // extract the messages from the pairs
    // Extract the second string using std::transform
    int c = 0;
    for(int i = this->current; i < this->end; i++)
    {
        double extraTime = c * (par("PT").doubleValue());
        string port = "port1$o";
        int seqNumber = circularIncremet(i);
        this->current++;
        sendWithErrors(this->nodeMessages[i].second, this->nodeMessages[i].first, startTime + extraTime, seqNumber);
        cMessage* timeoutMessage = new cMessage("Timer");
        this->timeoutMap[seqNumber] = timeoutMessage;
        int timeout = par("TO").doubleValue();
        scheduleAt(simTime() + timeout, timeoutMessage);
        c++;
    }
}

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



string Node0::deframing(string payload)
{
    string deframed_payload = "";
    int j = 0;
    for(int i = 1; i < payload.size() - 1; i++){
        if((payload[i] == '/' && payload[i+1] == '/') || (payload[i] == '/' && payload[i+1] == '$'))
        {
            continue;
        }
        else
        {
            deframed_payload[j] = payload[i];
            j++;
        }
    }
    return deframed_payload;
}

void Node0::recieveMessage(CustomMessage_Base* msg){
    string payload = msg->getPayload();
    string trailer = msg->getTrailer();
    int seqNumber = msg->getHeader();
    string deframed_payload = deframe(payload);
    CustomMessage_Base* messageToBeSent = new CustomMessage_Base("Reciever");
    messageToBeSent->setHeader(seqNumber);
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
    double time = par("PT").doubleValue();
    EV << "Time at reciever : " << simTime() << endl;
    EV << "Processing Time : " << time << endl;
    this->scheduleAt(simTime() + time, messageToBeSent);

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

void Node0::sendWithErrors(string message, string errorCode, double startTime, int seqNumber){
    double delay = 0;

    double processingTime = par("PT").doubleValue();
    double duplicationDelay = par("DD").doubleValue();
    bool dup = false;
    CustomMessage_Base* msg = new CustomMessage_Base("Sender");
    prepareFrame(msg, message, seqNumber);
    if(errorCode[1] == '1'){
        return;
    }
    if(errorCode[3] == '1'){
        // Function add delay
        delay = getDelay();
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
    double sendingTime = startTime + processingTime + delay;
    scheduleAt(simTime() + sendingTime, msg);
    if(dup){
        this->duplicateMessage(msg, sendingTime + duplicationDelay);
    }

}
void Node0::handleAck(int ack){
    if(ack == 1){
        this->front++;
        if(end < this->nodeMessages.size())
        {
            this->end++;
        }
        for(int i = this->current; i < this->end; i++){
            int seqNumber = circularIncremet(i);
            this->current++;
            sendWithErrors(this->nodeMessages[i].second, this->nodeMessages[i].first, 0, seqNumber);
        }
    }else{
        this->retransmit();
    }
}
void Node0::retransmit(){
    this->current = this->front;
    int c = 0;
    for(int i = this->current; i < this->end; i++){
       double extraTime = c * (par("PT").doubleValue());
       int seqNumber = circularIncremet(i);
       this->current++;
       sendWithErrors(this->nodeMessages[i].second, this->nodeMessages[i].first, extraTime, seqNumber);
       cMessage* timeoutMessage = new cMessage("Timer");
       this->timeoutMap[seqNumber] = timeoutMessage;
       int timeout = par("TO").doubleValue();
       scheduleAt(simTime() + timeout, timeoutMessage);
       c++;
    }
}
void Node0::handleMessage(cMessage *msg)
{
        // Check if it's a self message
        if(msg->isSelfMessage()){
            string title = msg->getName();
            if(title == "Timeout"){
                this->retransmit();
                return;
            }
            double delay = par("TD").doubleValue();
            this->sendDelayed(msg, delay, "port1$o");
            return;
        }
        CustomMessage_Base* recievedMessage = dynamic_cast<CustomMessage_Base *>(msg);
        if(recievedMessage == nullptr){
            string message = msg->getName();
            std::istringstream iss(message);
            int senderId;
            double startTime;
            iss >> senderId >> startTime;
            readFile(senderId);

            par("sender") = 1;
            // I am the sender so I will send for the first time once i receive from the coordinator

            goBackN(startTime);

        }else{
            int sender = par("sender"); // get the sender
            // if sender == 1, then I am the sender
            if(sender == 1){
                int type = recievedMessage->getType();
                if(type == 1){
                    EV << "Ack at sender" << endl;
                    int seqNumber = recievedMessage->getHeader();

                    cancelEvent(this->timeoutMap[seqNumber]);
                    timeoutMap.erase(seqNumber);

                }else if(type == 2){
                    EV << "NACK at sender" << endl;
                }
            }
            // else, i am the receiver
            else{
                recieveMessage(recievedMessage);
                EV << recievedMessage->getPayload();

            }
        }
}
