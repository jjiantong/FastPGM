using System;
using Smile;

// Tutorial8 loads continuous model from the XDSL file written by Tutorial7,
// then adds discrete nodes to create a hybrid model. Inference is performed
// and model is saved to disk.

namespace SmileNetTutorial
{
    class Tutorial8
    {
        public static void Run()
        {
            Console.WriteLine("Starting Tutorial8...");
            Network net = new Network();

            net.ReadFile("tutorial7.xdsl");

            CreateCptNode(
                net, "zone", "Climate Zone",
                new String[] { "Temperate", "Desert" },
                60, 20);

            int toaHandle = net.GetNode("toa");
            net.SetNodeEquation(toaHandle, 
                "toa=If(zone=\"Desert\",Normal(22,5),Normal(11,10))");

            int perceivedHandle = CreateCptNode(
                net, "perceived", "Perceived Temperature",
                new String[] { "Hot", "Warm", "Cold" },
                60, 300);
            net.AddArc(toaHandle, perceivedHandle);

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
            net.SetNodeDefinition(perceivedHandle, perceivedProbs);

            net.SetEvidence("zone", "Temperate");
            Console.WriteLine("Results in temperate zone:");
            UpdateAndShowStats(net);

            net.SetEvidence("zone", "Desert");
            Console.WriteLine("Results in desert zone:\n");
            UpdateAndShowStats(net);

            net.WriteFile("tutorial8.xdsl");
            Console.WriteLine(
                "Tutorial8 complete: Network written to tutorial8.xdsl");
        }


        private static int CreateEquationNode(
            Network net, String id, String name,
            String equation, double loBound, double hiBound,
            int xPos, int yPos)
        {
            int handle = net.AddNode(Network.NodeType.Equation, id);
            net.SetNodeName(handle, name);
            net.SetNodePosition(handle, xPos, yPos, 85, 55);
            net.SetNodeEquation(handle, equation);
            net.SetNodeEquationBounds(handle, loBound, hiBound);
            return handle;
        }


        private static void ShowStats(Network net, int nodeHandle)
        {
            String nodeId = net.GetNodeId(nodeHandle);

            if (net.IsEvidence(nodeHandle))
            {
                double v = net.GetContEvidence(nodeHandle);
                Console.WriteLine("{0} has evidence set ({1})", nodeId, v);
                return;
            }

            if (net.IsValueDiscretized(nodeHandle))
            {
                Console.WriteLine("{0} is discretized.", nodeId);
                DiscretizationInterval[] iv = 
                    net.GetNodeEquationDiscretization(nodeHandle);
                double[] bounds = net.GetNodeEquationBounds(nodeHandle);
                double[] discBeliefs = net.GetNodeValue(nodeHandle);
                double lo = bounds[0];
                for (int i = 0; i < discBeliefs.Length; i++)
                {
                    double hi = iv[i].boundary;
                    Console.WriteLine("\tP({0} in {1}..{2})={3}", 
                        nodeId, lo, hi, discBeliefs[i]);
                    lo = hi;
                }
            }
            else
            {
                double[] stats = net.GetNodeSampleStats(nodeHandle);
                Console.WriteLine("{0}: mean={1} stddev={2} min={3} max={4}",
                    nodeId, stats[0], stats[1], stats[2], stats[3]);
            }
        }


        private static void UpdateAndShowStats(Network net)
        {
            net.UpdateBeliefs();
            for (int h = net.GetFirstNode(); h >= 0; h = net.GetNextNode(h))
            {
                if (net.GetNodeType(h) == Network.NodeType.Equation)
                {
                    ShowStats(net, h);
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
