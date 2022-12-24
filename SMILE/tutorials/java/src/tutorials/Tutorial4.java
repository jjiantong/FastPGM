package tutorials;

import smile.*;

// Tutorial4 loads the XDSL file file created by Tutorial1
// and adds decision and utility nodes, which transforms 
// a Bayesian Network (BN) into an Influence Diagram (ID).

public class Tutorial4 {
    public static void run() {
        System.out.println("Starting Tutorial4...");
        Network net = new Network();
        
        net.readFile("tutorial1.xdsl");

        int s = net.getNode("Success");

        int i = createNode(net, Network.NodeType.DECISION, 
                "Invest", "Investment decision", 
                new String[]{ "Invest", "DoNotInvest" }, 160, 240);

        int g = createNode(net, Network.NodeType.UTILITY, 
                "Gain", "Financial gain", null, 60, 200);

        net.addArc(i, g);
        net.addArc(s, g);

        double[] gainDefinition = new double[] { 
            10000, // Utility(Invest=I, Success=S)   
            -5000, // Utility(Invest=I, Success=F)
            500,   // Utility(Invest=D, Success=S)
            500    // Utility(Invest=D, Success=F)
        };
        net.setNodeDefinition(g, gainDefinition);
        
        net.writeFile("tutorial4.xdsl");

        System.out.println(
            "Tutorial4 complete: Influence diagram written to tutorial4.xdsl.");
    }
    

    private static int createNode(
            Network net, int nodeType, String id, String name, 
            String[] outcomes, int xPos, int yPos) {
        int handle = net.addNode(nodeType, id);
        
        net.setNodeName(handle, name);
        net.setNodePosition(handle, xPos, yPos, 85, 55);
        
        if (outcomes != null) {
            int initialOutcomeCount = net.getOutcomeCount(handle); 
            for (int i = 0; i < initialOutcomeCount; i ++) {
                net.setOutcomeId(handle, i, outcomes[i]);
            }
            
            for (int i = initialOutcomeCount; i < outcomes.length; i ++) {
                net.addOutcome(handle, outcomes[i]);
            }
        }
        
        return handle;
    }

}
