package tutorials;

import smile.*;

// Tutorial7 creates a network with three equation-based nodes
// performs the inference, then saves the model to disk.

public class Tutorial7 {
    public static void run() {
        System.out.println("Starting Tutorial7...");
        Network net = new Network();

        net.setOutlierRejectionEnabled(true);
        
        createEquationNode(net,
            "tra", "Return Air Temperature",
            "tra=24", 23.9, 24.1,
            280, 100);
        
        createEquationNode(net, 
            "u_d", "Damper Control Signal",
            "u_d = Bernoulli(0.539)*0.8 + 0.2", 0, 1,
            160, 100);
        
        int toa = createEquationNode(net, 
            "toa", "Outside Air Temperature",
            "toa=Normal(11,15)", -10, 40,
            60, 100);
        
        // tra, toa and u_d are referenced in equation
        // arcs are created automatically
        int tma = createEquationNode(net, 
            "tma", "Mixed Air Temperature",
            "tma=toa*u_d+(tra-tra*u_d)", 10, 30,
            110, 200);

        setUniformIntervals(net, toa, 5);
        setUniformIntervals(net, tma, 4);

        System.out.println("Results with no evidence:");
        updateAndShowStats(net);

        net.setContEvidence(toa, 28.5);
        System.out.println(
            "Results with outside air temperature set to 28.5:");
        updateAndShowStats(net);
        
        net.clearEvidence(toa);
        System.out.println(
            "Results with mixed air temperature set to 21:");
        net.setContEvidence(tma, 21.0);
        updateAndShowStats(net);
        
        net.writeFile("tutorial7.xdsl");
        System.out.println(
            "Tutorial7 complete: Network written to tutorial7.xdsl");
    }

    
    static int createEquationNode(
    		Network net, String id, String name,
    	    String equation, double loBound, double hiBound,
    	    int xPos, int yPos) {
		int handle = net.addNode(Network.NodeType.EQUATION, id);
    	net.setNodeName(handle, name);
    	net.setNodeEquation(handle, equation);
    	net.setNodeEquationBounds(handle, loBound, hiBound);
    	
    	net.setNodePosition(handle, xPos, yPos, 85, 55);
		
		return handle;
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

    
    static void setUniformIntervals(Network net, int nodeHandle, int count) {
    	double[] bounds = net.getNodeEquationBounds(nodeHandle);
    	double lo = bounds[0];
    	double hi = bounds[1];
    			
    	DiscretizationInterval[] iv = new DiscretizationInterval[count];
        for (int i = 0; i < count; i++) {
            iv[i] = new DiscretizationInterval(
                null, lo + (i + 1) * (hi - lo) / count);
        }

        net.setNodeEquationDiscretization(nodeHandle, iv);
    }

    
    static void updateAndShowStats(Network net) {
        net.updateBeliefs();
        for (int h = net.getFirstNode(); h >= 0; h = net.getNextNode(h))
        {
            showStats(net, h);
        }
        System.out.println();
    }

}
