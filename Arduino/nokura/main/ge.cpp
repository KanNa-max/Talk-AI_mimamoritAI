/****************************************************************************
 *
 *   Copyright 2018 Sony Semiconductor Solutions Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Sony Semiconductor Solutions Corporation nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <stdio.h>
#include <string.h>

#include <Camera.h>

#include "ge.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define MSEL     1

/* Command code */

#define COPYCMD  0x4
#define ROPCMD   0x8
#define ABCMD    0xa

/* Command options */

#define SRC16BPP (1 << 10)
#define SCALING  (1 << 12)
#define PATMONO  (1 << 15)

/* Raster operation code */

#define SRCCOPY     0xcc
#define SRCPAINT    0xee
#define SRCAND      0x88
#define SRCINVERT   0x66
#define SRCERASE    0x44
#define NOTSRCCOPY  0x33
#define NOTSRCERASE 0x11
#define MARGECOPY   0xc0
#define MERGEPAINT  0xbb
#define PATCOPY     0xf0
#define PATPAINT    0xfb
#define PATINVERT   0x5a
#define DSTINVERT   0x55

/* Raster operation options */

#define CONV8BPP    (1 << 7)
#define FIXEDCOLOR  (1 << 3)
#define CMPDST      (3 << 1)
#define CMPSRC      (2 << 1)
#define CMPPAT      (1 << 1)
#define WBUNMATCHED (1 << 0)

/* Alpha blending options */

#define ALPHA1BPP   (1 << 15)
#define FIXEDSRC    (1 << 14)
#define MSBFIRST    (1 << 13)

#define CXD56_ADSP_BASE          0x4c000000
#define CXD56_GE2D_BASE           (CXD56_ADSP_BASE  + 0x02101000)

struct ge2dhw {
  volatile uint32_t INTR_ENABLE;
  volatile uint32_t INTR_STAT;
  volatile uint32_t ADDRESS_DESCRIPTOR_START;
  volatile uint32_t CMD_DESCRIPTOR;
};

#define GE2D_INTR_WR_ERR   (1 << 17)
#define GE2D_INTR_RD_ERR   (1 << 16)
#define GE2D_INTR_DSD      (1 << 8)
#define GE2D_INTR_NDE      (1 << 3)
#define GE2D_INTR_NDB      (1 << 2)
#define GE2D_INTR_NDF      (1 << 1)
#define GE2D_INTR_HPU      (1 << 0)

#define GE2D_INTR_ALL (GE2D_INTR_WR_ERR | GE2D_INTR_RD_ERR | \
                       GE2D_INTR_DSD | GE2D_INTR_NDE | GE2D_INTR_NDB | \
                       GE2D_INTR_NDF | GE2D_INTR_HPU)

#define GE2D_EXEC  1

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* Copy command (32 bytes) */

struct ge2d_copycmd_s
{
  uint32_t cmd;                 /* 0x00 */
  uint16_t srch;                /* 0x04 */
  uint16_t srcv;                /* 0x06 */
  uint32_t saddr;               /* 0x08 */
  uint32_t daddr;               /* 0x0c */
  uint16_t spitch;              /* 0x10 */
  uint16_t dpitch;              /* 0x12 */
  uint32_t reserved[3];
} __attribute__((aligned(16)));

/* Raster operation (ROP) command (48 bytes) */

struct ge2d_ropcmd_s
{
  uint16_t cmd;                 /* 0x00 */
  uint8_t  rop;                 /* 0x02 */
  uint8_t  options;             /* 0x03 */
  uint16_t srch;                /* 0x04 */
  uint16_t srcv;                /* 0x06 */
  uint32_t saddr;               /* 0x08 */
  uint32_t daddr;               /* 0x0c */
  uint16_t spitch;              /* 0x10 */
  uint16_t dpitch;              /* 0x12 */

  uint32_t fixedcolor;          /* 0x14 */
  uint32_t pataddr;             /* 0x18 */
  uint16_t patpitch;            /* 0x1c */
  uint8_t  pathoffset;          /* 0x1e */
  uint8_t  patvoffset;          /* 0x1f */

