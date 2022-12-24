// tutorial6.cpp
// Tutorial6 creates a dynamic Bayesian network (DBN),
// performs the inference, then saves the model to disk.

#include "smile.h"
#include <cstdio>


static int CreateCptNode(
    DSL_network &net, const char *id,
    const char *name, const char *outcomes[],
    int xPos, int yPos);

static void UpdateAndShowTemporalResults(DSL_network &net);


int Tutorial6()
{
    printf("Starting Tutorial6...\n");

    DSL_errorH().RedirectToFile(stdout);
    
    DSL_network net;

    const char *LOCATION_OUTCOMES[] = { "Pittsburgh", "Sahara", NULL };
    int loc = CreateCptNode(net, "Location", "Location",
        LOCATION_OUTCOMES, 160, 360);

    const char *BOOL_OUTCOMES[] = { "true", "false", NULL };
    int rain = CreateCptNode(net, "Rain", "Rain",
        BOOL_OUTCOMES, 380, 240);

    int umb = CreateCptNode(net, "Umbrella", "Umbrella",
        BOOL_OUTCOMES, 300, 100);

    net.SetTemporalType(rain, dsl_temporalType::dsl_plateNode);
    net.SetTemporalType(umb, dsl_temporalType::dsl_plateNode);

    net.AddArc(loc, rain);
    net.AddTemporalArc(rain, rain, 1);
    net.AddArc(rain, umb);

    DSL_nodeDefinition *rainDef = net.GetNode(rain)->Definition();
    DSL_doubleArray rainProbs(rainDef->GetMatrix()->GetSize());
    rainProbs[0] = 0.7;  // P(Rain=true |Location=Pittsburgh)
    rainProbs[1] = 0.3;  // P(Rain=false|Location=Pittsburgh)
    rainProbs[2] = 0.01; // P(Rain=true |Location=Sahara)
    rainProbs[3] = 0.99; // P(Rain=false|Location=Sahara)
    int res = rainDef->SetDefinition(rainProbs);
    if (DSL_OKAY != res)
    {
        return res;
    }

    rainProbs.SetSize(rainDef->GetTemporalDefinition(1)->GetSize());
    rainProbs[0] = 0.7;   // P(Rain=true |Location=Pittsburgh,Rain[t-1]=true)
    rainProbs[1] = 0.3;   // P(Rain=false|Location=Pittsburgh,Rain[t-1]=true)
    rainProbs[2] = 0.3;   // P(Rain=true |Location=Pittsburgh,Rain[t-1]=false)
    rainProbs[3] = 0.7;   // P(Rain=false|Location=Pittsburgh,Rain[t-1]=false)
    rainProbs[4] = 0.001; // P(Rain=true |Location=Sahara,Rain[t-1]=true)
    rainProbs[5] = 0.999; // P(Rain=false|Location=Sahara,Rain[t-1]=true)
    rainProbs[6] = 0.01;  // P(Rain=true |Location=Sahara,Rain[t-1]=false)
    rainProbs[7] = 0.99;  // P(Rain=false|Location=Sahara,Rain[t-1]=false)
    res = rainDef->SetTemporalDefinition(1, rainProbs);
    if (DSL_OKAY != res)
    {
        return res;
    }

    DSL_nodeDefinition *umbDef = net.GetNode(umb)->Definition();
    DSL_doubleArray umbProbs(umbDef->GetMatrix()->GetSize());
    umbProbs[0] = 0.9; // P(Umbrella=true |Rain=true)
    umbProbs[1] = 0.1; // P(Umbrella=false|Rain=true)
    umbProbs[2] = 0.2; // P(Umbrella=true |Rain=false)
    umbProbs[3] = 0.8; // P(Umbrella=false|Rain=false)
    res = umbDef->SetDefinition(umbProbs);
    if (DSL_OKAY != res)
    {
        return res;
    }

    net.SetNumberOfSlices(5);

    printf("Performing update without evidence.\n");
    UpdateAndShowTemporalResults(net);

    printf("Setting Umbrella[t=1] to true and Umbrella[t=3] to false.\n");
    net.GetNode(umb)->Value()->SetTemporalEvidence(1, 0);
    net.GetNode(umb)->Value()->SetTemporalEvidence(3, 1);
    UpdateAndShowTemporalResults(net);

    res = net.WriteFile("tutorial6.xdsl");
    if (DSL_OKAY != res)
    {
        return res;
    }
    
    printf("Tutorial6 complete: Network written to tutorial6.xdsl\n");
    return DSL_OKAY;
}


static void UpdateAndShowTemporalResults(DSL_network &net)
{
    net.UpdateBeliefs();
    int sliceCount = net.GetNumberOfSlices();
    for (int h = net.GetFirstNode(); h >= 0; h = net.GetNextNode(h))
    {
        if (net.GetTemporalType(h) == dsl_temporalType::dsl_plateNode)
        {
            DSL_node *node = net.GetNode(h);
            int outcomeCount = node->Definition()->GetNumberOfOutcomes();
            printf("Temporal beliefs for %s:\n", node->GetId());
            const DSL_Dmatrix *mtx = node->Value()->GetMatrix();
            for (int sliceIdx = 0; sliceIdx < sliceCount; sliceIdx++)
            {
                printf("\tt=%d:", sliceIdx);
                for (int i = 0; i < outcomeCount; i++)
                {
                    printf(" %f", (*mtx)[sliceIdx * outcomeCount + i]);
                }
                printf("\n");
            }
        }
    }
    printf("\n");
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
