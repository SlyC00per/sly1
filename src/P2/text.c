#include <gcc/math.h>
#include <text.h>
#include <memory.h>
#include <util.h>

INCLUDE_ASM("asm/nonmatchings/P2/text", CchParsePchzInt__FPcPi);

INCLUDE_ASM("asm/nonmatchings/P2/text", CchParsePchzTft__FPcP3TFTPPc);

INCLUDE_ASM("asm/nonmatchings/P2/text", WriteTftPchz__FP3TFTP5OSTRMPc);

int FNegTftUl(TFT *ptft, UL *pul)
{
    int fNeg = 0;

    switch (ptft->tfvw)
    {
    case TFVW_32:
        pul->unUnused = 0;
        break;
    case TFVW_16:
        pul->ul = pul->uw;
        break;
    case TFVW_64:
        break;
    }

    if (ptft->chConversion == 'd' || ptft->chConversion == 'i')
    {
        switch (ptft->tfvw)
        {
        case TFVW_32:
            if (pul->n < 0)
            {
                pul->n = -pul->n;
                fNeg = 1;
            }
            break;
        case TFVW_16:
            if (pul->w < 0)
            {
                pul->uw = -pul->uw;
                fNeg = 1;
            }
            break;
        case TFVW_64:
            if (pul->l < 0)
            {
                pul->l = -pul->l;
                fNeg = 1;
            }
            break;
        }
    }

    return fNeg;
}

__asm__(
    ".section .rodata\n\t"
    "nonmatching D_0024E2E0\n\t"
    "\n\t"
    "dlabel D_0024E2E0\n\t"
    "    .word 0x6C756E28\n\t"
    "    .word 0x0000296C\n\t"
    "    .word 0x00000000\n\t"
    "    .word 0x00000000\n\t"
    "enddlabel D_0024E2E0\n\t"
    ".section .text\n\t"
);

INCLUDE_ASM("asm/nonmatchings/P2/text", UnRadixFromChConversion__Fc);

__asm__(
    ".section .rodata\n\t"
    ".align 3\n\t"
    "nonmatching D_0024E3D0\n\t"
    "\n\t"
    "dlabel D_0024E3D0\n\t"
    "    .word 0x0000002D\n\t"
    "    .word 0x00000000\n\t"
    "enddlabel D_0024E3D0\n\t"
    "\n\t"
    "nonmatching D_0024E3D8\n\t"
    "\n\t"
    "dlabel D_0024E3D8\n\t"
    "    .word 0x0000002B\n\t"
    "    .word 0x00000000\n\t"
    "enddlabel D_0024E3D8\n\t"
    "\n\t"
    "nonmatching D_0024E3E0\n\t"
    "\n\t"
    "dlabel D_0024E3E0\n\t"
    "    .word 0x00000020\n\t"
    "    .word 0x00000000\n\t"
    "enddlabel D_0024E3E0\n\t"
    "\n\t"
    "nonmatching D_0024E3E8\n\t"
    "\n\t"
    "dlabel D_0024E3E8\n\t"
    "    .word 0x00006230\n\t"
    "    .word 0x00000000\n\t"
    "enddlabel D_0024E3E8\n\t"
    "\n\t"
    "nonmatching D_0024E3F0\n\t"
    "\n\t"
    "dlabel D_0024E3F0\n\t"
    "    .word 0x00000030\n\t"
    "    .word 0x00000000\n\t"
    "enddlabel D_0024E3F0\n\t"
    "\n\t"
    "nonmatching D_0024E3F8\n\t"
    "\n\t"
    "dlabel D_0024E3F8\n\t"
    "    .word 0x00007830\n\t"
    "    .word 0x00000000\n\t"
    "enddlabel D_0024E3F8\n\t"
    ".section .text\n\t"
);

int CchAchFromUl(char *achDest, int cchDest, unsigned long ul, uint unRadix)
{
    char *pchEnd = achDest + cchDest;
    char *pch = pchEnd;
    do
    {
        *--pch = s_achzNumerals[ul % unRadix];
        ul /= unRadix;
    } while (ul != 0 && pch >= achDest);
    int cch = pchEnd - pch;
    if (cch != 0 && achDest < pch)
    {
        CopyAb(achDest, pch, cch);
    }
    return cch;
}

INCLUDE_ASM("asm/nonmatchings/P2/text", WriteTftUl__FP3TFTP5OSTRMUl);

int ExpFirstFromG(double g)
{
    int exp;
    if (g > 0.0)
    {
        double gExp = (double)log10f(g);
        exp = (int)gExp;
        if (gExp < 0.0)
        {
            if (gExp != (double)exp)
            {
                exp--;
            }
        }
    }
    else
    {
        exp = 0;
    }
    return exp;
}

char ChDigitFromG(double g, int expFirst, int exp)
{
    int digit;
    if (exp > expFirst || exp <= expFirst - 15)
    {
        digit = 0;
    }
    else
    {
        double gScaled = GTrunc(g * (double)powf(10.0f, (float)-exp));
        digit = (int)(gScaled - GTrunc(gScaled * 0.1) * 10.0);
    }
    return s_achzNumerals[digit];
}

INCLUDE_ASM("asm/nonmatchings/P2/text", CchAchFromDouble__FPciP3TFTd);

