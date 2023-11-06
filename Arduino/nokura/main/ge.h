
#ifndef __GE_H
#define __GE_H

#include <Camera.h>

// forward reference
struct ge2dhw;

class ge2d {
private:
  volatile struct ge2dhw *hw;

  void *set_copy_cmd(void *cmdbuf, void *srcaddr, void *destaddr,
                     uint16_t srcwidth, uint16_t srcheight,
                     uint16_t srcpitch, uint16_t destpitch);
  void *set_rop_cmd(void *cmdbuf, void *srcaddr, void *destaddr,
                    uint16_t srcwidth, uint16_t srcheight, uint16_t srcpitch,
                    uint16_t destwidth, uint16_t destheight, uint16_t destpitch,
                    uint8_t rop, uint8_t options, uint16_t patcolor);
  void *set_ab_cmd(void *cmdbuf, void *srcaddr, void *destaddr,
                   uint16_t srcwidth, uint16_t srcheight,
                   uint16_t srcpitch, uint16_t destpitch,
                   void *aaddr, int options, uint8_t fixedalpha);
  void set_halt_cmd(void *cmdbuf);
  void manipulate();
  void alpha_init();

public:
  ge2d();
  
  void begin();
  void shrink(CamImage &frame, void *outimg);
  void decolate(CamImage &frame, int icon);
};

extern ge2d GE;

#endif
