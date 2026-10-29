#include <bis.h>
#include <util.h>
#include <memory.h>
#include <989snd.h>
#include <sdk/libcdvd.h>
#include <lib/libkernl/filestub.h>

CBinaryInputStream::CBinaryInputStream(int cbSpool, void *pvSpool, GRFBIS grfbis)
{
    m_bisk = BISK_Nil;
    m_grfbis = grfbis;
    m_pb = m_pbRaw = NULL;
    m_cb = m_cbRaw = 0;
    m_pprog = NULL;
    m_grfDecomp = 0;
    m_cbSpillOver = 0;

    m_abSpool = (byte *)(((int)pvSpool + 0x3f) & ~0x3f);
    m_cbSpool = cbSpool - ((int)m_abSpool - (int)pvSpool);
}

CBinaryInputStream::~CBinaryInputStream()
{
    Close();
}

int CBinaryInputStream::FOpenSector(uint isector, uint cb)
{
    m_bisk = BISK_Cd;
    m_isector = isector;
    m_cbFile = cb;
    m_cbRemaining = cb;
    m_cbAsyncRemaining = cb;
    m_pbRaw = m_abSpool;
    m_cbAsyncComplete = 0;
    m_cbPartialRead = 0;
    m_cbAsyncRequest = 0;

    int cbSpool = m_cbSpool;
    if (cbSpool < 0)
    {
        cbSpool += 0xfffff;
    }

    m_cbufFill = 0;

    int cbuf = 2;
    int shr = 20;
    if ((cbSpool >> shr) > cbuf)
    {
        cbuf = cbSpool >> shr;
    }

    m_ibufMic = 0;
    m_cbuf = cbuf;
    m_ibufMac = cbuf - 1;

    return 1;
}

void CBinaryInputStream::OpenMemory(int cb, void *pv)
{
    m_bisk = BISK_Mem;
    m_pb = (byte *)pv;
    m_cb = cb;
}

int CBinaryInputStream::FOpenFile(CFileLocation *pfl)
{
    return FOpenSector(pfl->m_fcl.isector, pfl->m_fcl.cb);
}

void CBinaryInputStream::Close()
{
    switch (m_bisk)
    {
        case BISK_Host:
        {
            if (m_fd >= 0)
                sceClose(m_fd);
            break;
        }
        case BISK_Cd:
        {
            if (m_cbAsyncRequest)
            {
                if (m_grfbis & 0x2)
                {
                    sceCdBreak();
                }
                else
                {
                    snd_StreamSafeCdBreak();
                }
            }
            break;
        }
    }

    m_bisk = BISK_Nil;
    m_pbRaw = 0;
    m_pb = 0;
    m_cbRaw = 0;
    m_cb = 0;
    m_grfDecomp = 0;
    m_cbSpillOver = 0;
}

void CBinaryInputStream::DecrementCdReadLimit(int cb)
{
    m_cbAsyncRemaining -= cb;
}

INCLUDE_ASM("asm/nonmatchings/P2/bis", PumpCd__18CBinaryInputStream);

void CBinaryInputStream::PumpHost()
{
    m_pbRaw = m_abSpool;
    m_cbRaw = sceRead(m_fd, m_pbRaw, NMin(0x8000, m_cbSpool, m_cbRemaining));
}

void CBinaryInputStream::Pump()
{
    switch (m_bisk)
    {
        case BISK_Host:
            PumpHost();
            break;
        case BISK_Cd:
            PumpCd();
            break;
        default:
            break;
    }

    // TODO: This code matches perfectly, but it could look much better.
    int iVar1 = m_cbRemaining - STRUCT_OFFSET(this, 0x24, int);
    int zero = 0;
    int remain = (iVar1 > zero) ? iVar1 : 0;

    m_cbRemaining = remain;

    if (m_pprog)
    {
        m_pprog->SetRemain(remain);
    }
}

INCLUDE_ASM("asm/nonmatchings/P2/bis", Decompress__18CBinaryInputStream);

void CBinaryInputStream::Read(int cb, void *pv)
{
    byte *pb = (byte *)pv;

    if (m_cb < 0)
        return;

    while (cb > 0)
    {
        if (m_cb == 0)
        {
            if (m_grfbis & 0x4)
            {
                Decompress();
            }
            else
            {
                Pump();
                m_cb = m_cbRaw;
                m_pb = m_pbRaw;
                m_pbRaw += m_cbRaw;
                m_cbRaw = 0;
            }

            if (m_cb == 0)
            {
                m_cb = -1;
                return;
            }
        }

        int cbCopy = NMin(cb, m_cb);
        if (pb)
        {
            CopyAb(pb, m_pb, cbCopy);
            pb += cbCopy;
        }

        m_pb += cbCopy;
        m_cb -= cbCopy;
        cb -= cbCopy;
    }
}

void CBinaryInputStream::Align(int n)
{
    int pbAligned = ((int)m_pb + (n - 1)) & -n;
    m_cb -= (pbAligned - (int)m_pb);
    m_pb = (byte *)pbAligned;
}

uchar CBinaryInputStream::U8Read()
{
    if (m_cb >= 1)
    {
        uchar b = *m_pb;
        m_pb++;
        m_cb--;
        return b;
    }

    uchar b;
    Read(sizeof(uchar), &b);
    return b;
}

