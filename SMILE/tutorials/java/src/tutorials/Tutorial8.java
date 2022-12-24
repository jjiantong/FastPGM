package tutorials;

import smile.*;

// Tutorial8 loads continuous model from the XDSL file written by Tutorial7,
// then adds discrete nodes to create a hybrid model. Inference is performed
// and model is saved to disk.

public class Tutorial8 {
    public static void run() {
        System.out.println("Starting Tutorial8...");
        Network net = new Network();
        
        net.readFile("tutorial7.xdsl");
        
        createCptNode(
        	net, "zone", "Climate Zone",
        	new String[] { "Temperate", "Desert" },
            60, 20);

        int toaHandle = net.getNode("toa");
        net.setNodeEquation(toaHandle, 
            "toa=If(zone=\"Desert\",Normal(22,5),Normal(11,10))");

        int perceivedHandle = createCptNode(
        	net, "perceived", "Perceived Temperature", 
        	new String[] { "Hot", "Warm", "Cold" },
            60, 300);
        net.addArc(toaHandle, perceivedHandle);
        
        double[] perceivedProbs = new double[] {
    		0,    // P(perceived=Hot |toa in -10..0)
            0.02, // P(perceived=Warm|toa in -10..0)
            0.98, // P(perceived=Cold|toa in -10..0)
            0.05, // P(perceived=Hot |toa in 0..10)
            0.15, // P(perceived=Warm|toa in 0..10)
            0.80, // P(perceived=Cold|toa in 0..10)
            0.10, // P(perceived=Hot |toa in 10..20)
            0.80, // P(perceived=Warm|toa in 10..20)
            0.10, // P(perceived=Cold|toa in 10..20)
            0.80, // P(perceived=Hot |toa in 20..30)
            0.15, // P(perceived=Warm|toa in 20..30)
            0.05, // P(perceived=Cold|toa in 20..30)
            0.98, // P(perceived=Hot |toa in 30..40)
            0.02, // P(perceived=Warm|toa in 30..40)
            0     // P(perceived=Cold|toa in 30..40)
        };
        net.setNodeDefinition(perceivedHandle, perceivedProbs);

        net.setEvidence("zone", "Temperate");
        System.out.println("Results in temperate zone:");
        updateAndShowStats(net);

        net.setEvidence("zone", "Desert");
        System.out.println("Results in desert zone:\n");
        updateAndShowStats(net);
        
        net.writeFile("tutorial8.xdsl");
        System.out.println(
            "Tutorial8 complete: Network written to tutorial8.xdsl");
    }

    
    static void showStats(Network net, int nodeHandle) {
		String nodeId = net.getNodeId(nodeHandle);
		
		if (net.isEvidence(nodeHandle)) {
			double v = net.getContEvidence(nodeHandle);
			System.out.printf("%s has evidence set (%g)\n", nodeId, v);
			return;
		} 
		
		if (net.isValueDiscretized(nodeHandle)) {
			System.out.printf("%s is discretized.\n", nodeId);
			DiscretizationInterval[] iv = 
                net.getNodeEquationDiscretization(nodeHandle);
			double[] bounds = net.getNodeEquationBounds(nodeHandle);
			double[] discBeliefs = net.getNodeValue(nodeHandle);
			double lo = bounds[0];
		    for (int i = 0; i < discBeliefs.length; i++) {
		        double hi = iv[i].boundary;
		        System.out.printf(
                    "\tP(%s in %g..%g)=%g\n", nodeId, lo, hi, discBeliefs[i]);
		        lo = hi;
		    }
		} else {
			double[] stats = net.getNodeSampleStats(nodeHandle);
	        System.out.printf("%s: mean=%g stddev=%g min=%g max=%g\n",
	            nodeId, stats[0], stats[1], stats[2], stats[3]);
		}
    }

   
    static void updateAndShowStats(Network net) {
        net.updateBeliefs();
        for (int h = net.getFirstNode(); h >= 0; h = net.getNextNode(h))
        {
            if (net.getNodeType(h) == Network.NodeType.EQUATION) {
            	showStats(net, h);
            }
        }
        System.out.println();
    }

    
    private static int createCptNode(
            Network net, String id, String name, 
            String[] outcomes, int xPos, int yPos) {
        int handle = net.addNode(Network.NodeType.CPT, id);
        
        net.setNodeName(handle, name);
        net.setNodePosition(handle, xPos, yPos, 85, 55);
        
        int initialOutcomeCount = net.getOutcomeCount(handle); 
        for (int i = 0; i < initialOutcomeCount; i ++) {
            net.setOutcomeId(handle, i, outcomes[i]);
        }
        
        for (int i = initialOutcomeCount; i < outcomes.length; i ++) {
            net.addOutcome(handle, outcomes[i]);
        }
        
        return handle;
    }

}
