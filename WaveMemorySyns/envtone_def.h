/* 
*  MAX_TONE 発音数 
* 16KHZで12ぐらい
* 32KHzなら5～6
* 4以上の発音数を指定するとoverflowした時に雑音が出ます
* CLIPを指定時発音数が１つぐらい減少
*/
#define MAX_TONE 5

/*
 * USE_32KZ  32KHzで使用するときはコメントアウトを外してください
 */
#define USE_32KHZ

/*
 *   CLIP 波形加算時overflowした時時最大値にCLIPします
 */
#define CLIP
