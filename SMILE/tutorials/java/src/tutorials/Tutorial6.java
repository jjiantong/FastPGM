package tutorials;

import smile.*;

// Tutorial6 creates a dynamic Bayesian network (DBN),
// performs the inference, then saves the model to disk.

public class Tutorial6 {
    public static void run() {
        System.out.println("Starting Tutorial6...");
        Network net = new Network();
        
        int loc = createCptNode(
        	net, "Location", "Location",
        	new String[] { "Pittsburgh", "Sahara" }, 
        	160, 360);
        
        int rain = createCptNode(
        	net, "Rain", "Rain",
        	new String[] { "true", "false" }, 
        	380, 240);
        
        int umb = createCptNode(
        	net, "Umbrella", "Umbrella",
        	new String[] { "true", "false" },
        	300, 100);
        
        net.setNodeTemporalType(rain, Network.NodeTemporalType.PLATE);
        net.setNodeTemporalType(umb, Network.NodeTemporalType.PLATE);
        
        net.addArc(loc, rain);
        net.addTemporalArc(rain, rain, 1);
        net.addArc(rain, umb);
    
        double[] rainDef = new double[] {
            0.7,  // P(Rain=true |Location=Pittsburgh)
            0.3,  // P(Rain=false|Location=Pittsburgh)
            0.01, // P(Rain=true |Location=Sahara)
            0.99  // P(Rain=false|Location=Sahara)
        };
        net.setNodeDefinition(rain, rainDef);

        double[] rainDefTemporal = new double[] {
            0.7,   // P(Rain=true |Location=Pittsburgh,Rain[t-1]=true)
            0.3,   // P(Rain=false|Location=Pittsburgh,Rain[t-1]=true)
            0.3,   // P(Rain=true |Location=Pittsburgh,Rain[t-1]=false)
            0.7,   // P(Rain=false|Location=Pittsburgh,Rain[t-1]=false)
            0.001, // P(Rain=true |Location=Sahara,Rain[t-1]=true)
            0.999, // P(Rain=false|Location=Sahara,Rain[t-1]=true)
            0.01,  // P(Rain=true |Location=Sahara,Rain[t-1]=false)
            0.99  // P(Rain=false|Location=Sahara,Rain[t-1]=false)
		};
        net.setNodeTemporalDefinition(rain, 1, rainDefTemporal);
        
        double[] umbDef = new double[] {
    		0.9, // P(Umbrella=true |Rain=true)
            0.1, // P(Umbrella=false|Rain=true)
            0.2, // P(Umbrella=true |Rain=false)
            0.8  // P(Umbrella=false|Rain=false)		
        };
        net.setNodeDefinition(umb, umbDef);
        
        net.setSliceCount(5);

        System.out.println("Performing update without evidence.");
        updateAndShowTemporalResults(net);

        System.out.println(
            "Setting Umbrella[t=1] to true and Umbrella[t=3] to false.");
        net.setTemporalEvidence(umb, 1, 0);
        net.setTemporalEvidence(umb, 3, 1);
        updateAndShowTemporalResults(net);

        net.writeFile("tutorial6.xdsl");
        System.out.println(
            "Tutorial6 complete: Network written to tutorial6.xdsl");
    }
    
    
    private static void updateAndShowTemporalResults(Network net) {
    	net.updateBeliefs();
	    int sliceCount = net.getSliceCount();
	    for (int h = net.getFirstNode(); h >= 0; h = net.getNextNode(h)) {
	    	if (net.getNodeTemporalType(h) == Network.NodeTemporalType.PLATE) {
	    		int outcomeCount = net.getOutcomeCount(h);
	            System.out.printf(
                    "Temporal beliefs for %s:\n", net.getNodeId(h));
	            double[] v = net.getNodeValue(h);
				for (int sliceIdx = 0; sliceIdx < sliceCount; sliceIdx++) {
					System.out.printf("\tt=%d:", sliceIdx);
					for (int i = 0; i < outcomeCount; i++) {
					    System.out.printf(" %f", v[sliceIdx*outcomeCount+i]);
					}
					System.out.println();
				}
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
