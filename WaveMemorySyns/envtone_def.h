/* 
*  MAX_TONE 発音数 
* 
* 34KHzなら8
* 4以上の発音数を指定するとoverflowした時に雑音が出ます
* CLIPを指定時発音数が1～2減少します
*/
#define MAX_TONE 8

/*
 * PWM_KHZ  サンプリング周波数 24ぐらいで十分な音質になります
 */

#define PWM_KHZ  24
/*
 *   CLIP 波形加算時overflowした時時最大値にCLIPします
 */
#define CLIP

/*
 * PWM_PIN 出力PINの選択  3,5,6,11
 *      3  Timer2 OCR2B
 *      5  Timer0 OCR0B
 *      6  Timer0 OCR0A 
 *      11 Timer2 OCR2A
 *      
 *      Timer0(Pin5 or Pin6)を指定すると
 *      delay()等のTimer0を使った時間管理が出来なくなります
 *       
 */
 #define PWM_PIN    3
 
  /*
  *  USE_ECHO   エコー使用
  * 
  *  BUF_SIZE   エコーバッファのサイズ
  *  ECHO_MUL   エコーの減衰  ECHO_MUL/256
  */
//#define USE_ECHO

#define BUF_SIZE 500    //500to 1500
#define ECHO_MUL 108
