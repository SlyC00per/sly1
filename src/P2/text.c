#include <gcc/math.h>
#include <text.h>
#include <memory.h>
#include <util.h>

int CchParsePchzInt(char *pch, int *pn)
{
    int nSign = (*pch == '-') ? -1 : 1;
    int n = 0;
    int cch = 0;
    if (nSign == -1)
    {
        pch++;
        cch = 1;
    }

    while (*pch >= '0' && *pch <= '9')
    {
        n = n * 10 - '0';
        n += *pch;
        pch++;
        cch++;
    }

    *pn = n * nSign;
    return cch;
}

INCLUDE_ASM("asm/nonmatchings/P2/text", CchParsePchzTft__FPcP3TFTPPc);

static inline void WriteOstrmChPad(OSTRM *postrm, char ch, int cchPad)
{
    while (cchPad-- > 0)
    {
        if (postrm->CbWrite(&ch, 1) != 1)
        {
            break;
        }
    }
}

static inline void WriteOstrmPchz(OSTRM *postrm, char *pchz)
{
    postrm->CbWrite(pchz, strlen(pchz));
}

void WriteTftPchz(TFT *ptft, OSTRM *postrm, char *pchz)
{
    if (pchz == NULL)
    {
        pchz = "(null)";
    }

    char chPad;
    int cchWrite = strlen(pchz);
    int cchLeftPad = 0;
    int cchRightPad = 0;
    if (ptft->cchPrecision != 0 && ptft->cchPrecision < cchWrite)
    {
        cchWrite = ptft->cchPrecision;
    }
    else if (ptft->fZeroPrecision != 0)
    {
        cchWrite = 0;
    }

    if (cchWrite < ptft->cchWidth)
    {
        chPad = ptft->fZero != 0 ? '0' : ' ';
        int cchPad = ptft->cchWidth - cchWrite;
        if (ptft->fMinus != 0)
        {
            cchRightPad = cchPad;
        }
        else
        {
            cchLeftPad = cchPad;
        }
    }

    WriteOstrmChPad(postrm, chPad, cchLeftPad);
    postrm->CbWrite(pchz, cchWrite);
    WriteOstrmChPad(postrm, ' ', cchRightPad);
}

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
    ".align 4\n\t"
    ".section .text\n\t"
);

uint UnRadixFromChConversion(char chConversion)
{
    switch (chConversion)
    {
    case 'd':
    case 'i':
    case 'u':
        return 10;
    case 'B':
    case 'b':
        return 2;
    case 'o':
        return 8;
    case 'x':
        return 16;
    default:
        return 10;
    }
}

