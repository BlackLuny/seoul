/*
 * User Thread Control Block (UTCB)
 *
 * Copyright (C) 2008, Udo Steinberg <udo@hypervisor.org>
 * Copyright (C) 2008-2010, Bernhard Kauer <bk@vmmon.org>
 * Copyright (C) 2011, Alexander Boettcher <ab764283@os.inf.tu-dresden.de>
 * Copyright (C) 2012, Julian Stecklina <jsteckli@os.inf.tu-dresden.de>
 * Economic rights: Technische Universitaet Dresden (Germany)
 *
 * This file is part of Seoul.
 *
 * Soul is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Seoul is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details.
 */

#pragma once

#include <service/string.h>
#include <nul/types.h>

enum {
  MTD_GPR_ACDB        = 1ul << 0,
  MTD_GPR_BSD         = 1ul << 1,
  MTD_RSP             = 1ul << 2,
  MTD_RIP_LEN         = 1ul << 3,
  MTD_RFLAGS          = 1ul << 4,
  MTD_DS_ES           = 1ul << 5,
  MTD_FS_GS           = 1ul << 6,
  MTD_CS_SS           = 1ul << 7,
  MTD_TR              = 1ul << 8,
  MTD_LDTR            = 1ul << 9,
  MTD_GDTR            = 1ul << 10,
  MTD_IDTR            = 1ul << 11,
  MTD_CR              = 1ul << 12,
  MTD_DR              = 1ul << 13,
  MTD_SYSENTER        = 1ul << 14,
  MTD_QUAL            = 1ul << 15,
  MTD_CTRL            = 1ul << 16,
  MTD_INJ             = 1ul << 17,
  MTD_STATE           = 1ul << 18,
  MTD_TSC             = 1ul << 19,
  MTD_IRQ             = MTD_RFLAGS | MTD_STATE | MTD_INJ | MTD_TSC,
  MTD_ALL             = (~0U >> 12) & ~MTD_CTRL
};

enum {
  INJ_IRQWIN = 0x1000,
  INJ_NMIWIN = 0x0000, // XXX missing
  INJ_WIN    = INJ_IRQWIN | INJ_NMIWIN
};

#define GREG(NAME)                                  \
  union {                                           \
    struct {                                        \
      unsigned char           NAME##l, NAME##h;     \
    };                                              \
    unsigned short          NAME##x;                \
    unsigned           e##NAME##x;                  \
    mword              r##NAME##x;                  \
  }
#define GREG16(NAME)                        \
  union {                                   \
    unsigned short          NAME;           \
    unsigned           e##NAME;             \
    mword              r##NAME##x;          \
  }

struct ArchCpuState
{
  typedef struct Descriptor
  {
    uint16 sel, ar;
    uint32 limit;
    union {
        uint64 : 64;
        mword base;
    };
    void set(uint16 _sel, uint32 _base, uint32 _limit, uint16 _ar) { sel = _sel; base = _base; limit = _limit; ar = _ar; };
  } Descriptor;

  mword     mtd;
  mword     inst_len;
  GREG16(ip); GREG16(fl);
  uint32     intr_state, actv_state, inj_info, inj_error;
  union {
    struct {
      GREG(a);    GREG(c);    GREG(d);    GREG(b);
      GREG16(sp); GREG16(bp); GREG16(si); GREG16(di);
#ifdef __x86_64__
      mword r8, r9, r10, r11, r12, r13, r14, r15;
#endif
    };
#ifdef __x86_64__
    mword gpr[16];
#else
    mword gpr[8];
#endif
  };
  uint64       qual[2];
  uint32       ctrl[2];
  uint64       : 64;          // reserved
  mword        cr0, cr2, cr3, cr4;
#ifdef __x86_64__
  mword        cr8;
  mword        : 64;          // reserved
#endif
  mword        dr7, sysenter_cs, sysenter_esp, sysenter_eip;
  Descriptor   es, cs, ss, ds, fs, gs;
  Descriptor   ld, tr, gd, id;
  uint64       tsc_value, tsc_off;

  /* Set all values to zero. */
  void clear() { memset(this, 0, sizeof(*this)); }
};

/* EOF */
