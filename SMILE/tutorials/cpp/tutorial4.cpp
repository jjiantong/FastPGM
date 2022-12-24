// tutorial4.cpp
// Tutorial4 loads the XDSL file file created by Tutorial1
// and adds decision and utility nodes, which transforms 
// a Bayesian Network (BN) into an Influence Diagram (ID).

#include "smile.h"
#include <cstdio>


static int CreateNode(
    DSL_network &net, int nodeType, const char *id, const char *name, 
    const char *outcomes[], int xPos, int yPos);


int Tutorial4()
{
    printf("Starting Tutorial4...\n");

    DSL_errorH().RedirectToFile(stdout);

    DSL_network net;
    // load the network created by Tutorial1
    int res = net.ReadFile("tutorial1.xdsl");
    if (DSL_OKAY != res)
    {
        printf(
            "Network load failed, did you run Tutorial1 before Tutorial4?\n");
        return res;
    }

    int s = net.FindNode("Success");
    if (s < 0)
    {
        printf("Success node not found.");
        return s;
    }

    const char *INVEST_DECISIONS[] = { "Invest", "DoNotInvest", NULL };
    int i = CreateNode(net, DSL_LIST, "Invest", "Investment decision", 
        INVEST_DECISIONS, 160, 240);

    int g = CreateNode(net, DSL_TABLE, "Gain", "Financial gain", NULL, 60, 200);

    net.AddArc(i, g);
    net.AddArc(s, g);

    DSL_nodeDefinition *gainDef = net.GetNode(g)->Definition();
    DSL_doubleArray gu(gainDef->GetMatrix()->GetSize());
    gu[0] = 10000; gu[1] = -5000; gu[2] = 500; gu[3] = 500;
    res = gainDef->SetDefinition(gu);
    if (DSL_OKAY != res)
    {
        return res;
    }

    res = net.WriteFile("tutorial4.xdsl");
    if (DSL_OKAY != res)
    {
        return res;
    }

    printf("Tutorial4 complete: Influence diagram written to tutorial4.xdsl\n");
    return DSL_OKAY;
}


static int CreateNode(
    DSL_network &net, int nodeType, const char *id, const char *name, 
    const char *outcomes[], int xPos, int yPos)
{
    int handle = net.AddNode(nodeType, id);
    DSL_node *node = net.GetNode(handle);

    if (NULL != outcomes)
    {
        DSL_nodeDefinition *def = node->Definition();
        DSL_idArray ida;
        for (const char **p = outcomes; *p != NULL; p++)
        {
            ida.Add(*p);
        }
        def->SetNumberOfOutcomes(ida);
    }

    node->Info().Header().SetName(name);

    DSL_rectangle &position = node->Info().Screen().position;
    position.center_X = xPos;
    position.center_Y = yPos;
    position.width = 85;
    position.height = 55;

    return handle;
}
