#include <avr/io.h>
#include <avr/pgmspace.h>
#include "envtone.h"


#define MIDI_NOTE_OFF        0x80
#define MIDI_NOTE_ON         0x90

EnvTone ENVTONE;
MemTone memtone[MAX_TONE];  //アセンブラからアクセスする


  uint8_t defaultTone[] = {
    15,  9, 19,  3,  7,
    31, 37, 42, 48, 47, 44, 38, 32, 26, 22, 19, 23, 29, 34, 41, 46,
    48, 48, 44, 38, 35, 31, 28, 33, 38, 45, 53, 61, 65, 67, 63, 59,
    -54, -59, -63, -67, -65, -61, -53, -45, -38, -33, -28, -31, -35, -38, -44, -48,
    -48, -46, -41, -34, -29, -23, -19, -22, -26, -32, -38, -44, -47, -48, -42, -37,
  };

EnvTone::EnvTone() {
  midi_setTone(defaultTone);
  for (int i = 0; i < MAX_TONE ; i ++) {
    voice_queue[i] = i;
    ch_midino[i] = 0;
  }
  memtone[0].set_echo(ECHO_MUL);
}


EnvTone::setup_hardware() {
  /* Use PWM OUTPUT */
#if (PWM_PIN == 3)
  //pinMode(3, OUTPUT);   //PWM　OCR2B出力  PD3
  DDRD |= 0x08;
  TCCR2A = 0x0;
  TCCR2B = 0x0;
  TCCR2A = (1 << COM2B1) | (1 << COM2B0) | (1 << WGM21) | (1 << WGM20);
  TCCR2B = (0 << CS21) | (1 << CS20) | (0 << WGM22);

#elif (PWM_PIN == 11)
  //pinMode(11, OUTPUT);   //PWM　OCR2A出力 PB3
  DDRB |= 0x8;
  TCCR2A = 0x0;
  TCCR2B = 0x0;
  TCCR2A = (1 << COM2A1) | (1 << COM2A0) | (1 << WGM21) | (1 << WGM20);
  TCCR2B = (0 << CS21) | (1 << CS20) | (0 << WGM22);

#elif (PWM_PIN == 5) 
  //pinMode(5, OUTPUT);   //PWM　OCR0B出力 PD5
  TIMSK0 = 0;
  DDRD |= 0x20;
  TCCR0A = 0x0;
  TCCR0B = 0x0;
  TCCR0A = (1 << COM0B1) | (1 << COM0B0) | (1 << WGM01) | (1 << WGM00);
  TCCR0B = (0 << CS01) | (1 << CS00) | (0 << WGM02);

#elif (PWM_PIN == 6)
    //pinMode(6, OUTPUT);   //PWM　OCR0A出力 PD6
  TIMSK0 = 0;
  DDRD |= 0x40;
  TCCR0A = 0x0;
  TCCR0B = 0x0;
  TCCR0A = (1 << COM0A1) | (1 << COM0A0) | (1 << WGM01) | (1 << WGM00);
  TCCR0B = (0 << CS01) | (1 << CS00) | (0 << WGM02);

#elif (PWM_PIN == 13) // Leonardo
  TIMSK0 = 0;
  DDRC |= 0x80;  // PC7  OC4A
  TCCR4A = 0x0;
  TCCR4B = 0x0;
  
  TCCR4A = (0 << COM4A1) | (1 << COM4A0) |(1 << PWM4A);
  TCCR4B = (0 << CS43) | ( 0 << CS42) | (0 << CS41) | (1 << CS40)             ;  
  TCCR4D = (0 << WGM41) | ( 0 << WGM40);
  PLLFRQ |= (1 << PLLTM1 )|(0 << PLLTM0);

#endif

  /* Timer 1 CTC mode clkI/O */
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  uint16_t freq;
  freq = (160000 / PWM_KHZ + 5 ) / 10;  //浮動小数点ライブラリを使いたくない
  OCR1A = freq;

  TCCR1A = (1 << COM1A0);
  TCCR1B = (1 << WGM12) | (1 << CS10);
  TIMSK1 = 0;
  TIMSK1 |= (1 << OCIE1A);    //Timer2 enable
}


EnvTone::note_on(uint8_t ch, uint8_t no, uint8_t vel) {
  ch &= 0x0f;
  memtone[ch].tone_on(no, vel >> 2);

}

EnvTone::note_off(uint8_t ch) {
  memtone[ch].tone_off();
}

