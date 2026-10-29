#include <frzg.h>
#include <memory.h>
#include <find.h>

void PostFrzgLoad(FRZG *pfrzg)
{
    MRG *pmrg = &pfrzg->mrg;
    PostLoLoad(pfrzg);

    SW *psw = pfrzg->psw;
    pmrg->apalo = (ALO **)PvAllocSwImpl(pfrzg->coid * sizeof(ALO *));

    for (int ioid = 0; ioid < pfrzg->coid; ioid++)
    {
        ALO *palo = (ALO *)PloFindSwChild(psw, pfrzg->aoid[ioid], pfrzg->paloParent);
        if (palo != NULL)
        {
            pmrg->apalo[pmrg->cpalo++] = palo;
        }
    }

    AddSwMergeGroup(psw, &pfrzg->mrg);
}

void AddFrzgObject(FRZG *pfrzg, OID oid)
{
    pfrzg->aoid[pfrzg->coid++] = oid;
}
