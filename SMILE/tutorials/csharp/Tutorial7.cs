using System;
using Smile;

// Tutorial7 creates a network with three equation-based nodes
// performs the inference, then saves the model to disk.

namespace SmileNetTutorial
{
    class Tutorial7
    {
        public static void Run()
        {
            Console.WriteLine("Starting Tutorial7...");
            Network net = new Network();

            net.OutlierRejectionEnabled = true;

            CreateEquationNode(net,
                "tra", "Return Air Temperature",
                "tra=24", 23.9, 24.1,
                280, 100);

            CreateEquationNode(net,
                "u_d", "Damper Control Signal",
                "u_d = Bernoulli(0.539)*0.8 + 0.2", 0, 1,
                160, 100);

            int toa = CreateEquationNode(net,
                "toa", "Outside Air Temperature",
                "toa=Normal(11,15)", -10, 40,
                60, 100);

            // tra, toa and u_d are referenced in equation
            // arcs are created automatically
            int tma = CreateEquationNode(net,
                "tma", "Mixed Air Temperature",
                "tma=toa*u_d+(tra-tra*u_d)", 10, 30,
                110, 200);

            SetUniformIntervals(net, toa, 5);
            SetUniformIntervals(net, tma, 4);

            Console.WriteLine("Results with no evidence:");
            UpdateAndShowStats(net);

            net.SetContEvidence(toa, 28.5);
            Console.WriteLine(
                "Results with outside air temperature set to 28.5:");
            UpdateAndShowStats(net);

            net.ClearEvidence(toa);
            Console.WriteLine(
                "Results with mixed air temperature set to 21:");
            net.SetContEvidence(tma, 21.0);
            UpdateAndShowStats(net);

            net.WriteFile("tutorial7.xdsl");
            Console.WriteLine(
                "Tutorial7 complete: Network written to tutorial7.xdsl");
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


        private static void SetUniformIntervals(
            Network net, int nodeHandle, int count)
        {
            double[] bounds = net.GetNodeEquationBounds(nodeHandle);
            double lo = bounds[0];
            double hi = bounds[1];

            DiscretizationInterval[] iv = new DiscretizationInterval[count];
            for (int i = 0; i < count; i++)
            {
                iv[i] = new DiscretizationInterval(
                    null, lo + (i + 1) * (hi - lo) / count);
            }

            net.SetNodeEquationDiscretization(nodeHandle, iv);
        }


        private static void UpdateAndShowStats(Network net)
        {
            net.UpdateBeliefs();
            for (int h = net.GetFirstNode(); h >= 0; h = net.GetNextNode(h))
            {
                ShowStats(net, h);
            }
            Console.WriteLine();
        }

    }
}
