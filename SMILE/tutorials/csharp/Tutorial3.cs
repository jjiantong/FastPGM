using System;
using Smile;

// Tutorial3 loads the XDSL file and prints the information
// about the structure (nodes and arcs) and the parameters 
// (conditional probabilities of the nodes) of the network.

namespace SmileNetTutorial
{
    class Tutorial3
    {
        public static void Run()
        {
            Console.WriteLine("Starting Tutorial3...");
            Network net = new Network();
            
            // load the network created by Tutorial1
            net.ReadFile("tutorial1.xdsl");

            for (int h = net.GetFirstNode(); h >= 0; h = net.GetNextNode(h))
            {
                PrintNodeInfo(net, h);
            }

            Console.WriteLine("Tutorial3 complete.");
        }


        private static void PrintNodeInfo(Network net, int nodeHandle)
        {
            Console.WriteLine("Node id/name: {0}/{1}",
                net.GetNodeId(nodeHandle),
                net.GetNodeName(nodeHandle));

            Console.Write("  Outcomes:");
            foreach (String outcomeId in net.GetOutcomeIds(nodeHandle))
            {
                Console.Write(" " + outcomeId);
            }
            Console.WriteLine();

            String[] parentIds = net.GetParentIds(nodeHandle);
            if (parentIds.Length > 0)
            {
                Console.Write("  Parents:");
                foreach (String parentId in parentIds)
                {
                    Console.Write(" " + parentId);
                }
                Console.WriteLine();
            }

            String[] childIds = net.GetChildIds(nodeHandle);
            if (childIds.Length > 0)
            {
                Console.Write("  Children:");
                foreach (String childId in childIds)
                {
                    Console.Write(" " + childId);
                }
                Console.WriteLine();
            }

            PrintCptMatrix(net, nodeHandle);
        }


        private static void PrintCptMatrix(Network net, int nodeHandle)
        {
            double[] cpt = net.GetNodeDefinition(nodeHandle);
            int[] parents = net.GetParents(nodeHandle);
            int dimCount = 1 + parents.Length;

            int[] dimSizes = new int[dimCount];
            for (int i = 0; i < dimCount - 1; i++)
            {
                dimSizes[i] = net.GetOutcomeCount(parents[i]);
            }
            dimSizes[dimSizes.Length - 1] = net.GetOutcomeCount(nodeHandle);

            int[] coords = new int[dimCount];
            for (int elemIdx = 0; elemIdx < cpt.Length; elemIdx++)
            {
                IndexToCoords(elemIdx, dimSizes, coords);

                String outcome = 
                    net.GetOutcomeId(nodeHandle, coords[dimCount - 1]);
                Console.Write("    P({0}", outcome);

                if (dimCount > 1)
                {
                    Console.Write(" | ");
                    for (int pIdx = 0; pIdx < parents.Length; pIdx++)
                    {
                        if (pIdx > 0) Console.Write(",");
                        int parentHandle = parents[pIdx];
                        Console.Write("{0}={1}",
                            net.GetNodeId(parentHandle),
                            net.GetOutcomeId(parentHandle, coords[pIdx]));
                    }
                }

                double prob = cpt[elemIdx];
                Console.WriteLine(")={0}", prob);
            }
        }

        private static void IndexToCoords(
            int index, int[] dimSizes, int[] coords)
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