INCLUDE_ASM("asm/nonmatchings/P2/text", WriteTftDouble__FP3TFTP5OSTRMd);

__asm__(
    ".section .rodata\n\t"
    "nonmatching D_0024E468\n\t"
    "\n\t"
    "dlabel D_0024E468\n\t"
    "    .word 0x6E616E28\n\t"
    "    .word 0x00000029\n\t"
    "enddlabel D_0024E468\n\t"
    "\n\t"
    "nonmatching D_0024E470\n\t"
    "\n\t"
    "dlabel D_0024E470\n\t"
    "    .word 0x666E6928\n\t"
    "    .word 0x00000029\n\t"
    "enddlabel D_0024E470\n\t"
    "\n\t"
    "nonmatching D_0024E478\n\t"
    "\n\t"
    "dlabel D_0024E478\n\t"
    "    .word 0x0000000A\n\t"
    "    .word 0x00000000\n\t"
    "enddlabel D_0024E478\n\t"
    "\n\t"
    "nonmatching D_0024E480\n\t"
    "\n\t"
    "dlabel D_0024E480\n\t"
    "    .word 0x00000A2C\n\t"
    "    .word 0x00000000\n\t"
    "enddlabel D_0024E480\n\t"
    "\n\t"
    "nonmatching D_0024E488\n\t"
    "\n\t"
    "dlabel D_0024E488\n\t"
    "    .word 0x0000207B\n\t"
    "    .word 0x00000000\n\t"
    "enddlabel D_0024E488\n\t"
    "\n\t"
    "nonmatching D_0024E490\n\t"
    "\n\t"
    "dlabel D_0024E490\n\t"
    "    .word 0x0000202C\n\t"
    "    .word 0x00000000\n\t"
    "enddlabel D_0024E490\n\t"
    "\n\t"
    "nonmatching D_0024E498\n\t"
    "\n\t"
    "dlabel D_0024E498\n\t"
    "    .word 0x00007D20\n\t"
    "    .word 0x00000000\n\t"
    "enddlabel D_0024E498\n\t"
    ".section .text\n\t"
);

INCLUDE_ASM("asm/nonmatchings/P2/text", WriteTft__FP3TFTP5OSTRMPPcPci);

INCLUDE_ASM("asm/nonmatchings/P2/text", CchOstrmPrintf__FP5OSTRMPcT1);

extern "C" int vprintf(char *pchzFormat, va_list val)
{
    OSTRM stream(-1);
    return CchOstrmPrintf(&stream, pchzFormat, val);
}

extern "C" int printf(char *pchzFormat, ...)
{
    va_list arg;
    va_start(arg, pchzFormat);
    int ret = vprintf(pchzFormat, arg);
    va_end(arg);
    return ret;
}

extern "C" int vsprintf(char *pchzDest, char *pchzFormat, va_list val)
{
    OSTRM ostrm(pchzDest, 0);
    int len = CchOstrmPrintf(&ostrm, pchzFormat, val);
    pchzDest[len] = '\0';
    return len;
}

extern "C" int sprintf(char *pchzDest, char *pchzFormat, ...)
{
    va_list arg;
    va_start(arg, pchzFormat);
    int ret = vsprintf(pchzDest, pchzFormat, arg);
    va_end(arg);
    return ret;
}

extern "C" int _vsnprintf(char *pchzDest, int cchDest, char *pchzFormat, va_list val)
{
    OSTRM ostrm(pchzDest, cchDest);
    int len = CchOstrmPrintf(&ostrm, pchzFormat, val);

    if (len >= cchDest)
    {
        pchzDest[cchDest - 1] = '\0';
        len = -1;
    }
    else
    {
        pchzDest[len] = '\0';
    }

    return len;
}

JUNK_ADDIU(10);

extern "C" int _snprintf(char *pchzDest, int cchDest, char *pchzFormat, ...)
{
    va_list arg;
    va_start(arg, pchzFormat);
    int ret = _vsnprintf(pchzDest, cchDest, pchzFormat, arg);
    va_end(arg);
    return ret;
}

JUNK_WORD(0x27BD0450);

extern "C" uint strlen(const char *pchz)
{
    uint len = 0;

    while (*pchz != '\0')
    {
        pchz++;
        len++;
    }

    return len;
}

extern "C" char *strcpy(char *pchzDst, const char *pchzSrc)
{
    CopyAb(pchzDst, (char *)pchzSrc, strlen(pchzSrc) + 1);
    return pchzDst;
}

extern "C" char *strcpy1(char *pchzDst, char *pchzSrc)
{
    uint dstLength = strlen(pchzDst);
    uint srcLength = strlen(pchzSrc);
    CopyAb(pchzDst + dstLength, pchzSrc, srcLength + 1);
    return pchzDst;
}

extern "C" char *strchr(char *pchz, int ch)
{
    while (*pchz != '\0')
    {
        if (*pchz == ch)
        {
            return pchz;
        }
        pchz++;
    }
    return NULL;
}

JUNK_NOP();
JUNK_ADDIU(30);

void UpperizePchz(char *pchz)
{
    while (*pchz != '\0')
    {
        int ch = *pchz;
        int chUpper = ch - 0x20;
        if (!(_ctype_[ch] & 2))
        {
            chUpper = ch;
        }
        *pchz = chUpper;
        pchz++;
    }
}
