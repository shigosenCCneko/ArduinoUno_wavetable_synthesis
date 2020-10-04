#include <avr/io.h>
#include <avr/pgmspace.h>
#include "MemTone.h"
#include "envtone_def.h"
#include "tone_freq.h"
const uint8_t envelope_cnt[16] PROGMEM = {0, 1, 2, 3, 4, 5, 7, 10, 13, 20, 29, 43, 64, 86, 128, 255}; //endless,255,127,85,63,51,36,25,19,12,8,5,3,2,1,0

/* tone_freq[80] MIDI note No 36 to 127  2C to 9G */
//#ifdef USE_32KHZ
//const  uint16_t tone_freq[80] PROGMEM = {
//   33 ,   35 ,   38 ,   40 ,   42 ,   45 ,   47 ,   50 ,   53 ,   56 ,   60 ,   63,
//   67 ,   71 ,   75 ,   80 ,   84 ,   89 ,   95 ,  100 ,  106 ,  113 ,  119 ,  126,
//  134 ,  142 ,  150 ,  159 ,  169 ,  179 ,  189 ,  201 ,  213 ,  225 ,  239 ,  253,
//  268 ,  284 ,  301 ,  319 ,  338 ,  358 ,  379 ,  401 ,  425 ,  451 ,  477 ,  505,
//  536 ,  568 ,  601 ,  637 ,  675 ,  715 ,  758 ,  803 ,  851 ,  901 ,  955 , 1011,
// 2143 , 2271 , 2406 , 2549 , 2700 , 2861 , 3031 , 3211 , 3402 , 3604 , 3819 , 4046,
// 4286 , 4541 , 4811 , 5098 , 5401 , 5722 , 6062 , 6422
//};
//#else
//const  uint16_t tone_freq[80] PROGMEM = {
//   33*2 ,   35*2 ,   38*2 ,   40*2 ,   42*2 ,   45*2 ,   47*2 ,   50*2 ,   53*2 ,   56*2 ,   60*2 ,   63*2,
//   67*2 ,   71*2 ,   75*2 ,   80*2 ,   84*2 ,   89*2 ,   95*2 ,  100*2 ,  106*2 ,  113*2 ,  119*2 ,  126*2,
//  134*2 ,  142*2 ,  150*2 ,  159*2 ,  169*2 ,  179*2 ,  189*2 ,  201*2 ,  213*2 ,  225*2 ,  239*2 ,  253*2,
//  268*2 ,  284*2 ,  301*2 ,  319*2 ,  338*2 ,  358*2 ,  379*2 ,  401*2 ,  425*2 ,  451*2 ,  477*2 ,  505*2,
//  536*2 ,  568*2 ,  601*2 ,  637*2 ,  675*2 ,  715*2 ,  758*2 ,  803*2 ,  851*2 ,  901*2 ,  955*2 , 1011*2,
// 2143*2 , 2271*2 , 2406*2 , 2549*2 , 2700*2 , 2861*2 , 3031*2 , 3211*2 , 3402*2 , 3604*2 , 3819*2 , 4046*2,
// 4286*2 , 4541*2 , 4811*2 , 5098*2 , 5401*2 , 5722*2 , 6062*2 , 6422*2
//};
//#endif

extern uint8_t echo_mul;

MemTone::MemTone() {

}


static MemTone::tone_on( int tone, int vol) {
  int f;
  if (tone >= 24)
    tone -= 24;
  f = pgm_read_word(&(tone_freq[tone]));

  this->sin_step = f;
  this->sin_pos = 0;
  this->env_state = 1;
  this->vol = (vol & 0x1f) * 8;
  this->env_cnt = 249;
  this->level = 0;
  this->env_step = this->atk;
}

MemTone::tone_off() {
  this->env_state = 4;
  this->env_step = this->rel;

}

MemTone::set_wave(char *wave) {
  this->wave_tbl = wave;
}

MemTone:: set_envelope( uint8_t atk, uint8_t decy, uint8_t sul, uint8_t sus, uint8_t rel) {
  this->atk = conv_count(atk);
  this->decy = conv_count(decy);
  this->sul = sul * 4;
  this->sus = conv_count(sus);
  this->rel = conv_count(rel);
}

MemTone::set_atk(uint8_t v) {
  this->atk = conv_count(v);
}

MemTone::set_decy(uint8_t v) {
  this->decy = conv_count(v);
}

MemTone::set_sul(uint8_t v) {
  this->sul = v * 4;
}

MemTone::set_sus(uint8_t v) {
  this->sus = conv_count(v);
}

MemTone::set_rel(uint8_t v) {
  this->rel = conv_count(v);
}

uint8_t MemTone::conv_count(uint8_t d) {
  return pgm_read_byte(&(envelope_cnt[d & 0x0f]));
}

MemTone::set_value(uint8_t p, uint8_t val) {
  char *po;
  po = this->wave_tbl;
  po[p] = val;
}

MemTone::set_echo(uint8_t d) {
#ifdef USE_ECHO
  echo_mul = d;
#endif
}


