using System;
using Smile;

// Tutorial6 creates a dynamic Bayesian network (DBN),
// performs the inference, then saves the model to disk.

namespace SmileNetTutorial
{
    class Tutorial6
    {
        public static void Run()
        {
            Console.WriteLine("Starting Tutorial6...");
            Network net = new Network();

            int loc = CreateCptNode(
                net, "Location", "Location",
                new String[] { "Pittsburgh", "Sahara" },
                160, 360);

            int rain = CreateCptNode(
                net, "Rain", "Rain",
                new String[] { "true", "false" },
                380, 240);

            int umb = CreateCptNode(
                net, "Umbrella", "Umbrella",
                new String[] { "true", "false" },
                300, 100);

            net.SetNodeTemporalType(rain, Network.NodeTemporalType.Plate);
            net.SetNodeTemporalType(umb, Network.NodeTemporalType.Plate);

            net.AddArc(loc, rain);
            net.AddTemporalArc(rain, rain, 1);
            net.AddArc(rain, umb);

            double[] rainDef = new double[] 
            {
                0.7,  // P(Rain=true |Location=Pittsburgh)
                0.3,  // P(Rain=false|Location=Pittsburgh)
                0.01, // P(Rain=true |Location=Sahara)
                0.99  // P(Rain=false|Location=Sahara)
            };
            net.SetNodeDefinition(rain, rainDef);

            double[] rainDefTemporal = new double[] 
            {
                0.7,   // P(Rain=true |Location=Pittsburgh,Rain[t-1]=true)
                0.3,   // P(Rain=false|Location=Pittsburgh,Rain[t-1]=true)
                0.3,   // P(Rain=true |Location=Pittsburgh,Rain[t-1]=false)
                0.7,   // P(Rain=false|Location=Pittsburgh,Rain[t-1]=false)
                0.001, // P(Rain=true |Location=Sahara,Rain[t-1]=true)
                0.999, // P(Rain=false|Location=Sahara,Rain[t-1]=true)
                0.01,  // P(Rain=true |Location=Sahara,Rain[t-1]=false)
                0.99  // P(Rain=false|Location=Sahara,Rain[t-1]=false)
		    };
            net.SetNodeTemporalDefinition(rain, 1, rainDefTemporal);

            double[] umbDef = new double[] 
            {
                0.9, // P(Umbrella=true |Rain=true)
                0.1, // P(Umbrella=false|Rain=true)
                0.2, // P(Umbrella=true |Rain=false)
                0.8  // P(Umbrella=false|Rain=false)		
            };
            net.SetNodeDefinition(umb, umbDef);

            net.SetSliceCount(5);

            Console.WriteLine("Performing update without evidence.");
            UpdateAndShowTemporalResults(net);

            Console.WriteLine(
                "Setting Umbrella[t=1] to true and Umbrella[t=3] to false.");
            net.SetTemporalEvidence(umb, 1, 0);
            net.SetTemporalEvidence(umb, 3, 1);
            UpdateAndShowTemporalResults(net);

            net.WriteFile("tutorial6.xdsl");
            Console.WriteLine(
                "Tutorial6 complete: Network written to tutorial6.xdsl");
        }


        private static void UpdateAndShowTemporalResults(Network net)
        {
            net.UpdateBeliefs();
            int sliceCount = net.GetSliceCount();
            for (int h = net.GetFirstNode(); h >= 0; h = net.GetNextNode(h))
            {
                if (net.GetNodeTemporalType(h) == 
                    Network.NodeTemporalType.Plate)
                {
                    int outcomeCount = net.GetOutcomeCount(h);
                    Console.WriteLine(
                        "Temporal beliefs for {0}:", net.GetNodeId(h));
                    double[] v = net.GetNodeValue(h);
                    for (int sliceIdx = 0; sliceIdx < sliceCount; sliceIdx++)
                    {
                        Console.Write("\tt={0}:", sliceIdx);
                        for (int i = 0; i < outcomeCount; i++)
                        {
                            Console.Write(
                                " {0}", v[sliceIdx * outcomeCount + i]);
                        }
                        Console.WriteLine();
                    }
                }
            }
            Console.WriteLine();
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
