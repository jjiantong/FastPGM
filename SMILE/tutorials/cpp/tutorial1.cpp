// tutorial1.cpp
// Tutorial1 creates a simple network with three nodes,
// then saves it as XDSL file to disk.

#include "smile.h"
#include <cstdio>


static int CreateCptNode(
    DSL_network &net, const char *id, 
    const char *name, const char *outcomes[], 
    int xPos, int yPos);


int Tutorial1()
{
    printf("Starting Tutorial1...\n");

    // show errors and warnings in the console
    DSL_errorH().RedirectToFile(stdout);

    DSL_network net;

    const char *ECONOMY_OUTCOMES[] = { "Up", "Flat", "Down", NULL };
    int e = CreateCptNode(net, "Economy", "State of the economy", 
        ECONOMY_OUTCOMES, 160, 40);

    const char *SUCCESS_OUTCOMES[] = { "Success", "Failure", NULL };
    int s = CreateCptNode(net, "Success", "Success of the venture", 
        SUCCESS_OUTCOMES, 60, 40);
    
    const char *FORECAST_OUTCOMES[] = { "Good", "Moderate", "Poor", NULL };
    int f = CreateCptNode(net, "Forecast", "Expert forecast", 
        FORECAST_OUTCOMES, 110, 140);

    net.AddArc(e, s);
    net.AddArc(s, f);
    net.AddArc(e, f);

    DSL_nodeDefinition *economyDef = net.GetNode(e)->Definition();
    DSL_doubleArray ep(economyDef->GetMatrix()->GetSize());
    ep[0] = 0.2; // P(Economy=U)
    ep[1] = 0.7; // P(Economy=F)
    ep[2] = 0.1; // P(Economy=D)
    int res = economyDef->SetDefinition(ep);
    if (DSL_OKAY != res)
    {
        return res;
    }

    DSL_nodeDefinition *successDef = net.GetNode(s)->Definition();
    DSL_doubleArray sp(successDef->GetMatrix()->GetSize());
    sp[0] = 0.3; // P(Success=S|Economy=U)
    sp[1] = 0.7; // P(Success=F|Economy=U)
    sp[2] = 0.2; // P(Success=S|Economy=F)
    sp[3] = 0.8; // P(Success=F|Economy=F)
    sp[4] = 0.1; // P(Success=S|Economy=D)
    sp[5] = 0.9; // P(Success=F|Economy=D)
    res = successDef->SetDefinition(sp);
    if (DSL_OKAY != res)
    {
        return res;
    }

    DSL_nodeDefinition *forecastDef = net.GetNode(f)->Definition();
    DSL_doubleArray fp(forecastDef->GetMatrix()->GetSize());
    fp[0] = 0.70; // P(Forecast=G|Success=S,Economy=U)
    fp[1] = 0.29; // P(Forecast=M|Success=S,Economy=U)
    fp[2] = 0.01; // P(Forecast=P|Success=S,Economy=U)
    
    fp[3] = 0.65; // P(Forecast=G|Success=S,Economy=F)
    fp[4] = 0.30; // P(Forecast=M|Success=S,Economy=F)
    fp[5] = 0.05; // P(Forecast=P|Success=S,Economy=F)

    fp[6] = 0.60; // P(Forecast=G|Success=S,Economy=D)
    fp[7] = 0.30; // P(Forecast=M|Success=S,Economy=D)
    fp[8] = 0.10; // P(Forecast=P|Success=S,Economy=D)

    fp[9] = 0.15;  // P(Forecast=G|Success=F,Economy=U)
    fp[10] = 0.30; // P(Forecast=M|Success=F,Economy=U)
    fp[11] = 0.55; // P(Forecast=P|Success=F,Economy=U)

    fp[12] = 0.10; // P(Forecast=G|Success=F,Economy=F)
    fp[13] = 0.30; // P(Forecast=M|Success=F,Economy=F)
    fp[14] = 0.60; // P(Forecast=P|Success=F,Economy=F)

    fp[15] = 0.05; // P(Forecast=G|Success=F,Economy=D)
    fp[16] = 0.25; // P(Forecast=G|Success=F,Economy=D)
    fp[17] = 0.70; // P(Forecast=G|Success=F,Economy=D)

    res = forecastDef->SetDefinition(fp);
    if (DSL_OKAY != res)
    {
        return res;
    }

    res = net.WriteFile("tutorial1.xdsl");
    if (DSL_OKAY != res)
    {
        return res;
    }

    printf("Tutorial1 complete: Network written to tutorial1.xdsl\n");
    return DSL_OKAY;
}


static int CreateCptNode(
	DSL_network &net, const char *id, const char *name, 
    const char *outcomes[], int xPos, int yPos)
{
    int handle = net.AddNode(DSL_CPT, id);
    DSL_node *node = net.GetNode(handle);

    DSL_idArray ida;
    for (const char **p = outcomes; *p != NULL; p++)
    {
        ida.Add(*p);
    }
    DSL_nodeDefinition *def = node->Definition();
    def->SetNumberOfOutcomes(ida);

    node->Info().Header().SetName(name);

    DSL_rectangle &position = node->Info().Screen().position;
    position.center_X = xPos;
    position.center_Y = yPos;
    position.width = 85;
    position.height = 55;

    return handle;
}
