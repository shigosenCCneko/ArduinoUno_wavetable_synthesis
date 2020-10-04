


struct MemTone {
private:
    uint16_t sin_pos;
    uint16_t sin_step;
    uint8_t  vol;

    uint8_t env_state;
    uint8_t env_cnt;

    uint8_t atk;
    uint8_t decy;
    uint8_t sul;
    uint8_t sus;
    uint8_t rel;
    uint8_t level;
    char * wave_tbl;
    uint8_t env_step;

public:
    MemTone();
    tone_on( int tone, int vol);
    tone_off();
    set_wave(char *wave);
    set_envelope( uint8_t atk, uint8_t decy, uint8_t sul, uint8_t sus, uint8_t rel);
    set_value(uint8_t p, uint8_t val);
    
  set_atk(uint8_t v);
  set_decy(uint8_t v);
  set_sul(uint8_t v);
  set_sus(uint8_t v);
  set_rel(uint8_t v);

  set_echo(uint8_t v);


private:
 
    uint8_t conv_count(uint8_t d);

};
