#include <avr/io.h>
#include <avr/pgmspace.h>
#include "envtone.h"


#define MIDI_NOTE_OFF        0x80
#define MIDI_NOTE_ON         0x90

EnvTone ENVTONE;            //
MemTone memtone[MAX_TONE];  //アセンブラからアクセスする



const  char EnvTone::wave_sin[WAVE_TBL_SIZE] PROGMEM = {
  0, 3, 6, 9, 12, 15, 17, 20, 22, 24, 26, 28, 29, 30, 31, 31,
  31, 31, 31, 30, 29, 28, 26, 24, 22, 20, 17, 15, 12, 9, 6, 3,
  0, -3, -6, -9, -12, -15, -17, -20, -22, -24, -26, -28, -29, -30, -31, -31,
  -31, -31, -31, -30, -29, -28, -26, -24, -22, -20, -17, -15, -12, -9, -6, -3,
};
const  char EnvTone::wave_2OP[WAVE_TBL_SIZE]  PROGMEM = {
  0, 8, 17, 23, 27, 28, 29, 30, 29, 28, 27, 27, 23, 21, 21, 21,
  21, 25, 28, 30, 30, 27, 23, 17, 8, 0, -3, -6, -8, -8, -6, -3,
  0, 3, 6, 8, 8, 6, 3, 0, -8, -17, -23, -27, -30, -30, -28, -25,
  -21, -21, -21, -21, -23, -27, -27, -28, -29, -30, -29, -28, -27, -23, -17, -8
};
const  char EnvTone::wave_rect[WAVE_TBL_SIZE]  PROGMEM = {
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, -21, -21, -21, -21, -21, -21,
  -21, -21, -21, -21, -21, -21, -21, -21, -21, -21, -21, -21, -21, -21, -21, 21,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21
};
const  char EnvTone::wave_tri[WAVE_TBL_SIZE]  PROGMEM = {
  0, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29,
  31, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2,
  0, -1, -3, -5, -7, -9, -11, -13, -15, -17, -19, -21, -23, -25, -27, -29,
  -31, -29, -27, -25, -23, -21, -19, -17, -15, -13, -11, -9, -7, -5, -3, -1,
};





EnvTone::EnvTone() {
  for (int i = 0; i < MAX_TONE ; i ++) {
    memtone[i].set_wave(wave_tri);
    memtone[i].set_envelope(15, 9, 9, 2, 5);
    voice_queue[i] = i;
    ch_midino[i] = 0;
  }
}


EnvTone::setup_hardware() {
  /* Use PWM OUTPUT */
  //pinMode(3, OUTPUT);   //PWM　OCR2B出力
  DDRD |= 0x08;
  TCCR2A = 0x0;
  TCCR2B = 0x0;
  TCCR2A = (1 << COM2B1) | (1 << COM2B0) | (0 << WGM21) | (1 << WGM20);
  TCCR2B = (0 << CS21) | (1 << CS20) | (0 << WGM22);

  /* Timer 1 CTC mode clkI/O */
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;


//  OCR1A = 499; //32KHz
  OCR1A = 999; //16KHz

  TCCR1A = (1 << COM1A0);
  TCCR1B = (1 << WGM12) | (1 << CS10);
  TIMSK1 = 0;
  TIMSK1 |= (1 << OCIE1A);    //Timer2 enable

}


EnvTone::note_on(uint8_t ch, uint8_t no, uint8_t vel) {
  ch &= 0x03;
  memtone[ch].tone_on(no, vel >> 2);

}

EnvTone::note_off(uint8_t ch) {
  memtone[ch].tone_off();
}

EnvTone::set_wave(uint8_t ch , wavetype i) {
  uint16_t wave;
  switch (i) {
    case SIN:
      wave = wave_sin;
      break;
    case FM2OP:
      wave = wave_2OP;
      break;
    case RECT:
      wave = wave_rect;
      break;
    case TRI:
      wave = wave_tri;
      break;
    default:
      break;
  }
  memtone[ch].set_wave(wave);
}

EnvTone::midi_setwave(wavetype wave) {
  for (uint8_t i = 0; i < MAX_TONE; i++) {
    set_wave(i, wave);
  }
}

EnvTone::set_envelope(uint8_t ch, uint8_t atk, uint8_t decy, uint8_t sul, uint8_t sus, uint8_t rel){
  ch &= 0x03;
  atk &= 0x0f;
  decy &= 0x0f;
  sul &= 0x1f;
  sus &= 0x0f;
  rel &= 0x0f;
  memtone[ch].set_envelope(atk,decy,sul,sus,rel);
}
EnvTone::midi_set_envelope(uint8_t atk, uint8_t decy, uint8_t sul, uint8_t sus, uint8_t rel){
  atk &= 0x0f;
  decy &= 0x0f;
  sul &= 0x1f;
  sus &= 0x0f;
  rel &= 0x0f;
  for(uint8_t i = 0;i < MAX_TONE;i++){
    this->set_envelope( i,  atk,  decy,  sul, sus,  rel);
  }
  
}

EnvTone::midi_command(uint8_t com, char dat1, char dat2, char dat3) {

  uint8_t voice_ch;
  uint8_t ch = dat1 & 0x03;

  if ( this->midi_state == MIDI_POLY) {  // PolyPhonic Mode
    if (ch == 0) {

      switch (com) {

        case MIDI_NOTE_ON:
          if (dat3 == 0) {  //ベロシティ0はノートオフ
            midi_note_off(dat2);
          } else {
            // キューから鳴らすチャンネル番号を得る
            if (active_voice_num == MAX_TONE) {
              voice_ch = voice_queue[voice_queue_top];
              memtone[voice_ch].tone_off();
            } else {
              voice_ch = voice_queue[voice_queue_top++];
              if (voice_queue_top == MAX_TONE) {
                voice_queue_top = 0;
              }
              active_voice_num++;
            }
            ch_midino[voice_ch] = dat2;
            memtone[voice_ch].tone_on(dat2, dat3 >> 2);

          }
          break;

        case MIDI_NOTE_OFF:
          midi_note_off(dat2);

          break;


        default:
          break;
      }
    }
  } else {
    switch (com) {

      case MIDI_NOTE_ON:

        if (dat3 == 0) {
          memtone[ch].tone_off();

        } else {
          memtone[ch].tone_on( dat2, dat3 >> 2 );

        }
        break;

      case MIDI_NOTE_OFF:
        memtone[ch].tone_off();

        break;
      default:

        break;
    }
  }
}

EnvTone::midi_note_off(uint8_t midi_no) {
  for (uint8_t i = 0; i < MAX_TONE; i++) {
    if (ch_midino[i] == midi_no) {
      memtone[i].tone_off();
      ch_midino[i] = 0;
      // キューへオフしたチャンネル番号を入れる
      voice_queue[voice_queue_tail++] = i;
      if (voice_queue_tail == MAX_TONE) {
        voice_queue_tail = 0;
      }
      active_voice_num--;

    }
  }
}

EnvTone::set_midistate(midistat s){
  this->midi_state = s;
}




