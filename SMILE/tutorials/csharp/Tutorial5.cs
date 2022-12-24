using System;
using Smile;

// Tutorial5 loads the XDSL file created by Tutorial4,
// then performs the series of inference calls,
// changing evidence each time.

namespace SmileNetTutorial
{
    class Tutorial5
    {
        public static void Run()
        {
            Console.WriteLine("Starting Tutorial5...");
            Network net = new Network();

            net.ReadFile("tutorial4.xdsl");

            Console.WriteLine("No evidence set.");
            net.UpdateBeliefs();
            PrintFinancialGain(net);

            Console.WriteLine("Setting Forecast=Good.");
            ChangeEvidenceAndUpdate(net, "Forecast", "Good");

            Console.WriteLine("Adding Economy=Up");
            ChangeEvidenceAndUpdate(net, "Economy", "Up");

            Console.WriteLine("Tutorial5 complete.");
        }


        static void ChangeEvidenceAndUpdate(
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
            PrintFinancialGain(net);
        }


        static void PrintFinancialGain(Network net)
        {
            double[] expectedUtility = net.GetNodeValue("Gain");
            int[] utilParents = net.GetValueIndexingParents("Gain");
            printGainMatrix(net, expectedUtility, utilParents);
        }


        static void printGainMatrix(Network net, double[] mtx, int[] parents)
        {
            int dimCount = 1 + parents.Length;

            int[] dimSizes = new int[dimCount];
            for (int i = 0; i < dimCount - 1; i++)
            {
                dimSizes[i] = net.GetOutcomeCount(parents[i]);
            }
            dimSizes[dimSizes.Length - 1] = 1;

            int[] coords = new int[dimCount];
            for (int elemIdx = 0; elemIdx < mtx.Length; elemIdx++)
            {
                IndexToCoords(elemIdx, dimSizes, coords);

                Console.Write("    Utility(");

                if (dimCount > 1)
                {
                    for (int pIdx = 0; pIdx < parents.Length; pIdx++)
                    {
                        if (pIdx > 0) Console.Write(",");
                        int parentHandle = parents[pIdx];
                        Console.Write("{0}={1}",
                            net.GetNodeId(parentHandle),
                            net.GetOutcomeId(parentHandle, coords[pIdx]));
                    }
                }

                Console.Write(")={0}\n", mtx[elemIdx]);
            }
            Console.WriteLine();
        }


        static void IndexToCoords(int index, int[] dimSizes, int[] coords)
        {
            int prod = 1;
            for (int i = dimSizes.Length - 1; i >= 0; i--)
            {
                coords[i] = (index / prod) % dimSizes[i];
                prod *= dimSizes[i];
            }
        }

    }
}
