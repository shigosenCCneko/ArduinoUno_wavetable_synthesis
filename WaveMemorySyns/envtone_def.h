/* 
*発音数 
* 16KHZで12ぐらい
* 32KHzなら5
* 4以上の発音数を指定するとoverflowした時に強い雑音が出ます
* (8bit符号付きで演算しているためoverflowに対応出来ない)
*/
#define MAX_TONE 4

/*
 * ３２KHｚで使用するときはコメントアウトを外してください
 */
//#define USE_32KHZ
