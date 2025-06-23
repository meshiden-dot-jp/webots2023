#include <tinkerbots_utility.h>

// A=右車輪，Aは正転で前進方向に車輪が回転
// B=左車輪，Bは正転で後退方向に車輪が回転

int main(int argc, char **argv) {

  Initialize(); // Tinkerbotのデバイスの初期化

  // ---------------------------------
  // ここから下にロボットへの命令を書いて下さい．
  // ---------------------------------

  Repeat_until("Infinity","","",""){
  
    Steering("A","B","100","15");
    Keep_until("Motor","A","=","-193");
    
    Steering("A","B","0","0");
    Keep_until("Timer","","","1");
    
    Steering("A","B","0","50");
    Keep_until("Touch","1","","On");
    
    Steering("A","B","0","-30");
    Keep_until("Timer","","","1");
    
    Steering("A","B","0","0");
    Keep_until("Timer","","","1");
    
  }


  // ---------------------------------
  // ここから上にロボットへの命令を書いて下さい．
  // ---------------------------------

  // 以降，最後の命令を永久に実行し続ける
  Repeat_until("Infinity", "", "", "");
  wb_robot_cleanup(); // Tinkerbotのデバイスの終了処理

  return EXIT_SUCCESS;
}
