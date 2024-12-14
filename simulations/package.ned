package project_v1.simulations;

import ned.IdealChannel;
import project_v1.Cordinator;
import project_v1.Node0;


@license(LGPL);
//
// TODO documentation
//
network Network
{
    @display("bgb=725,381");
    submodules:
        node0: Node0 {
            @display("p=150,117");
        }
        node1: Node0 {
            @display("p=507,117");
        }
        cordinator: Cordinator {
            @display("p=335,255");
        }
    connections:
        cordinator.port1 --> node0.port2;
        cordinator.port2 --> node1.port2;
        node1.port1 <--> node0.port1;
}

