// tutorial5.cpp
// Tutorial5 loads the XDSL file created by Tutorial4,
// then performs the series of inference calls,
// changing evidence each time.

#include "smile.h"
#include <cstdio>


static int SetEvidenceById(
    DSL_network &net, const char *nodeId, const char *outcomeId);
static void PrintFinancialGain(DSL_network &net, int gainHandle);


int Tutorial5()
{
    printf("Starting Tutorial5...\n");

    DSL_errorH().RedirectToFile(stdout);

    DSL_network net;
    // load the network created by Tutorial4
    int res = net.ReadFile("tutorial4.xdsl");
    if (DSL_OKAY != res)
    {
        printf(
            "Network load failed, did you run Tutorial4 before Tutorial5?\n");
        return res;
    }

    int gain = net.FindNode("Gain");
    if (gain < 0)
    {
        printf("Gain node not found.");
        return gain;
    }

    printf("No evidence set.\n");
    if (DSL_OKAY != net.UpdateBeliefs())
    {
        return res;
    }
    PrintFinancialGain(net, gain);

    printf("\nSetting Forecast=Good.\n");
    SetEvidenceById(net, "Forecast", "Good");
    if (DSL_OKAY != net.UpdateBeliefs())
    {
        return res;
    }
    PrintFinancialGain(net, gain);


    printf("\nAdding Economy=Up\n");
    SetEvidenceById(net, "Economy", "Up");
    if (DSL_OKAY != net.UpdateBeliefs())
    {
        return res;
    }
    PrintFinancialGain(net, gain);

    printf("\nTutorial5 complete.\n");
    return DSL_OKAY;
}


// PrintMatrix displays each probability entry in the matrix in the separate 
// line, preceeded by the information about node and parent outcomes the entry 
// relates to.
// The coordinates of the matrix are ordered as P1,...,Pn,S
// where Pi is the outcome index of i-th parent and S is the outcome of the node.
// If node type is utility, then S collapses and the last coordinate is 
// always zero.
static void PrintMatrix(
    DSL_network &net, const DSL_Dmatrix &mtx, const char *prefix, 
    const DSL_idArray *outcomes, const DSL_intArray &parents)
{
    int dimCount = mtx.GetNumberOfDimensions();
    DSL_intArray coords(dimCount);
    coords.FillWith(0);

    // elemIdx and coords will be moving in sync
    for (int elemIdx = 0; elemIdx < mtx.GetSize(); elemIdx++)
    {
        if (NULL != outcomes)
        {
            const char *outcome = (*outcomes)[coords[dimCount - 1]];
            printf("    %s(%s", prefix, outcome);
        }
        else
        {
            printf("    %s(", prefix);
        }

        if (dimCount > 1)
        {
            if (NULL != outcomes)
            {
                printf("|");
            }
        
            for (int parentIdx = 0; parentIdx < dimCount - 1; parentIdx++)
            {
                if (parentIdx > 0) printf(",");
                DSL_node *parentNode = net.GetNode(parents[parentIdx]);
                const DSL_idArray &parentOutcomes = 
                    *parentNode->Definition()->GetOutcomesNames();
                printf("%s=%s", 
                    parentNode->GetId(), parentOutcomes[coords[parentIdx]]);
            }
        }

        double prob = mtx[elemIdx];
        printf(")=%g\n", prob);

        mtx.NextCoordinates(coords);
    }
}

static void PrintFinancialGain(DSL_network &net, int gainHandle)
{
    DSL_node *node = net.GetNode(gainHandle);
    const char *nodeName = node->Info().Header().GetName();
    printf("%s:\n", nodeName);
    DSL_nodeValue *val = node->Value();
    const DSL_Dmatrix &mtx = *val->GetMatrix();
    const DSL_intArray &parents = val->GetIndexingParents();
    PrintMatrix(net, mtx, "Utility", NULL, parents);
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
