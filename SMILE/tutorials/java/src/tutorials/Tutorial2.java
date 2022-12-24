package tutorials;

import smile.*;

// Tutorial2 loads the XDSL file created by Tutorial1,
// then performs the series of inference calls,
// changing evidence each time.

public class Tutorial2 {
    public static void run() {
        System.out.println("Starting Tutorial2...");
        Network net = new Network();
        
        // load the network created by Tutorial1
        net.readFile("tutorial1.xdsl");

        System.out.println("Posteriors with no evidence set:");
        net.updateBeliefs();
        printAllPosteriors(net);

        System.out.println("Setting Forecast=Good.");
        changeEvidenceAndUpdate(net, "Forecast", "Good");

        System.out.println("Adding Economy=Up.");
        changeEvidenceAndUpdate(net, "Economy", "Up");

        System.out.println("Changing Forecast to Poor, keeping Economy=Up.");
        changeEvidenceAndUpdate(net, "Forecast", "Poor");

        System.out.println(
            "Removing evidence from Economy, keeping Forecast=Poor.");
        changeEvidenceAndUpdate(net, "Economy", null);

        System.out.println("Tutorial2 complete.");
    }
    
    
    private static void printPosteriors(Network net, int nodeHandle) {
        String nodeId = net.getNodeId(nodeHandle);
        if (net.isEvidence(nodeHandle)) {
            System.out.printf("%s has evidence set (%s)\n", 
                nodeId, 
                net.getOutcomeId(nodeHandle, net.getEvidence(nodeHandle)));
        } else {
            double[] posteriors = net.getNodeValue(nodeHandle);
            for (int i = 0; i < posteriors.length; i ++) {
                System.out.printf("P(%s=%s)=%f\n", 
                    nodeId, 
                    net.getOutcomeId(nodeHandle, i),
                    posteriors[i]);
            }
        }
    }


    private static void printAllPosteriors(Network net) {
        for (int h = net.getFirstNode(); h >= 0; h = net.getNextNode(h)) {
            printPosteriors(net, h);
        }
        System.out.println();
    }

    
    private static void changeEvidenceAndUpdate(
            Network net, String nodeId, String outcomeId) {
        if (outcomeId != null) {
            net.setEvidence(nodeId, outcomeId);	
        } else {
            net.clearEvidence(nodeId);
        }
        
        net.updateBeliefs();
        printAllPosteriors(net);
    }
    
}
