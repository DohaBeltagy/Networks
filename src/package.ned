package project_v1;

import ned.IdealChannel;


@license(LGPL);
//
// TODO documentation
//
network Network
{
    @display("bgb=618,328");
    submodules:
        cordinator: Cordinator {
            @display("p=268,236");
        }
        node0: Node0 {
            parameters:
                id = 0;
                sender = 0;
            @display("p=125,98");
        }
        node1: Node0 {
            	id = 1;
                sender = 0;
            @display("p=408,86");
        }
    connections:
        cordinator.port1 --> IdealChannel --> node0.port2;
        cordinator.port2 --> IdealChannel --> node1.port2;
        node0.port1 <--> IdealChannel <--> node1.port1;
}