  uint16_t desth;               /* 0x20 */
  uint16_t destv;               /* 0x22 */
  uint16_t ratioh;              /* 0x24 */
  uint16_t ratiov;              /* 0x26 */

  uint8_t  hphaseinit;          /* 0x28 */
  uint8_t  hphaseoffset;        /* 0x29: must be 0 */
  uint8_t  vphaseinit;          /* 0x2a */
  uint8_t  vphaseoffset;        /* 0x2b: must be 0 */

  uint32_t intpmode;            /* 0x2c: interpolation mode */
} __attribute__((aligned(16)));

/* Alpha blending (AB) command (32 bytes) */

struct ge2d_abcmd_s
{
  uint16_t cmd;                 /* 0x00 */
  uint16_t mode;                /* 0x02 */
  uint16_t srch;                /* 0x04 */
  uint16_t srcv;                /* 0x06 */
  uint32_t saddr;               /* 0x08 */
  uint32_t daddr;               /* 0x0c */
  uint16_t spitch;              /* 0x10 */
  uint16_t dpitch;              /* 0x12 */

  uint32_t fixedsrc;            /* 0x14 */
  uint32_t aaddr;               /* 0x18 */
  uint16_t apitch;              /* 0x1c */
  uint16_t reserved;
} __attribute__((aligned(16)));

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* singleton */

ge2d GE;

static char commandbuf[256] __attribute__((aligned(16)));
static uint8_t window_mask[320/8*240] __attribute__((aligned(16)));

void ge2d::manipulate()
{
  uint32_t mask;

  mask = GE2D_INTR_WR_ERR | GE2D_INTR_RD_ERR | GE2D_INTR_NDE | GE2D_INTR_DSD |
    GE2D_INTR_NDF;

  // Disable all interrupts
  hw->INTR_ENABLE = 0;

  // Clear all interrupts
  hw->INTR_STAT = GE2D_INTR_ALL;

  // Set command buffer
  hw->ADDRESS_DESCRIPTOR_START = (uint32_t)(uintptr_t)commandbuf | MSEL;

  // Do raster operation
  hw->CMD_DESCRIPTOR = GE2D_EXEC;

  // Wait command complete or any errors has been indicated.
  while ((hw->INTR_STAT & mask) == 0);
}

void *ge2d::set_copy_cmd(void *cmdbuf, void *srcaddr, void *destaddr,
                         uint16_t srcwidth, uint16_t srcheight,
                         uint16_t srcpitch, uint16_t destpitch)
{
  struct ge2d_copycmd_s *cc = (struct ge2d_copycmd_s *)cmdbuf;

  memset(cc, 0, sizeof(struct ge2d_copycmd_s));

  cc->cmd = COPYCMD | SRC16BPP;
  cc->srcv = srcheight - 1;
  cc->srch = srcwidth - 1;
  cc->dpitch = destpitch - 1;
  cc->spitch = srcpitch -1;
  cc->saddr = (uint32_t)(uintptr_t)srcaddr | MSEL;
  cc->daddr = (uint32_t)(uintptr_t)destaddr | MSEL;

  /* return next command area */

  return (void *)((uintptr_t)cmdbuf + sizeof(struct ge2d_copycmd_s));
}

void *ge2d::set_rop_cmd(void *cmdbuf, void *srcaddr, void *destaddr,
                        uint16_t srcwidth, uint16_t srcheight, uint16_t srcpitch,
                        uint16_t destwidth, uint16_t destheight, uint16_t destpitch,
                        uint8_t rop, uint8_t options, uint16_t patcolor)
{
  struct ge2d_ropcmd_s *rc = (struct ge2d_ropcmd_s *)cmdbuf;
  uint16_t rv;
  uint16_t rh;
  uint16_t cmd = ROPCMD | SRC16BPP;

  memset(rc, 0, sizeof(struct ge2d_ropcmd_s));

  rv = 256 * (uint32_t)srcheight / (uint32_t)destheight;
  rh = 256 * (uint32_t)srcwidth / (uint32_t)destwidth;

  /* If ratio is not 256 (x1), then set scaling bit. */

  if (rv != 256 || rh != 256)
    {
      cmd |= SCALING;
    }

  rc->cmd = cmd;
  rc->rop = rop;
  rc->options = options;
  rc->fixedcolor = patcolor;
  rc->srch = srcwidth - 1;
  rc->srcv = srcheight - 1;
  rc->saddr = (uint32_t)(uintptr_t)srcaddr | MSEL;
  rc->daddr = (uint32_t)(uintptr_t)destaddr | MSEL;
  rc->spitch = srcpitch - 1;
  rc->dpitch = destpitch - 1;
  rc->desth = destwidth - 1;
  rc->destv = destheight - 1;
  rc->ratiov = rv - 1;
  rc->ratioh = rh - 1;
  rc->hphaseinit = 1;
  rc->vphaseinit = 1;
  rc->intpmode = 0x310041; /* XXX: HV Linear interpolation */

  /* return next command area */

  return (void *)((uintptr_t)cmdbuf + sizeof(struct ge2d_ropcmd_s));
}