__asm__(
    ".section .rodata\n\t"
    ".align 3\n\t"
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

void WriteTftUl(TFT *ptft, OSTRM *postrm, unsigned long ul)
{
    char achzNum[65];
    char achzPrefix[5];
    int cchPrefix;
    int cchZeroPad;
    int cchNum;

    if (ptft->fZeroPrecision != 0 || ptft->cchPrecision != 0)
    {
        ptft->fZero = 0;
    }

    if (ptft->fZeroPrecision != 0 && ul == 0)
    {
        cchPrefix = 0;
        cchZeroPad = 0;
        cchNum = 0;
    }
    else
    {
        int fNeg = FNegTftUl(ptft, (UL *)&ul);
        cchZeroPad = ptft->cchPrecision;
        achzPrefix[0] = '\0';

        switch (ptft->chConversion)
        {
        case 'd':
        case 'i':
            if (fNeg)
            {
                strcpy(achzPrefix, "-");
            }
            else if (ptft->fPlus != 0)
            {
                strcpy(achzPrefix, "+");
            }
            else if (ptft->fSpace != 0)
            {
                strcpy(achzPrefix, " ");
            }
            cchZeroPad -= strlen(achzPrefix);
            break;
        case 'b':
            if (ptft->fHash != 0)
            {
                strcpy(achzPrefix, "0b");
            }
            break;
        case 'o':
            if (ptft->fHash != 0)
            {
                strcpy(achzPrefix, "0");
            }
            break;
        case 'x':
            if (ptft->fHash != 0)
            {
                strcpy(achzPrefix, "0x");
            }
            break;
        }

        cchPrefix = strlen(achzPrefix);
        cchNum = CchAchFromUl(achzNum, 64, ul, UnRadixFromChConversion(ptft->chConversion));
        cchZeroPad -= cchNum;
        if (cchZeroPad < 0)
        {
            cchZeroPad = 0;
        }
    }

    achzNum[cchNum] = '\0';
    if (ptft->fUpperize != 0)
    {
        UpperizePchz(achzPrefix);
        UpperizePchz(achzNum);
    }

    int cchTotal = cchPrefix + cchZeroPad + cchNum;
    int cchLeftPad = 0;
    int cchRightPad = 0;
    if (cchTotal < ptft->cchWidth)
    {
        int cchPad = ptft->cchWidth - cchTotal;
        if (ptft->fMinus != 0)
        {
            cchRightPad = cchPad;
        }
        else if (ptft->fZero != 0 && cchNum != 0)
        {
            cchZeroPad += cchPad;
        }
        else
        {
            cchLeftPad = cchPad;
        }
    }

    WriteOstrmChPad(postrm, ' ', cchLeftPad);
    postrm->CbWrite(achzPrefix, cchPrefix);
    WriteOstrmChPad(postrm, '0', cchZeroPad);
    postrm->CbWrite(achzNum, cchNum);
    WriteOstrmChPad(postrm, ' ', cchRightPad);
}

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

int CchAchFromDouble(char *achDest, int cchDest, TFT *ptft, double g)
{
    if ((*(ulong *)&g & 0x7FF0000000000000) == 0x7FF0000000000000)
    {
        strcpy(achDest, (*(ulong *)&g & 0xFFFFFFFFFFFFF) ? "(nan)" : "(inf)");
        return strlen(achDest);
    }

    int expFirst = ExpFirstFromG(g);
    char *pch = achDest;
    int fENotation = ptft->chConversion == 'e';
    if (ptft->chConversion == 'g')
    {
        if (expFirst < -4 || expFirst >= ptft->cchPrecision)
        {
            fENotation = 1;
            ptft->cchPrecision--;
        }
    }

    int expDecimal = fENotation ? expFirst : 0;
    double gRound = g + (double)powf(10.0f, (float)(expDecimal - ptft->cchPrecision)) * 0.5;

    int expNewFirst = ExpFirstFromG(gRound);
    if (expFirst < ExpFirstFromG(gRound))
    {
        expFirst = expNewFirst;
    }

    int cchPre;
    int expNext;
    if (fENotation)
    {
        cchPre = 1;
        expNext = expFirst;
    }
    else if (expFirst >= 0)
    {
        cchPre = expFirst + 1;
        expNext = expFirst;
    }
    else
    {
        cchPre = 1;
        expNext = 0;
    }

    while (cchPre-- > 0)
    {
        *pch++ = ChDigitFromG(gRound, expFirst, expNext--);
    }

    if (ptft->fZeroPrecision == 0)
    {
        *pch++ = '.';
        int cchPost = ptft->cchPrecision;
        while (cchPost-- > 0)
        {
            *pch++ = ChDigitFromG(gRound, expFirst, expNext--);
        }

        if (ptft->chConversion == 'g' && achDest < pch)
        {
            while (achDest < pch && pch[-1] == '0')
            {
                pch--;
            }
            if (achDest < pch && pch[-1] == '.')
            {
                pch--;
            }
        }
    }

    if (fENotation)
    {
        *pch++ = 'e';
        if (expFirst < 0)
        {
            *pch++ = '-';
            expFirst = -expFirst;
        }
        else
        {
            *pch++ = '+';
        }

        int nDiv = (int)powf(10.0f, 2.0f);
        int cchPost = 2;
        do
        {
            *pch++ = s_achzNumerals[expFirst / nDiv];
            expFirst = (expFirst % nDiv) * 10;
        } while (--cchPost >= 0);
    }

    return pch - achDest;
}

void WriteTftDouble(TFT *ptft, OSTRM *postrm, double g)
{
    char achzNum[438];
    char achzPrefix[5];
    int cchZeroPad = 0;

    if (ptft->cchPrecision == 0 && ptft->fZeroPrecision == 0)
    {
        ptft->cchPrecision = 6;
    }
    else if (ptft->fZeroPrecision != 0 && ptft->chConversion == 'g')
    {
        ptft->cchPrecision = 1;
    }

    int fNeg = g < 0.0;
    if (fNeg)
    {
        g = -g;
    }

    achzPrefix[0] = '\0';
    if (fNeg)
    {
        strcpy(achzPrefix, "-");
    }
    else if (ptft->fPlus != 0)
    {
        strcpy(achzPrefix, "+");
    }
    else if (ptft->fSpace != 0)
    {
        strcpy(achzPrefix, " ");
    }

    int cchPrefix = strlen(achzPrefix);
    int cchNum = CchAchFromDouble(achzNum, 437, ptft, g);
    achzNum[cchNum] = '\0';
    if (ptft->fUpperize != 0)
    {
        UpperizePchz(achzNum);
    }

    int cchTotal = cchPrefix + cchNum;
    int cchLeftPad = 0;
    int cchRightPad = 0;
    if (cchTotal < ptft->cchWidth)
    {
        int cchPad = ptft->cchWidth - cchTotal;
        if (ptft->fMinus != 0)
        {
            cchRightPad = cchPad;
        }
        else if (ptft->fZero != 0 && cchNum != 0)
        {
            cchZeroPad = cchPad;
        }
        else
        {
            cchLeftPad = cchPad;
        }
    }

    WriteOstrmChPad(postrm, ' ', cchLeftPad);
    postrm->CbWrite(achzPrefix, cchPrefix);
    WriteOstrmChPad(postrm, '0', cchZeroPad);
    postrm->CbWrite(achzNum, cchNum);
    WriteOstrmChPad(postrm, ' ', cchRightPad);
}

void WriteTft(TFT *ptft, OSTRM *postrm, va_list *pval, char *pch, int cch)
{
    unsigned long ul;
    int iRow;
    int iCol;

    switch (ptft->chConversion)
    {
    case '%':
        postrm->CbWrite(&ptft->chConversion, 1);
        break;
    case 'c':
    {
        char ch = va_arg(*pval, char);
        postrm->CbWrite(&ch, 1);
        break;
    }
    case 's':
        WriteTftPchz(ptft, postrm, va_arg(*pval, char *));
        break;
    case 'p':
        ptft->cchPrecision = 8;
        ptft->chConversion = 'x';
        ptft->tfvw = TFVW_32;
    case 'b':
    case 'd':
    case 'i':
    case 'o':
    case 'u':
    case 'x':
    {
        switch (ptft->tfvw)
        {
        case TFVW_32:
        case TFVW_16:
            ul = va_arg(*pval, uint);
            break;
        case TFVW_64:
            ul = va_arg(*pval, unsigned long);
            break;
        }
        WriteTftUl(ptft, postrm, ul);
        break;
    }
    case 'e':
    case 'f':
    case 'g':
        if (ptft->cCol != 0)
        {
            float *ag = va_arg(*pval, float *);
            char *pchzNl = "\n";
            char *pchzRowSep = ",\n";
            char *pchzRowBegin = "{ ";
            char *pchzColSep = ", ";
            char *pchzRowEnd = " }";
            if (ptft->cRow >= 2)
            {
                WriteOstrmPchz(postrm, pchzNl);
            }
            for (iRow = 0; iRow < ptft->cRow; iRow++)
            {
                if (iRow != 0)
                {
                    WriteOstrmPchz(postrm, pchzRowSep);
                }
                WriteOstrmPchz(postrm, pchzRowBegin);
                for (iCol = 0; iCol < ptft->cCol; iCol++)
                {
                    if (iCol != 0)
                    {
                        WriteOstrmPchz(postrm, pchzColSep);
                    }
                    WriteTftDouble(ptft, postrm, *ag++);
                }
                WriteOstrmPchz(postrm, pchzRowEnd);
            }
            if (ptft->cRow >= 2)
            {
                WriteOstrmPchz(postrm, pchzNl);
            }
        }
        else
        {
            WriteTftDouble(ptft, postrm, va_arg(*pval, double));
        }
        break;
    case 'n':
        switch (ptft->tfvw)
        {
        case TFVW_32:
        {
            int *pn = va_arg(*pval, int *);
            *pn = postrm->CbWritten();
            break;
        }
        case TFVW_16:
        {
            short *pw = va_arg(*pval, short *);
            *pw = postrm->CbWritten();
            break;
        }
        case TFVW_64:
        {
            long *pl = va_arg(*pval, long *);
            *pl = postrm->CbWritten();
            break;
        }
        }
        break;
    default:
        postrm->CbWrite(pch, cch);
        break;
    }
}

int CchOstrmPrintf(OSTRM *postrm, char *pchzFormat, va_list val)
{
    TFT tft;
    char *pchMic = pchzFormat;

    if (*pchMic != ' ')
    {
        do
        {
            char *pchMac = strchr(pchMic, '%');
            int cch;
            if (pchMac != NULL)
            {
                cch = pchMac - pchMic;
            }
            else
            {
                cch = strlen(pchMic);
                pchMac = pchMic + cch;
            }

            if (cch != 0)
            {
                if (postrm->CbWrite(pchMic, cch) < cch)
                {
                    break;
                }
                pchMic = pchMac;
            }

            if (*pchMic == '%')
            {
                cch = CchParsePchzTft(pchMic, &tft, &val);
                WriteTft(&tft, postrm, &val, pchMic, cch);
                pchMic += cch;
            }
        } while (*pchMic != ' ');
    }

    postrm->Flush();
    return postrm->CbWritten();
}

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
