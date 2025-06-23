#include <tinkerbots_utility.h>
#include <webots/display.h>
#include <webots/robot.h>

int main() {
  Initialize();

  WbDeviceTag display = wb_robot_get_device("display");

  WbImageRef Red = wb_display_image_load(display, "red.png");
  WbImageRef Blue = wb_display_image_load(display, "blue.png");
  WbImageRef Green = wb_display_image_load(display, "green.png");
  WbImageRef Yellow = wb_display_image_load(display, "yellow.png");
  WbImageRef White = wb_display_image_load(display, "white.png");
  WbImageRef Black = wb_display_image_load(display, "black.png");

  // ---------------------------------
  // ここから下にロボットへの命令を書いて下さい．
  // ---------------------------------

  Repeat_until("Infinity","","",""){
    wb_display_image_paste(display, Blue, 0, 0, false);
    Keep_until("Timer", "", "", "4.5");

    wb_display_image_paste(display, Yellow, 0, 0, false);
    Keep_until("Timer", "", "", "2");

    wb_display_image_paste(display, Red, 0, 0, false);
    Keep_until("Timer", "", "", "4");
  }

  // ---------------------------------
  // ここから上にロボットへの命令を書いて下さい．
  // ---------------------------------

  // 以降，最後の命令を永久に実行し続ける
  Repeat_until("Infinity", "", "", "");

  wb_display_image_delete(display, Red);
  wb_display_image_delete(display, Blue);
  wb_display_image_delete(display, Green);
  wb_display_image_delete(display, Yellow);
  wb_display_image_delete(display, White);
  wb_display_image_delete(display, Black);

  wb_robot_cleanup();

  return 0;
}
