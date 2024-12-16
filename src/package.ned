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
        node0: Node0 {
            @display("p=152,121");
        }
        node1: Node0 {
            @display("p=404,121");
        }
        cordinator: Cordinator {
            @display("p=268,236");
        }
    connections:
        node1.port1 <--> IdealChannel <--> node0.port1;
        cordinator.port1 --> IdealChannel --> node0.port2;
        cordinator.port2 --> IdealChannel --> node1.port2;
}

