// hello.cpp

#include <cstdio>
#include <smile.h>
#include "smile_license.h" // your licensing key

int main()
{
    DSL_errorH().RedirectToFile(stdout);
    DSL_network net;
    int res = net.ReadFile("VentureBN.xdsl");
    if (DSL_OKAY != res)
    {
        return res;
    }

    int handle = net.FindNode("Forecast");
    if (handle < 0)
    {
        return handle;
    }

    DSL_node *f = net.GetNode(handle);
    int idx =
        f->Definition()->GetOutcomesNames()->FindPosition("Moderate");
    if (idx < 0)
    {
        return idx;
    }
    f->Value()->SetEvidence(idx);

    net.UpdateBeliefs();

    handle = net.FindNode("Success");
    if (handle < 0)
    {
        return handle;
    }
    DSL_node *s = net.GetNode(handle);
    const DSL_Dmatrix &beliefs = *s->Value()->GetMatrix();
    const DSL_idArray &outcomes =
        *s->Definition()->GetOutcomesNames();
    for (int i = 0; i < outcomes.NumItems(); i++)
    {
        printf("%s=%g\n", outcomes[i], beliefs[i]);
    }

    return DSL_OKAY;
}
