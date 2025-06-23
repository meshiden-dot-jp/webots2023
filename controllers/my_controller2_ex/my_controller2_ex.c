#include <tinkerbots_utility.h>

// A=右車輪，Aは正転で前進方向に車輪が回転
// B=左車輪，Bは正転で後退方向に車輪が回転

int main(int argc, char **argv) {

  Initialize(); // Tinkerbotのデバイスの初期化

  // ---------------------------------
  // ここから下にロボットへの命令を書いて下さい．
  // ---------------------------------

  Repeat_until("Counter","","","4"){
  
  Steering("A","B","100","10");
  Keep_until("Timer","","","5.29");
  Steering("A","B","0","50");
  Keep_until("Timer","","","3.15");
  
  }
  
  Repeat_until("Counter","","","3"){
  
  Steering("A","B","100","10");
  Keep_until("Timer","","","7.5");
  Steering("A","B","0","100");
  Keep_until("Timer","","","2");
  
  }
  
  Steering("A","B","0","0");

  // ---------------------------------
  // ここから上にロボットへの命令を書いて下さい．
  // ---------------------------------

  // 以降，最後の命令を永久に実行し続ける
  Repeat_until("Infinity", "", "", "");
  wb_robot_cleanup(); // Tinkerbotのデバイスの終了処理

  return EXIT_SUCCESS;
}
