#include <tinkerbots_utility.h>

// A=右車輪，Aは正転で前進方向に車輪が回転
// B=左車輪，Bは正転で後退方向に車輪が回転

int main(int argc, char **argv) {

  Initialize(); // Tinkerbotのデバイスの初期化

  // ---------------------------------
  // ここから下にロボットへの命令を書いて下さい．
  // ---------------------------------

  // 無限に繰り返す
  Repeat_until("Infinity","","",""){
  
  // どのような動作をしたかを書こう
  Steering("A","B","30","100");
  Keep_until("Timer","","","2");
  
  // どのような動作をしたかを書こう
  Steering("A","B","-30","100");
  Keep_until("Timer","","","2");
  
  }

  // ---------------------------------
  // ここから上にロボットへの命令を書いて下さい．
  // ---------------------------------

  // 以降，最後の命令を永久に実行し続ける
  Repeat_until("Infinity", "", "", "");
  wb_robot_cleanup(); // Tinkerbotのデバイスの終了処理

  return EXIT_SUCCESS;
}
