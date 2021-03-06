
#include "neslib.h"
#include "vrambuf.h"

// index to end of buffer
byte updptr = 0;

// add EOF marker to buffer (but don't increment pointer)
void cendbuf(void) {
  VRAMBUF_SET(NT_UPD_EOF);
}

// clear vram buffer and place EOF marker
void cclearbuf(void) {
  updptr = 0;
  cendbuf();
}

// wait for next frame, then clear buffer
// this assumes the NMI will call flush_vram_update()
void cflushnow(void) {
  // make sure buffer has EOF marker
  cendbuf();
  // wait for next frame to flush update buffer
  // this will also set the scroll registers properly
  ppu_wait_frame();
  // clear the buffer
  cclearbuf();
}

// add multiple characters to update buffer
// using horizontal increment
void putbytes(word addr, const char* str, byte len) {
  byte i;
  // if bytes won't fit, wait for vsync and flush buffer
  if (updptr >= VBUFSIZE-4-len) cflushnow();
  // add vram address
  VRAMBUF_ADD((addr >> 8) ^ NT_UPD_HORZ);
  VRAMBUF_ADD(addr); // only lower 8 bits
  // add length
  VRAMBUF_ADD(len);
  // add data
  for (i=0; i<len; ++i) {
    	VRAMBUF_ADD(str[i]);
  }
  // place EOF mark
  cendbuf();
}
