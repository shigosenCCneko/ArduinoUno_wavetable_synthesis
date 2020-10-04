
#include "envtone.h"
#include "sysex.h"
#include <avr/io.h>
uint8_t sysex_buf[65];

volatile uint8_t tone_no = 0;

#define NO_SERIALBEGIN      //ArduinoUno のみ　受信ポーリング
char usart_read(void);


int main(void) {
  setup();
  attachInterrupt(0,change_tone,FALLING);
  sei();
  loop();
}


void setup() {
//　　USART設定

#ifdef NO_SERIALBEGIN
  // (115200);
  UBRR0 = 0x10;
  UCSR0C = 0x06;
  UCSR0B = (1 <<RXEN0) | (1 << TXEN0);
  UCSR0A = (1 << U2X0);
  DDRD &= 0xfe;
#else
  Serial.begin(115200);
#endif

  ENVTONE.setup_hardware();

/*
 *  発音モードと音色の設定
 * 
 */

/* ---------------- Use preset tone -------------------------------------------*/

  ENVTONE.set_midistate(MIDI_POLY);    //ポリフォニック
  ENVTONE.defTone(TONE_SAX);   //{TONE_SPACE, TONE_SAX, TONE_FLUTE, TONE_PIANO}
  ENVTONE.setEchoMul(108);    // エコー設定時の減衰値 
/*------------------ use Polyphonic mode user tone -----------------------------------------*/
/* ユーザが定義した音色を使用 */
//  ENVTONE.set_midistate(MIDI_POLY);    //ポリフォニック
//
//  uint8_t sss[] = {
//    15,  9, 19,  3,  7,
//    31, 37, 42, 48, 47, 44, 38, 32, 26, 22, 19, 23, 29, 34, 41, 46,
//    48, 48, 44, 38, 35, 31, 28, 33, 38, 45, 53, 61, 65, 67, 63, 59,
//    -54, -59, -63, -67, -65, -61, -53, -45, -38, -33, -28, -31, -35, -38, -44, -48,
//    -48, -46, -41, -34, -29, -23, -19, -22, -26, -32, -38, -44, -47, -48, -42, -37,
//  };
//
//  ENVTONE.midi_setTone(sss);     //全チャンネルに音色sssをセット

/*--------------- use monophonic mode user tone ------------------------------*/
/* モノフォニックでユーザが定義した音色を使用 */

//  ENVTONE.set_midistate(MIDI_MONO);     //モノフォニック
//  uint8_t new_tone1[] = {
//    15, 11, 19,  2,  6,
//    0,  8, 15, 22, 27, 29, 31, 31, 28, 24, 23, 23, 23, 23, 23, 23,
//    23, 23, 24, 24, 25, 25, 25, 24, 24, 31, 31, 29, 27, 22, 15,  8,
//    0, -8, -15, -22, -27, -29, -31, -31, -28, -25, -21, -16, -11, -7, -3,  0,
//    -1,  0, -3, -7, -11, -16, -21, -25, -28, -31, -31, -29, -27, -22, -15, -8,
//  };
//  ENVTONE.setTone(0, new_tone1);
//
//  uint8_t new_tone2[] = {
//    15,  2, 25,  4,  5,
//    24, 41, 52, 46, 54, 58, 65, 50, 57, 47, 32, 33, 31, 24, 17, 18,
//    14, 16, 19, 24, 27, 34, 35, 41, 47, 53, 63, 61, 62, 54, 54, 42,
//    -33, -36, -55, -63, -61, -70, -58, -67, -47, -44, -39, -42, -26, -25, -14, -26,
//    -25, -21, -18, -20, -19, -36, -31, -52, -56, -55, -60, -65, -50, -55, -48, -30
//  };
//  ENVTONE.setTone(1, new_tone2);
//
//  uint8_t new_tone3[] = {
//    14,  1, 13,  1, 13,
//    56, 53, 56, 62, 63, 61, 59, 70, 52, 54, 61, 53, 50, 33, 41, 49,
//    65, 63, 59, 58, 58, 57, 63, 64, 66, 69, 64, 69, 67, 70, 59, 59,
//    56, 44, 49, 46, 33, 40, 31, 47, 38, 47, 51, 63, -16, -15, -65, -53,
//    -52, -58, -59, -61, -64, -52, -64, -49, -51, -62, -62, -58, -73, -61, -59, -64,
//
//  };
//  ENVTONE.setTone(2, new_tone3);
//
//  uint8_t new_tone4[] = {
//    14,  0, 18,  1, 12,
//    0, -6, -14, -21, -28, -36, -42, -47, -49, -51, -52, -50, -47, -45, -41, -40,
//    -39, -41, -39, -42, -44, -46, -49, -54, -53, -51, -47, -41, -34, -27, -19, -6,
//    3, 13, 29, 41, 46, 49, 51, 51, 53, 54, 55, 53, 49, 46, 42, 45,
//    47, 47, 41, 45, 47, 50, 52, 51, 49, 47, 42, 36, 28, 21, 14,  6,
//  };
//  ENVTONE.setTone(3, new_tone3);
//  ENVTONE.setTone(4, new_tone3);
//  ENVTONE.setTone(5, new_tone3);
//  ENVTONE.setTone(6, new_tone3);
//  ENVTONE.setTone(7, new_tone3);
/*------------------------------------------------------------------------------*/
 }

void loop() {
  uint8_t data;
  uint8_t sysExcnt = 0;
  uint8_t dat1, dat2, dat3, com;
  TIMSK0 = 0;
  
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
      midi_sysEx(sysex_buf, sysExcnt);
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

#ifdef NO_SERIALBEGIN
  while (  ! (UCSR0A & 0x80))
    ;
  return UDR0;
  
#else
    char c ;
    while (Serial.available() == 0)
      ;
    c = Serial.read();
    return (char)(0x00ff & c);

#endif

}
void change_tone(){
  cli();
  tone_no++;
  tone_no &= 0x03;
  ENVTONE.defTone(tone_no);
  sei();
  for(volatile long i = 0; i < 10000;i++){
    ;
  }
}









