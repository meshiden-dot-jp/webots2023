#include <tinkerbots_utility.h>

// A=右車輪，Aは正転で前進方向に車輪が回転
// B=左車輪，Bは正転で後退方向に車輪が回転

char* Random_value1(void){
  static char ret_value[64];
  int random_value = rand()%7; // 0~6の整数が代入される

  if(random_value==0){
    sprintf(ret_value, "30");
  }
  else if(random_value==1){
    sprintf(ret_value, "20");
  }
  else if(random_value==2){
    sprintf(ret_value, "10");
  }
  else if(random_value==3){
    sprintf(ret_value, "0");
  }
  else if(random_value==4){
    sprintf(ret_value, "-10");
  }
  else if(random_value==5){
    sprintf(ret_value, "-20");
  }
  else{ // random_value==6
    sprintf(ret_value, "-30");
  }

  return ret_value;
}

char* Random_value2(void){
  static char ret_value[64];
  int random_value = rand()%3; // 0~2の整数が代入される

  if(random_value==0){
    sprintf(ret_value, "1");
  }
  else if(random_value==1){
    sprintf(ret_value, "3");
  }
  else{ // random_value==2
    sprintf(ret_value, "5");
  }

  return ret_value;
}

int main(int argc, char **argv) {

  Initialize(); // Tinkerbotのデバイスの初期化

  // ---------------------------------
  // ここから下にロボットへの命令を書いて下さい．
  // ---------------------------------

  Repeat_until("Infinity","","",""){
    Steering("A","B","0",Random_value1());
    Keep_until("Timer","","",Random_value2());
  }

  // ---------------------------------
  // ここから上にロボットへの命令を書いて下さい．
  // ---------------------------------

  // 以降，最後の命令を永久に実行し続ける
  Repeat_until("Infinity", "", "", "");
  wb_robot_cleanup(); // Tinkerbotのデバイスの終了処理

  return EXIT_SUCCESS;
}
