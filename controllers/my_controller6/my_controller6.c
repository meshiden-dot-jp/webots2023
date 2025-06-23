#include <tinkerbots_utility.h>

// A=右車輪，Aは正転で前進方向に車輪が回転
// B=左車輪，Bは正転で後退方向に車輪が回転

int main(int argc, char **argv) {

  Initialize(); // Tinkerbotのデバイスの初期化

  // ---------------------------------
  // ここから下にロボットへの命令を書いて下さい．
  // ---------------------------------

  //前進してカラーセンサーが認識する位置に行く
  Steering("A","B","0","10");
  Keep_until("Motor","A","=","400");
  Steering("A","B","0","0");
  
  //カラーセンサの認識によって場合分けする
  Repeat_until("Counter","","","2"){
  
  //条件1が成立
  if(Is_it_true("Color","G","","Blue") ){
  
   //後退する
   Steering("A","B","0","-10");
   Keep_until("Motor","A","=","-400");
   Steering("A","B","0","0");
   
   //左に曲がる
   Steering("A","B","-100","10");
   Keep_until("Motor","A","=","193");
   Steering("A","B","0","0");
   
   //目標の位置に調整する
   Steering("A","B","0","10");
   Keep_until("Motor","A","=","90");
   Steering("A","B","0","0");
   
   //右に曲がる
   Steering("A","B","-100","-10");
   Keep_until("Motor","A","=","-193");
   Steering("A","B","0","0");
   
   
   //アームを開く
   Hand("E", "15");
   Keep_until("Hand", "E", "=", "30");
   Hand("E", "0");
  
   //対象に接近する
   Steering("A","B","0","10");
   Keep_until("Motor","A","=","220");
   Steering("A","B","0","0");
  
   //アームを閉じる
   Hand("E","-30");
   Keep_until("Timer","","","1");
   Hand("E", "0");
  
   //アームを曲げる
   Motor("C","-5");
   Keep_until("Motor","C","=","-90");
   Motor("C","0");

   //アームを曲げる
   Motor("D","5");
   Keep_until("Motor","D","=","90");
   Motor("D","0");

   //後退する
   Steering("A","B","0","-10");
   Keep_until("Motor","A","=","-262");
   Steering("A","B","0","0");
  
  }
  
  //条件1,2が不成立
  else{
  
   //後退する
   Steering("A","B","0","-10");
   Keep_until("Motor","A","=","-400");
   Steering("A","B","0","0");
  
   
   //左に曲がる
   Steering("A","B","-100","10");
   Keep_until("Motor","A","=","193");
   Steering("A","B","0","0");
   Keep_until("Timer","","","1");
  
   //目標の位置に調整する
   Steering("A","B","0","10");
   Keep_until("Motor","A","=","230");
   Steering("A","B","0","0");
   
   //右に曲がる
   Steering("A","B","-100","-10");
   Keep_until("Motor","A","=","-193");
   Steering("A","B","0","0");
   
   //次の目標の色をカラーセンサーが認識できるまで前進する
   Steering("A","B","0","10");
   Keep_until("Motor","A","=","400");
   Steering("A","B","0","0");
   
  }
  }

  //右に回転をする
  Steering("A","B","-100","-10");
  Keep_until("Motor","A","=","-390");
  Steering("A","B","0","0");
  
  //集積エリアの境界を判定する
  Steering("A","B","0","10");
  Keep_until("Color","G","","Black");
  Steering("A","B","0","0");
  
  //アームを開く
  Hand("E", "15");
  Keep_until("Hand", "E", "=", "30");
  Hand("E", "0");
  
  //アームと閉じる
  Hand("E", "-15");
  Keep_until("Timer", "", "", "1");
  Hand("E", "0");
  
  //右に回転する
  Steering("A","B","-100","-10");
  Keep_until("Motor","A","=","-390");
  Steering("A","B","0","0");
  
  //初期位置に戻る
  Steering("A","B","0","10");
  Keep_until("Timer","","","5");
  Steering("A","B","0","0");
 
   
  // ---------------------------------
  // ここから上にロボットへの命令を書いて下さい．
  // ---------------------------------

  // 以降，最後の命令を永久に実行し続ける
  Repeat_until("Infinity", "", "", "");
  wb_robot_cleanup(); // Tinkerbotのデバイスの終了処理

  return EXIT_SUCCESS;
}
