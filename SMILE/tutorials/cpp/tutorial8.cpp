// tutorial8.cpp
// Tutorial8 loads continuous model from the XDSL file written by Tutorial7,
// then adds discrete nodes to create a hybrid model. Inference is performed
// and model is saved to disk.

#include "smile.h"
#include <cstdio>

static int CreateCptNode(
    DSL_network &net, const char *id,
    const char *name, const char *outcomes[],
    int xPos, int yPos);

static int SetEvidenceById(
    DSL_network &net, const char *nodeId, const char *outcomeId);

static void UpdateAndShowStats(DSL_network &net);


int Tutorial8()
{
    printf("Starting Tutorial8...\n");

    DSL_errorH().RedirectToFile(stdout);

    DSL_network net;
    int res = net.ReadFile("tutorial7.xdsl");
    if (DSL_OKAY != res)
    {
        printf(
            "Network load failed, did you run Tutorial7 before Tutorial8?\n");
        return res;
    }

    const char *ZONE_OUTCOMES[] = { "Temperate", "Desert", NULL };
    CreateCptNode(net, 
        "zone", "Climate Zone", ZONE_OUTCOMES,
        60, 20);

    int toa = net.FindNode("toa");
    if (toa < 0)
    {
        printf("Outside air temperature node not found.\n");
        return toa;
    }
    DSL_equation *eq =  
        static_cast<DSL_equation *>(net.GetNode(toa)->Definition());
    eq->SetEquation("toa=If(zone=\"Desert\",Normal(22,5),Normal(11,10))");

    const char *PERCEIVED_OUTCOMES[] = { "Hot", "Warm", "Cold", NULL };
    int perceived = CreateCptNode(net,
        "perceived", "Perceived Temperature", PERCEIVED_OUTCOMES,
        60, 300);
    net.AddArc(toa, perceived);

    DSL_nodeDefinition *percDef = net.GetNode(perceived)->Definition();
    DSL_doubleArray percProbs(percDef->GetMatrix()->GetSize());
    percProbs[0]  = 0;    // P(perceived=Hot |toa in -10..0)
    percProbs[1]  = 0.02; // P(perceived=Warm|toa in -10..0)
    percProbs[2]  = 0.98; // P(perceived=Cold|toa in -10..0)
    percProbs[3]  = 0.05; // P(perceived=Hot |toa in 0..10)
    percProbs[4]  = 0.15; // P(perceived=Warm|toa in 0..10)
    percProbs[5]  = 0.80; // P(perceived=Cold|toa in 0..10)
    percProbs[6]  = 0.10; // P(perceived=Hot |toa in 10..20)
    percProbs[7]  = 0.80; // P(perceived=Warm|toa in 10..20)
    percProbs[8]  = 0.10; // P(perceived=Cold|toa in 10..20)
    percProbs[9]  = 0.80; // P(perceived=Hot |toa in 20..30)
    percProbs[10] = 0.15; // P(perceived=Warm|toa in 20..30)
    percProbs[11] = 0.05; // P(perceived=Cold|toa in 20..30)
    percProbs[12] = 0.98; // P(perceived=Hot |toa in 30..40)
    percProbs[13] = 0.02; // P(perceived=Warm|toa in 30..40)
    percProbs[14] = 0;    // P(perceived=Cold|toa in 30..40)
    percDef->SetDefinition(percProbs);

    SetEvidenceById(net, "zone", "Temperate");
    printf("Results in temperate zone:\n");
    UpdateAndShowStats(net);

    SetEvidenceById(net, "zone", "Desert");
    printf("Results in desert zone:\n");
    UpdateAndShowStats(net);

    res = net.WriteFile("tutorial8.xdsl");
    if (DSL_OKAY != res)
    {
        return res;
    }

    printf("Tutorial8 complete: Network written to tutorial8.xdsl\n");
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


static void ShowStats(DSL_network &net, int nodeHandle)
{
    const char *nodeId = net.GetNode(nodeHandle)->GetId();

    DSL_valEqEvaluation *eqVal =
        static_cast<DSL_valEqEvaluation *>(net.GetNode(nodeHandle)->Value());

    if (eqVal->IsEvidence())
    {
        double v;
        eqVal->GetEvidence(v);
        printf("%s has evidence set (%g)\n", nodeId, v);
        return;
    }

    const std::vector<double> &discBeliefs = eqVal->GetDiscBeliefs();
    if (discBeliefs.empty())
    {
        double mean, stddev, vmin, vmax;
        eqVal->GetStats(mean, stddev, vmin, vmax);
        printf("%s: mean=%g stddev=%g min=%g max=%g\n",
            nodeId, mean, stddev, vmin, vmax);
    }
    else
    {
        DSL_equation *eqDef =
            static_cast<DSL_equation *>(net.GetNode(nodeHandle)->Definition());
        const DSL_equation::IntervalVector &iv = eqDef->GetDiscIntervals();
        printf("%s is discretized.\n", nodeId);
        double loBound, hiBound;
        eqDef->GetBounds(loBound, hiBound);
        double lo = loBound;
        for (unsigned i = 0; i < discBeliefs.size(); i++)
        {
            double hi = iv[i].second;
            printf("\tP(%s in %g..%g)=%g\n", nodeId, lo, hi, discBeliefs[i]);
            lo = hi;
        }
    }
}


static void UpdateAndShowStats(DSL_network &net)
{
    net.UpdateBeliefs();
    for (int h = net.GetFirstNode(); h >= 0; h = net.GetNextNode(h))
    {
        if (net.GetNode(h)->Definition()->GetType() == DSL_EQUATION)
        { 
           ShowStats(net, h);
        }
    }
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
