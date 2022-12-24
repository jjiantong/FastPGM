using System;
using Smile;

// Tutorial2 loads the XDSL file created by Tutorial1,
// then performs the series of inference calls,
// changing evidence each time.

namespace SmileNetTutorial
{
    class Tutorial2
    {
        public static void Run()
        {
            Console.WriteLine("Starting Tutorial2...");
            Network net = new Network();

            // load the network created by Tutorial1
            net.ReadFile("tutorial1.xdsl");

            Console.WriteLine("Posteriors with no evidence set:");
            net.UpdateBeliefs();
            PrintAllPosteriors(net);

            Console.WriteLine("Setting Forecast=Good.");
            ChangeEvidenceAndUpdate(net, "Forecast", "Good");

            Console.WriteLine("Adding Economy=Up.");
            ChangeEvidenceAndUpdate(net, "Economy", "Up");

            Console.WriteLine("Changing Forecast to Poor, keeping Economy=Up.");
            ChangeEvidenceAndUpdate(net, "Forecast", "Poor");

            Console.WriteLine(
                "Removing evidence from Economy, keeping Forecast=Poor.");
            ChangeEvidenceAndUpdate(net, "Economy", null);

            Console.WriteLine("Tutorial2 complete.");
        }


        private static void PrintPosteriors(Network net, int nodeHandle)
        {
            String nodeId = net.GetNodeId(nodeHandle);
            if (net.IsEvidence(nodeHandle))
            {
                Console.WriteLine("{0} has evidence set ({1})",
                    nodeId,
                    net.GetOutcomeId(nodeHandle, net.GetEvidence(nodeHandle)));
            }
            else
            {
                double[] posteriors = net.GetNodeValue(nodeHandle);
                for (int i = 0; i < posteriors.Length; i++)
                {
                    Console.WriteLine("P({0}={1})={2}",
                        nodeId,
                        net.GetOutcomeId(nodeHandle, i),
                        posteriors[i]);
                }
            }
        }


        private static void PrintAllPosteriors(Network net)
        {
            for (int h = net.GetFirstNode(); h >= 0; h = net.GetNextNode(h))
            {
                PrintPosteriors(net, h);
            }
            Console.WriteLine();
        }


        private static void ChangeEvidenceAndUpdate(
            Network net, String nodeId, String outcomeId)
        {
            if (outcomeId != null)
            {
                net.SetEvidence(nodeId, outcomeId);
            }
            else
            {
                net.ClearEvidence(nodeId);
            }

            net.UpdateBeliefs();
            PrintAllPosteriors(net);
        }

    }
}
