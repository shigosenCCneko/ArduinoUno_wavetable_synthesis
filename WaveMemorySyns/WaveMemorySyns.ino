
#include "envtone.h"

uint8_t sysex_buf[65];

char usart_read(void);
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
// Serial.begin(2000000);
  ENVTONE.setup_hardware();
  ENVTONE.set_midistate(MIDI_POLY);    //声ポリフォニック
  ENVTONE.midi_setwave(FM2OP2);        //
  ENVTONE.midi_set_envelope(12,9,25,2,8);  
  
//  ENVTONE.set_midistate(MIDI_MONO);     //モノフォニック
//  ENVTONE.set_wave(1,TRI);              //ch1を三角波へ
//  ENVTONE.set_wave(2,TRI);
//  ENVTONE.set_wave(3,SIN);
//  ENVTONE.set_envelope(3,8,9,25,2,5);

}

void loop() {

  uint8_t data;
  uint8_t sysExcnt = 0;
  uint8_t dat1, dat2, dat3, com;

  while (1) {
    dat1 = usart_read();
    if (dat1 == 0xf0) {
      sysExcnt = 1;
      do {
        data = usart_read();
        sysex_buf[sysExcnt++] = data;
        if (sysExcnt > 64) {
          break;
        }
      } while (data != 0xf7);  //システムエクルシーブメッセージ終了
      //midi_sysEx(sysex_buf, sysExcnt);
      sysExcnt = 0;

    } else {
      if (dat1 & 0x80) {
        com = dat1 & 0xf0;
        dat2 = usart_read();

        if ( (com == 0xc0) || (com == 0xd0)) {
          ENVTONE.midi_command(com, dat1, dat2, dat3);
        } else {
          dat3 = usart_read();
          ENVTONE.midi_command(com, dat1, dat2, dat3);
        }
      }
    }
  }
}


char usart_read() {
  char c ;
  while (Serial.available() == 0)
    ;
  c = Serial.read();
  return (char)(0x00ff & c);
}

