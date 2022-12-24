using System;
using Smile;

// Tutorial1 creates a simple network with three nodes,
// then writes its content as XDSL file to disk.

namespace SmileNetTutorial
{
    class Tutorial1
    {
        public static void Run()
        {
            Console.WriteLine("Starting Tutorial1...");
            Network net = new Network();

            int e = CreateCptNode(net,
                "Economy", "State of the economy",
                new String[] { "Up", "Flat", "Down" },
                160, 40);

            int s = CreateCptNode(net,
                "Success", "Success of the venture",
                new String[] { "Success", "Failure" },
                60, 40);

            int f = CreateCptNode(net,
                "Forecast", "Expert forecast",
                new String[] { "Good", "Moderate", "Poor" },
                110, 140);

            net.AddArc(e, s);
            net.AddArc(s, f);

            // we can also use node identifiers when creating arcs  
            net.AddArc("Economy", "Forecast");

            double[] economyDef = 
            {
                0.2, // P(Economy=U)
                0.7, // P(Economy=F)
                0.1  // P(Economy=D)
            };
            net.SetNodeDefinition(e, economyDef);

            double[] successDef = new double[] 
            {
                0.3, // P(Success=S|Economy=U)
                0.7, // P(Success=F|Economy=U)
                0.2, // P(Success=S|Economy=F)
                0.8, // P(Success=F|Economy=F)
                0.1, // P(Success=S|Economy=D)
                0.9  // P(Success=F|Economy=D)
            };
            net.SetNodeDefinition(s, successDef);

            double[] forecastDef = new double[] 
            {
                0.70, // P(Forecast=G|Success=S,Economy=U)
                0.29, // P(Forecast=M|Success=S,Economy=U)
                0.01, // P(Forecast=P|Success=S,Economy=U)
    
                0.65, // P(Forecast=G|Success=S,Economy=F)
                0.30, // P(Forecast=M|Success=S,Economy=F)
                0.05, // P(Forecast=P|Success=S,Economy=F)
    
                0.60, // P(Forecast=G|Success=S,Economy=D)
                0.30, // P(Forecast=M|Success=S,Economy=D)
                0.10, // P(Forecast=P|Success=S,Economy=D)
    
                0.15,  // P(Forecast=G|Success=F,Economy=U)
                0.30, // P(Forecast=M|Success=F,Economy=U)
                0.55, // P(Forecast=P|Success=F,Economy=U)
    
                0.10, // P(Forecast=G|Success=F,Economy=F)
                0.30, // P(Forecast=M|Success=F,Economy=F)
                0.60, // P(Forecast=P|Success=F,Economy=F)
    
                0.05, // P(Forecast=G|Success=F,Economy=D)
                0.25, // P(Forecast=G|Success=F,Economy=D)
                0.70  // P(Forecast=G|Success=F,Economy=D)	
            };
            net.SetNodeDefinition(f, forecastDef);

            net.WriteFile("tutorial1.xdsl");

            Console.WriteLine(
                "Tutorial1 complete: Network written to tutorial1.xdsl");
        }


        private static int CreateCptNode(
            Network net, String id, String name,
            String[] outcomes, int xPos, int yPos)
        {
            int handle = net.AddNode(Network.NodeType.Cpt, id);

            net.SetNodeName(handle, name);
            net.SetNodePosition(handle, xPos, yPos, 85, 55);

            int initialOutcomeCount = net.GetOutcomeCount(handle);
            for (int i = 0; i < initialOutcomeCount; i++)
            {
                net.SetOutcomeId(handle, i, outcomes[i]);
            }

            for (int i = initialOutcomeCount; i < outcomes.Length; i++)
            {
                net.AddOutcome(handle, outcomes[i]);
            }

            return handle;
        }
    }

}
