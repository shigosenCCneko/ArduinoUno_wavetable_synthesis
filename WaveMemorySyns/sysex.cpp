#include <avr/io.h>
#include "envtone.h"

void midi_sysEx(uint8_t * sysex_mes, uint8_t dat_len) {
  uint8_t Data[4];
  uint8_t l;

  if (dat_len < 8) {

    Data[0] = sysex_mes[1];
    Data[1] = sysex_mes[2];
    Data[2] = sysex_mes[3];
    Data[3] = sysex_mes[4];
    l = sysex_mes[5];

    if (l & 0x04) {
      Data[1] += 0x80;
    }
    if (l & 0x02) {
      Data[2] += 0x80;
    }
    if (l & 0x01) {
      Data[3] += 0x80;
    }
    
    switch (Data[0]) {
      case 1:     //change envelope val

         ENVTONE.change_envelope(0,Data[2],Data[3]);
          break;
     case 101:    //rewrite wave tbl
        ENVTONE.change_wavevalue(Data[2],Data[3]);
          break;

    default:
      break;
    }
  }
}
