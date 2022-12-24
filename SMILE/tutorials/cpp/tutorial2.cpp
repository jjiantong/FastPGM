// tutorial2.cpp
// Tutorial2 loads the XDSL file created by Tutorial1,
// then performs the series of inference calls,
// changing evidence each time.

#include "smile.h"
#include <cstdio>


static int ChangeEvidenceAndUpdate(
    DSL_network &net, const char *nodeId, const char *outcomeId);
static void PrintAllPosteriors(DSL_network &net);


int Tutorial2()
{
    printf("Starting Tutorial2...\n");

    DSL_errorH().RedirectToFile(stdout);

    // load the network created by Tutorial1
    DSL_network net;
    int res = net.ReadFile("tutorial1.xdsl");
    if (DSL_OKAY != res)
    {
        printf(
            "Network load failed, did you run Tutorial1 before Tutorial2?\n");
        return res;
    }

    printf("Posteriors with no evidence set:\n");
    net.UpdateBeliefs();
    PrintAllPosteriors(net);

    printf("\nSetting Forecast=Good.\n");
    ChangeEvidenceAndUpdate(net, "Forecast", "Good");

    printf("\nAdding Economy=Up.\n");
    ChangeEvidenceAndUpdate(net, "Economy", "Up");

    printf("\nChanging Forecast to Poor, keeping Economy=Up.\n");
    ChangeEvidenceAndUpdate(net, "Forecast", "Poor");

    printf("\nRemoving evidence from Economy, keeping Forecast=Poor.\n");
    ChangeEvidenceAndUpdate(net, "Economy", NULL);

    printf("\nTutorial2 complete.\n");
    return DSL_OKAY;
}


static int SetEvidenceById(
    DSL_network &net, const char *nodeId, const char *outcomeId)
{
    int handle = net.FindNode(nodeId);
    if (handle < 0)
    {
        return handle;
    }

    DSL_node *node = net.GetNode(handle);
    
    if (NULL == outcomeId)
    {
        return node->Value()->ClearEvidence();
    }
    else
    {
        int idx = 
            node->Definition()->GetOutcomesNames()->FindPosition(outcomeId);
        if (idx < 0)
        {
            return idx;
        }

        return node->Value()->SetEvidence(idx);
    }
}


static void PrintPosteriors(DSL_network &net, int handle)
{
    DSL_node *node = net.GetNode(handle);
    const char *nodeId = node->GetId();
    const DSL_idArray &outcomeIds = *node->Definition()->GetOutcomesNames();
    DSL_nodeValue *val = node->Value();
    if (val->IsEvidence())
    {
        printf("%s has evidence set (%s)\n", 
            nodeId, outcomeIds[val->GetEvidence()]);
    }
    else
    {
        const DSL_Dmatrix &posteriors = *val->GetMatrix();
        for (int i = 0; i < posteriors.GetSize(); i++)
        {
            printf("P(%s=%s)=%g\n", nodeId, outcomeIds[i], posteriors[i]);
        }
    }
}


static void PrintAllPosteriors(DSL_network &net)
{
    for (int h = net.GetFirstNode(); h >= 0; h = net.GetNextNode(h))
    {
        PrintPosteriors(net, h);
    }
}


static int ChangeEvidenceAndUpdate(
    DSL_network &net, const char *nodeId, const char *outcomeId)
{
    int res = SetEvidenceById(net, nodeId, outcomeId);
    if (DSL_OKAY != res)
    {
        return res;
    }
    res = net.UpdateBeliefs();
    if (DSL_OKAY != res)
    {
        return res;
    }
    PrintAllPosteriors(net);
    return DSL_OKAY;
}
