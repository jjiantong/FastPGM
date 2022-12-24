package tutorials;

import smile.*;

// Tutorial5 loads the XDSL file created by Tutorial4,
// then performs the series of inference calls,
// changing evidence each time.

public class Tutorial5 {
    public static void run() {
        System.out.println("Starting Tutorial5...");
        Network net = new Network();
        
        net.readFile("tutorial4.xdsl");

        System.out.println("No evidence set.");
        net.updateBeliefs();
        printFinancialGain(net);

        System.out.println("Setting Forecast=Good.");
        changeEvidenceAndUpdate(net, "Forecast", "Good");

        System.out.println("Adding Economy=Up");
        changeEvidenceAndUpdate(net, "Economy", "Up");
        
        System.out.println("Tutorial5 complete.");	
    }
    
    
    static void changeEvidenceAndUpdate(
            Network net, String nodeId, String outcomeId) {
        if (outcomeId != null) {
            net.setEvidence(nodeId, outcomeId);	
        } else {
            net.clearEvidence(nodeId);
        }
        
        net.updateBeliefs();
        printFinancialGain(net);
    }

    
    static void printFinancialGain(Network net) {
        double[] expectedUtility = net.getNodeValue("Gain");
        int[] utilParents = net.getValueIndexingParents("Gain");
        printGainMatrix(net, expectedUtility, utilParents);
    }
    
    
    static void printGainMatrix(Network net, double[] mtx, int[] parents) {
        int dimCount = 1 + parents.length;
        
        int[] dimSizes = new int[dimCount];
        for (int i = 0; i < dimCount - 1; i ++) {
            dimSizes[i] = net.getOutcomeCount(parents[i]);
        }
        dimSizes[dimSizes.length - 1] = 1;
        
        int[] coords = new int[dimCount];
        for (int elemIdx = 0; elemIdx < mtx.length; elemIdx ++) {
            indexToCoords(elemIdx, dimSizes, coords);
            
            System.out.print("    Utility(");
            
            if (dimCount > 1)
            {
                for (int parentIdx = 0; parentIdx < parents.length; parentIdx++)
                {
                    if (parentIdx > 0) System.out.print(",");
                    int parentHandle = parents[parentIdx];
                    System.out.printf("%s=%s", 
                        net.getNodeId(parentHandle), 
                        net.getOutcomeId(parentHandle, coords[parentIdx]));
                }
            }

            System.out.printf(")=%f\n", mtx[elemIdx]);
        }
        System.out.println();
    }

    
    static void indexToCoords(int index, int[] dimSizes, int[] coords) {
        int prod = 1;
        for (int i = dimSizes.length - 1; i >= 0; i --) {
            coords[i] = (index / prod) % dimSizes[i];
            prod *= dimSizes[i];
        }
    }

}
