using System;
using Smile;

// Tutorial4 loads the XDSL file file created by Tutorial1
// and adds decision and utility nodes, which transforms 
// a Bayesian Network (BN) into an Influence Diagram (ID).

namespace SmileNetTutorial
{
    class Tutorial4
    {
        public static void Run()
        {
            Console.WriteLine("Starting Tutorial4...");
            Network net = new Network();

            net.ReadFile("tutorial1.xdsl");

            int s = net.GetNode("Success");

            int i = CreateNode(net, Network.NodeType.List,
                    "Invest", "Investment decision",
                    new String[] { "Invest", "DoNotInvest" }, 160, 240);

            int g = CreateNode(net, Network.NodeType.Table,
                    "Gain", "Financial gain", null, 60, 200);

            net.AddArc(i, g);
            net.AddArc(s, g);

            double[] gainDefinition = new double[] 
            {
                10000, // Utility(Invest=I, Success=S)   
                -5000, // Utility(Invest=I, Success=F)
                500,   // Utility(Invest=D, Success=S)
                500    // Utility(Invest=D, Success=F)
            };
            net.SetNodeDefinition(g, gainDefinition);

            net.WriteFile("tutorial4.xdsl");

            Console.WriteLine(
                "Tutorial4 complete: ID written to tutorial4.xdsl.");
         }

        private static int CreateNode(
            Network net, Network.NodeType nodeType, String id, String name,
            String[] outcomes, int xPos, int yPos)
        {
            int handle = net.AddNode(nodeType, id);

            net.SetNodeName(handle, name);
            net.SetNodePosition(handle, xPos, yPos, 85, 55);

            if (outcomes != null)
            {
                int initialOutcomeCount = net.GetOutcomeCount(handle);
                for (int i = 0; i < initialOutcomeCount; i++)
                {
                    net.SetOutcomeId(handle, i, outcomes[i]);
                }

                for (int i = initialOutcomeCount; i < outcomes.Length; i++)
                {
                    net.AddOutcome(handle, outcomes[i]);
                }
            }

            return handle;
        }

    }
}