void *ge2d::set_ab_cmd(void *cmdbuf, void *srcaddr, void *destaddr,
                        uint16_t srcwidth, uint16_t srcheight,
                        uint16_t srcpitch, uint16_t destpitch,
                        void *aaddr, int options, uint8_t fixedalpha)
{
  struct ge2d_abcmd_s *ac = (struct ge2d_abcmd_s *)cmdbuf;

  memset(ac, 0, sizeof(struct ge2d_abcmd_s));

  ac->cmd = ABCMD | options;
  ac->mode = fixedalpha;
  ac->srch = srcwidth - 1;
  ac->srcv = srcheight - 1;
  ac->saddr = (uint32_t)(uintptr_t)srcaddr | MSEL;
  ac->daddr = (uint32_t)(uintptr_t)destaddr | MSEL;
  ac->spitch = srcpitch - 1;
  ac->dpitch = destpitch - 1;
  ac->fixedsrc = 0x0080;
  ac->aaddr = (uint32_t)(uintptr_t)aaddr | MSEL;
  ac->apitch = srcpitch - 1;

  return (void *)((uintptr_t)cmdbuf + sizeof(struct ge2d_abcmd_s));
}

void ge2d::set_halt_cmd(void *cmdbuf)
{
  memset(cmdbuf, 0, 16);
}

/*
 * alpha_init
 *
 * This function create and setup rectangle alpha channel (1bit).
 * The outside of cropping area into dark.
 */

void ge2d::alpha_init()
{
  uint8_t *p;
  int y;
  size_t sz;
  uint8_t fp[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc };
  uint8_t bp[] = { 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff };
  int bpl = 320 / 8; /* bytes per line */

  /* Alpha channel uses (320 / 8) * 240 = 9600 bytes */

  /* Use 1bit alpha channel */

  p = window_mask;

  memset(p, 0xff, bpl * 6);
  p += bpl * 6;
  memset(p, 0, bpl * 112);
  p += bpl * 112;
  memset(p, 0xff, bpl * 6);

  /* Start from 6 line */
  p = window_mask + (bpl * 6);
  for (y = 6; y < 234; y++)
    {
      /* Set front porch and back porch */
      memcpy(p, fp, sizeof(fp));
      memcpy(p + (bpl - sizeof(bp)), bp, sizeof(bp));
      p += bpl;
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void ge2d::shrink(CamImage &frame, void *outimg)
{
  void *cmd = commandbuf;
  uint8_t *start;
  uint16_t width;
  uint16_t height;

  width = frame.getWidth();
  height = frame.getHeight();

  /* Skip cropping start address from (x, y) = (46, 6) */

  start = frame.getImgBuff();
  start += (width * 6 * 2) + (46 * 2);

  /* Output 8bpp shrinked image */

  cmd = set_rop_cmd(cmd, start, outimg,
                    112, 112, width,
                    28, 28, 28,
                    SRCCOPY, FIXEDCOLOR | CONV8BPP, 0x0080);

  /* Finish command sequence */

  set_halt_cmd(cmd);

  manipulate();
}


ge2d::ge2d(void)
{
  this->hw = (volatile struct ge2dhw *)CXD56_GE2D_BASE;
  alpha_init();
}