EnvTone::set_envelope(uint8_t ch, uint8_t atk, uint8_t decy, uint8_t sul, uint8_t sus, uint8_t rel){
  ch &= 0x0F;
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
  uint8_t ch = dat1 & 0x07;

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

EnvTone::change_envelope(uint8_t ch,uint8_t type,uint8_t val){
  for(uint8_t i = 0; i < MAX_TONE;i++){
    if(this->midi_state ==  MIDI_POLY){
      ch = i;
    }
    switch(type){
      case ATK:
        memtone[ch].set_atk(val);
      break;
  
      case DECY:
        memtone[ch].set_decy( val);
      break;
  
      case SL:
        memtone[ch].set_sul(val);
      break;
      
      case SR:
        memtone[ch].set_sus(val);
      break;
      
      case RR:
        memtone[ch].set_rel(val);
      break;

      case ECHO:
        memtone[ch].set_echo(val);
        break;
      
      default:
      break;
    }
  }
}

EnvTone::change_wavevalue(uint8_t p,uint8_t val){
  memtone[0].set_value(p,val);

#ifdef USE_ECHO
  //midi_state = MIDI_POLY;
#endif  
}

EnvTone::setTone(uint8_t ch,uint8_t source[]){
  midi_set_envelope(*(source),*(source+1),*(source+2),*(source+3),*(source+4));
  source+=5;
  memtone[ch].set_wave(source);
}

EnvTone::midi_setTone(uint8_t source[]){
  midi_set_envelope(*(source),*(source+1),*(source+2),*(source+3),*(source+4));
  source+=5;

   for (uint8_t i = 0; i < MAX_TONE; i++) {
      memtone[i].set_wave(source);
  } 
}

EnvTone::setEchoMul(uint8_t mul){
    memtone[0].set_echo(mul);
}
/* --------------------プリセット音------------------*/



const uint8_t def_tone[][69] PROGMEM = {
{ //space
 15,  2, 25,  4,  5,
 24, 41, 52, 46, 54, 58, 65, 50, 57, 47, 32, 33, 31, 24, 17, 18,
 14, 16, 19, 24, 27, 34, 35, 41, 47, 53, 63, 61, 62, 54, 54, 42,
-33,-36,-55,-63,-61,-70,-58,-67,-47,-44,-39,-42,-26,-25,-14,-26,
-25,-21,-18,-20,-19,-36,-31,-52,-56,-55,-60,-65,-50,-55,-48,-30
},


{ //sax
 14,  1, 13,  1, 13,
 56, 53, 56, 62, 63, 61, 59, 70, 52, 54, 61, 53, 50, 33, 41, 49,
 65, 63, 59, 58, 58, 57, 63, 64, 66, 69, 64, 69, 67, 70, 59, 59,
 56, 44, 49, 46, 33, 40, 31, 47, 38, 47, 51, 63,-16,-15,-65,-53,
-52,-58,-59,-61,-64,-52,-64,-49,-51,-62,-62,-58,-73,-61,-59,-64,
},

{ //flute
 14,  0, 18,  1, 12,
  0, -6,-14,-21,-28,-36,-42,-47,-49,-51,-52,-50,-47,-45,-41,-40,
-39,-41,-39,-42,-44,-46,-49,-54,-53,-51,-47,-41,-34,-27,-19, -6,
  3, 13, 29, 41, 46, 49, 51, 51, 53, 54, 55, 53, 49, 46, 42, 45,
 47, 47, 41, 45, 47, 50, 52, 51, 49, 47, 42, 36, 28, 21, 14,  6,
},

{//piano
 15, 11, 18,  3,  7,
  0,  8, 21, 45, 53, 54, 45, 55, 37, 35, 38, 27, 21, 21, 24, 22,
 27, 26, 25, 25, 30, 34, 45, 51, 58, 66, 62, 67, 63, 58, 39, 29,
-14,-35,-38,-48,-63,-58,-67,-52,-58,-48,-40,-26,-32,-25,-30,-16,
-16,-19,-23,-26,-33,-25,-40,-30,-37,-50,-50,-47,-57,-38,-25,-19,
}


};
EnvTone::defTone(uint8_t no){
    uint8_t *p;
    p = (def_tone[no]);
    midi_set_envelope(
      pgm_read_byte(p),
      pgm_read_byte(p+1),
      pgm_read_byte(p+2),
      pgm_read_byte(p+3),
      pgm_read_byte(p+4)
      );
      p += 5;
  for(uint8_t i = 0;i < 64;i++){
    memtone[0].set_value(i,pgm_read_byte(p++));
  }
}
