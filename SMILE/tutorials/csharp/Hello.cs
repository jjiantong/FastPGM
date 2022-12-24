using System;
using Smile;

namespace SmileNetTutorial
{
    class Hello
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Insert your SMILE License Key here.");
            // new Smile.License(...);

            Network net = new Network();
            net.ReadFile("VentureBN.xdsl");
            net.SetEvidence("Forecast", "Moderate");
            net.UpdateBeliefs();
            double[] beliefs = net.GetNodeValue("Success");
            for (int i = 0; i < beliefs.Length; i++)
            {
                Console.WriteLine("{0} = {1}", 
                    net.GetOutcomeId("Success", i), beliefs[i]);
            }
        }
    }
}