ushort CBinaryInputStream::U16Read()
{
    if (m_cb >= 2)
    {
        ushort u = m_pb[0];
        ushort v = m_pb[1];
        m_pb += 2;
        m_cb -= 2;
        return (v << 8) | u;
    }

    ushort v;
    Read(sizeof(ushort), &v);
    return v;
}

uint CBinaryInputStream::U32Read()
{
    if (m_cb >= 4)
    {
        uint u = m_pb[0];
        uint v = m_pb[1];
        uint w = m_pb[2];
        uint x = m_pb[3];
        m_pb += 4;
        m_cb -= 4;
        return u + (v * 0x100) + (w * 0x10000) + (x * 0x1000000);
    }

    uint v;
    Read(sizeof(uint), &v);
    return v;
}

JUNK_NOP();
JUNK_ADDIU(20);

char CBinaryInputStream::S8Read()
{
    if (m_cb >= 1)
    {
        char b = *m_pb;
        m_pb += 1;
        m_cb -= 1;
        return b;
    }

    char b;
    Read(sizeof(char), &b);
    return b;
}

short CBinaryInputStream::S16Read()
{
    if (m_cb >= 2)
    {
        short v = m_pb[1] << 8;
        v |= m_pb[0];
        m_pb += 2;
        m_cb -= 2;
        return v;
    }

    short v;
    Read(sizeof(short), &v);
    return v;
}

int CBinaryInputStream::S32Read()
{
    if (m_cb >= 4)
    {
        int u = m_pb[0];
        int v = m_pb[1];
        int w = m_pb[2];
        char x = m_pb[3];
        m_pb += 4;
        m_cb -= 4;
        return u + (v << 8) + (w << 16) + (x << 24);
    }

    int v;
    Read(sizeof(int), &v);
    return v;
}

JUNK_NOP();
JUNK_ADDIU(20);

float CBinaryInputStream::F32Read()
{
    if (m_cb >= 4)
    {
        int g = (m_pb[0] + (m_pb[1] << 8) + (m_pb[2] << 16) + (m_pb[3] << 24));
        m_pb += 4;
        m_cb -= 4;
        return *(float *)&g;
    }

    float g;
    Read(sizeof(float), &g);
    return g;
}

void CBinaryInputStream::ReadVector(VECTOR *pvec)
{
    // NOTE: VECTOR has one extra float of padding.
    Read(sizeof(VECTOR) - sizeof(float), pvec);
}

void CBinaryInputStream::ReadVector4(VECTOR4 *pvec)
{
    Read(sizeof(VECTOR4), pvec);
}

void CBinaryInputStream::ReadMatrix(MATRIX3 *pmat)
{
    const int cbRow = sizeof(float) * 3;
    Read(cbRow, &pmat->aag[0]);
    Read(cbRow, &pmat->aag[1]);
    Read(cbRow, &pmat->aag[2]);
}

void CBinaryInputStream::ReadMatrix4(MATRIX4 *pmat)
{
    // TODO: This might be possible to clean up, but I wasn't able to.
    // The compiler seems to insist that must we have pointer to each row. -545u
    float *row0 = &pmat->aag[0][0];
    float *row1 = &pmat->aag[1][0];
    float *row2 = &pmat->aag[2][0];
    float *row3 = &pmat->aag[3][0];

    const int cbRow = sizeof(float) * 3;
    Read(cbRow, row0);
    Read(cbRow, row1);
    Read(cbRow, row2);
    Read(cbRow, row3);

    row0[3] = 0.0f;
    row1[3] = 0.0f;
    row2[3] = 0.0f;
    row3[3] = 1.0f;
}

INCLUDE_ASM("asm/nonmatchings/P2/bis", ReadGeom__18CBinaryInputStreamP4GEOM);

INCLUDE_ASM("asm/nonmatchings/P2/bis", ReadBspc__18CBinaryInputStreamP4GEOMP4BSPC);

void CBinaryInputStream::ReadVbsp(int *pcvbsp, VBSP **pavbsp)
{
    *pcvbsp = U16Read();
    *pavbsp = (VBSP *)PvAllocSwClearImpl(*pcvbsp * sizeof(VBSP));

    for (int i = 0; i < *pcvbsp; i++)
    {
        VBSP *pvbsp = *pavbsp + i;

        ReadVector(&pvbsp->normal);
        pvbsp->gDot = F32Read();

        uint negIndex = U32Read();
        if (negIndex & 0x80000000)
        {
            pvbsp->pvbspNeg = (VBSP *)negIndex;
        }
        else
        {
            pvbsp->pvbspNeg = *pavbsp + negIndex;
        }

        uint posIndex = U32Read();
        if (posIndex & 0x80000000)
        {
            pvbsp->pvbspPos = (VBSP *)posIndex;
        }
        else
        {
            pvbsp->pvbspPos = *pavbsp + posIndex;
        }
    }
}

void CBinaryInputStream::ReadStringSw(char **pachz)
{
    int length = U16Read();
    char *buffer = (char *)PvAllocSwImpl(length + 1);
    Read(length, buffer);
    buffer[length] = 0;
    *pachz = buffer;
}

JUNK_NOP();
JUNK_ADDIU(50);

void CBinaryInputStream::Unknown1()
{
    ushort size = U16Read();
    Read(size, NULL);
}

void CBinaryInputStream::Unknown2(void **ppv)
{
    uint data[8];
    Read(0x20, &data);

    if (data[1] != data[7])
    {
        *ppv = PvAllocSwCopyImpl(0x20, data);
        return;
    }

    *ppv = NULL;
}
