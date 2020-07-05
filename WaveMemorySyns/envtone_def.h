/* 
*  MAX_TONE 発音数 
* 16KHZで12
* 32KHzなら5～6
* 4以上の発音数を指定するとoverflowした時に雑音が出ます
* CLIPを指定時発音数が1～2減少します
*/
#define MAX_TONE  6
/*
 * USE_32KZ  32KHzで使用するときはコメントアウトを外してください
 */
#define USE_32KHZ

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
 #define PWM_PIN    11
