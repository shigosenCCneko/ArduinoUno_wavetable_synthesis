#include <avr/io.h>
#include <avr/pgmspace.h>
#include "MemTone.h"

const uint8_t envelope_cnt[16] PROGMEM = {0, 1, 2, 3, 4, 5, 7, 10, 13, 20, 29, 43, 64, 86, 128, 255}; //endless,255,127,85,63,51,36,25,19,12,8,5,3,2,1,0

/* tone_freq[80] MIDI note No 36 to 127  2C to 9G */
const  uint16_t tone_freq[80] PROGMEM = {
   33 ,   35 ,   38 ,   40 ,   42 ,   45 ,   47 ,   50 ,   53 ,   56 ,   60 ,   63,
   67 ,   71 ,   75 ,   80 ,   84 ,   89 ,   95 ,  100 ,  106 ,  113 ,  119 ,  126,
  134 ,  142 ,  150 ,  159 ,  169 ,  179 ,  189 ,  201 ,  213 ,  225 ,  239 ,  253,
  268 ,  284 ,  301 ,  319 ,  338 ,  358 ,  379 ,  401 ,  425 ,  451 ,  477 ,  505,
  536 ,  568 ,  601 ,  637 ,  675 ,  715 ,  758 ,  803 ,  851 ,  901 ,  955 , 1011,
 2143 , 2271 , 2406 , 2549 , 2700 , 2861 , 3031 , 3211 , 3402 , 3604 , 3819 , 4046,
 4286 , 4541 , 4811 , 5098 , 5401 , 5722 , 6062 , 6422
};

MemTone::MemTone() {

}


static MemTone::tone_on( int tone, int vol) {
  int f;
  if (tone >= 36)
    tone -= 36;
  f = pgm_read_word(&(tone_freq[tone]));

  this->sin_step = f;
  this->sin_pos = 0;
  this->env_state = 1;
  this->vol = vol & 0x1f;
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
  this->sul = sul;
  this->sus = conv_count(sus);
  this->rel = conv_count(rel);

}


uint8_t MemTone::conv_count(uint8_t d) {
  return pgm_read_byte(&(envelope_cnt[d & 0x0f]));
}




