// ------------------------------------------------------------------
// ヘッダファイルのインクルード
// ------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS
#include <webots/device.h>
#include <webots/led.h>
#include <webots/motor.h>
#include <webots/distance_sensor.h>
#include <webots/position_sensor.h>
#include <webots/touch_sensor.h>
#include <webots/camera.h>
#include <webots/robot.h>

#include <math.h>
#include <string.h>
#include <time.h>

#include "tinkerbots_utility.h"

// ------------------------------------------------------------------
// 定数，グローバル変数の定義
// ------------------------------------------------------------------

// モータ，センサの各デバイスの系統ごとの最大数
#define MAX_DEVICES 10
// LEDの色の数と定義
#define N_LED_COLORS 7
static const int led_colors[N_LED_COLORS] = {
	0x000000,  // OFF
	0xFF0000,  // RED
	0x00FF00,  // GREEN
	0x0000FF,  // BLUE
	//  0x00FFFF,  // MINT
	0xFFFF00,  // YELLOW
	0xFFFFFF,  // WHITE
	0xFF00FF   // PURPLE
};

// モータの角速度の最大値
#define MOTOR_MAX_SPEED 6.28
// モータの引数の最大値
#define MOTOR_MAX_INPUT 100
// 円周率
#define PI 3.1415926535897932384626433832795
// 不正な値（この値のままである場合は，適正な処理がされてない状態とする）
#define ILLEGAL_VALUE -10000000

// カラーセンサの色の数と戻り値
#define N_SENSING_COLORS 7
char sensing_colors[N_LED_COLORS][16] = {
	"None",
	"Red",
	"Green",
	"Blue",
	"Yellow",
	"White",
	"Black"
};

// 微小時間
int timestep;

// デバイスタグと各種デバイス数の宣言
WbDeviceTag motors[MAX_DEVICES];
int n_motors = 0;
WbDeviceTag touch_sensors[MAX_DEVICES];
int n_touch_sensors = 0;
WbDeviceTag distance_sensors[MAX_DEVICES];
int n_distance_sensors = 0;
WbDeviceTag color_sensors[MAX_DEVICES];
int n_color_sensors = 0;
WbDeviceTag gyro_sensors[MAX_DEVICES];
int n_gyro_sensors = 0;
WbDeviceTag position_sensors[MAX_DEVICES];
int n_position_sensors = 0;
WbDeviceTag leds[MAX_DEVICES * 4];
int n_leds = 0;

// 基準点用グローバル変数
double g_timer = 0.0;
int g_counter = 0;
double g_gyro[MAX_DEVICES] = { 0.0 };
double g_position[MAX_DEVICES] = { 0.0 };

// カラーセンサ（カメラ）のパラメータ
int camera_width = ILLEGAL_VALUE;
int camera_height = ILLEGAL_VALUE;

//double get_rotationValue(char* device_name);

// ------------------------------------------------------------------
// tinkerbotのすべてのデバイスの初期化処理
// ------------------------------------------------------------------
void Initialize(void){
	// webotsの初期化
	wb_robot_init();
	timestep = (const int)wb_robot_get_basic_time_step();

	//　乱数の初期化
	const int seed = time(NULL);
	srand(seed);

	// すべてデバイスについてタグの取得と初期化処理
	int i;
	for (i = 0; i < wb_robot_get_number_of_devices(); ++i) {
		WbDeviceTag device = wb_robot_get_device_by_index(i);
		WbNodeType device_type = wb_device_get_node_type(device);

		if (device_type == WB_NODE_ROTATIONAL_MOTOR) {
			// モータの初期化
			printf("- Ready to use Motor: %s\n", wb_device_get_name(device));
			motors[n_motors++] = device;
			wb_motor_set_position(device, INFINITY);
			wb_motor_set_velocity(device, 0.0);
		}
		else if (device_type == WB_NODE_LED) {
			// LEDの初期化
			printf("- LED: %s\n", wb_device_get_name(device));
			leds[n_leds++] = device;
			wb_led_set(device, led_colors[0]);
		}
		else if (device_type == WB_NODE_TOUCH_SENSOR) {
			// タッチセンサの初期化
			printf("- Touch Sensor: %s\n", wb_device_get_name(device));
			touch_sensors[n_touch_sensors++] = device;
			wb_touch_sensor_enable(device, timestep);
			//if(wb_touch_sensor_get_type(device)==WB_TOUCH_SENSOR_BUMPER) printf("bumper\n");
			//if(wb_touch_sensor_get_type(device)==WB_TOUCH_SENSOR_FORCE) printf("force\n");
		}
		else if (device_type == WB_NODE_DISTANCE_SENSOR) {
			// 距離センサの初期化
			printf("- Distance Sensor: %s\n", wb_device_get_name(device));
			distance_sensors[n_distance_sensors++] = device;
			wb_distance_sensor_enable(device, timestep);
		}
		else if (device_type == WB_NODE_CAMERA) {
			// カラーセンサの初期化
			printf("- Color Sensor: %s\n", wb_device_get_name(device));
			color_sensors[n_color_sensors++] = device;
			wb_camera_enable(device, timestep);
			camera_width = wb_camera_get_width(device);
			camera_height = wb_camera_get_height(device);
		}
		else if (device_type == WB_NODE_GYRO) {
			// ジャイロセンサの初期化
			printf("- Gyro Sensor: %s\n", wb_device_get_name(device));
			gyro_sensors[n_gyro_sensors++] = device;
			// （ここに初期化処理を追加）
		}
		else if (device_type == WB_NODE_POSITION_SENSOR) {
			// 角度センサ（エンコーダ）の初期化
			printf("- Rotational Sensor: %s\n", wb_device_get_name(device));
			position_sensors[n_position_sensors++] = device;
			wb_position_sensor_enable(device, timestep);
		}
	}
	// printf("n_motors: %d\n", n_motors);

	// 1ステップだけシミュレーションを動かして，センサ情報を更新しておく．（さもないとNanになる）
	int ret = wb_robot_step(timestep);
	if (ret == -1){
		wb_robot_cleanup();
		exit(EXIT_FAILURE);
	}
	return;
}

// ------------------------------------------------------------------
// 単体のモータを動かす関数
// 第1引数：デバイス名
// 第2引数：回転角速度入力．"-100"～"100"を与える．"100"のときに1回転毎秒で回転．
// ------------------------------------------------------------------
void Motor(char* device_name, char* speed){
	int i;
	// すべてのモータについて
	for (i = 0; i < n_motors; ++i) {
		// デバイス名が登録されているか照合
		if (strcmp(device_name, wb_device_get_name(motors[i])) == 0){
			int i_speed = (int)(atof(speed) + 0.5);
			// 角速度入力の上限下限の処理
			if (i_speed > MOTOR_MAX_INPUT) i_speed = MOTOR_MAX_INPUT;
			if (i_speed < -MOTOR_MAX_INPUT) i_speed = -MOTOR_MAX_INPUT;
			// モータの角速度[rad/s]の計算
			double d_speed = MOTOR_MAX_SPEED*(double)i_speed / (double)MOTOR_MAX_INPUT;
			// 角速度入力で回転するようモータに設定
			wb_motor_set_position(motors[i], INFINITY);
			wb_motor_set_velocity(motors[i], d_speed);
			// 設定できたら関数を終了する．
			return;
		}
	}
	// デバイス名が登録されていない場合，エラー出力
	printf("Error! No such device: %s\n", device_name);
	wb_robot_cleanup();
	exit(EXIT_FAILURE);
	return;
}

// ------------------------------------------------------------------
// 2つのモータを動かす関数
// 第1引数：デバイス名（右車輪）
// 第2引数：デバイス名（左車輪）
// 第3引数：曲がり具合．"-100"～"100"を与える．
//          "100"のとき，右にその場旋回
//           "50"のとき，左車輪だけを動かして右旋回
//            "0"のとき，直進
// 第4引数：回転角速度入力．"-100"～"100"を与える．"100"のときに1回転毎秒で回転．
// ------------------------------------------------------------------
void Steering(char* device_name1, char* device_name2, char* steer, char* speed){
	int i;
	int cnt = 0;
	// 入力の上限下限の設定
	int i_speed = (int)(atof(speed) + 0.5);
	if (i_speed > MOTOR_MAX_INPUT) i_speed = MOTOR_MAX_INPUT;
	if (i_speed < -MOTOR_MAX_INPUT) i_speed = -MOTOR_MAX_INPUT;
	int i_steer = (int)(atof(steer) + 0.5);
	if (i_steer > MOTOR_MAX_INPUT) i_steer = MOTOR_MAX_INPUT;
	if (i_steer < -MOTOR_MAX_INPUT) i_steer = -MOTOR_MAX_INPUT;
	double d_speed1;
	double d_speed2;
	// 左右の車輪の角速度の計算
	if (i_steer == 0){
		d_speed1 = MOTOR_MAX_SPEED*(double)i_speed / (double)MOTOR_MAX_INPUT;
		d_speed2 = -MOTOR_MAX_SPEED*(double)i_speed / (double)MOTOR_MAX_INPUT;
	}
	else if (i_steer > 0){
		d_speed1 = ((double)MOTOR_MAX_INPUT - 2.0*(double)i_steer) / (double)MOTOR_MAX_INPUT*MOTOR_MAX_SPEED*(double)i_speed / (double)MOTOR_MAX_INPUT;
		d_speed2 = -MOTOR_MAX_SPEED*(double)i_speed / (double)MOTOR_MAX_INPUT;
	}
	else{ //if(i_steer < 0)
		d_speed1 = MOTOR_MAX_SPEED*(double)i_speed / (double)MOTOR_MAX_INPUT;
		d_speed2 = -((double)MOTOR_MAX_INPUT + 2.0*(double)i_steer) / (double)MOTOR_MAX_INPUT*MOTOR_MAX_SPEED*(double)i_speed / (double)MOTOR_MAX_INPUT;
	}

	// モータに速度指令
	for (i = 0; i < n_motors; ++i) {
		if (strcmp(device_name1, wb_device_get_name(motors[i])) == 0){
			wb_motor_set_velocity(motors[i], d_speed1);
			cnt++;
		}
		else if (strcmp(device_name2, wb_device_get_name(motors[i])) == 0){
			wb_motor_set_velocity(motors[i], d_speed2);
			cnt += 2;
		}
	}

	// デバイス名が登録されていない場合，エラー出力
	if (cnt == 3) return;
	else if (cnt == 2){
		printf("Error! No such device: %s\n", device_name1);
		wb_robot_cleanup();
		exit(EXIT_FAILURE);
	}
	else if (cnt == 1){
		printf("Error! No such device: %s\n", device_name2);
		wb_robot_cleanup();
		exit(EXIT_FAILURE);
	}
	else if (cnt == 0){
		printf("Error! No such devices: %s and %s\n", device_name1, device_name2);
		wb_robot_cleanup();
		exit(EXIT_FAILURE);
	}
	return;
}
/*
// オーバーロード不可
void Motor(char* device_name, char* speed, char* mode, char* sign, char* value){
int i;
for (i = 0; i < n_motors; ++i) {
if(strcmp(device_name, wb_device_get_name(motors[i]))==0){
int i_speed = atoi(speed);
if(i_speed > MOTOR_MAX_INPUT) i_speed = MOTOR_MAX_INPUT;
if(i_speed < -MOTOR_MAX_INPUT) i_speed = -MOTOR_MAX_INPUT;
double d_speed = MOTOR_MAX_SPEED*(double)i_speed/(double)MOTOR_MAX_INPUT;
wb_motor_set_velocity(motors[i], d_speed);


return;
}
}
printf("Error! No such device: %s\n", device_name);
wb_robot_cleanup();
exit(EXIT_FAILURE);
return;
}
*/

/*
// 位置制御命令が機能しないため無効に．一度Infinityに設定してしまうと変更できなくなる？
// ------------------------------------------------------------------
// 単体のモータを動かす関数（位置制御．現在の角度から入力値だけ回転）
// 第1引数：デバイス名
// 第2引数：角度入力[deg]．現在の角度から入力値だけ回転．
// ------------------------------------------------------------------
void Joint(char* device_name, char* angle){
int i;
// すべてのモータについて
for (i = 0; i < n_motors; ++i) {
// デバイス名が登録されているか照合
if(strcmp(device_name, wb_device_get_name(motors[i]))==0){
// 現在の角度に入力した角度を加算して目標角度[rad]とする．
double d_angle = atof(angle)*PI/180.0 + get_rotationValue(device_name);
// 角速度入力で回転するようモータに設定
wb_motor_set_position(motors[i], d_angle);
// 設定できたら関数を終了する．
return;
}
}
// デバイス名が登録されていない場合，エラー出力
printf("Error! No such device: %s\n", device_name);
wb_robot_cleanup();
exit(EXIT_FAILURE);
return;
}
*/

// ------------------------------------------------------------------
// 単体のグリッパを動かす関数（速度制御）
// 第1引数：デバイス名
// 第2引数：回転角速度入力．"-100"～"100"を与える．"100"のときに1回転毎秒で回転．
// ------------------------------------------------------------------
void Hand(char* device_name, char* value){
	char tmp_device_name1[64] = "\0";
	char tmp_device_name2[64] = "\0";
	char tmp_device_name3[64] = "\0";
	// LEDのデバイス名 = "ベースのデバイス名"+" led"
	sprintf(tmp_device_name1, "%s finger A", device_name);
	sprintf(tmp_device_name2, "%s finger B", device_name);
	sprintf(tmp_device_name3, "%s finger C", device_name);

	Motor(tmp_device_name1, value);
	Motor(tmp_device_name2, value);
	Motor(tmp_device_name3, value);
	// デバイス名が登録されていない場合のエラー出力はMotor関数で行う．
	return;
}

/*
// 位置制御命令が機能しないためオミット．
// ------------------------------------------------------------------
// 単体のグリッパを動かす関数（位置制御．速度制御両方）
// 第1引数：デバイス名
// 第2引数：動作モード．
// 第3引数：角度入力[deg]or回転角速度入力[-100,+100]．
// ------------------------------------------------------------------
void Hand(char* device_name, char* mode, char* value){
char tmp_device_name1[64] = "\0";
char tmp_device_name2[64] = "\0";
char tmp_device_name3[64] = "\0";
// LEDのデバイス名 = "ベースのデバイス名"+" led"
sprintf(tmp_device_name1, "%s finger A", device_name);
sprintf(tmp_device_name2, "%s finger B", device_name);
sprintf(tmp_device_name3, "%s finger C", device_name);

if(strcmp(mode, "Motor")==0){
Motor(tmp_device_name1, value);
Motor(tmp_device_name2, value);
Motor(tmp_device_name3, value);
return;
} else if(strcmp(mode, "Joint")==0){
Joint(tmp_device_name1, value);
Joint(tmp_device_name2, value);
Joint(tmp_device_name3, value);
return;
}
// デバイス名が登録されていない場合，エラー出力
printf("Error! No such mode: %s\n", mode);
wb_robot_cleanup();
exit(EXIT_FAILURE);
return;
}
*/
// ------------------------------------------------------------------
// LEDを制御する関数
// 第1引数：デバイス名
// 第2引数：色
// ------------------------------------------------------------------
void Led(char* device_name, char* color){
	int i, j;
	char tmp_device_name[64] = "\0";
	// LEDのデバイス名 = "ベースのデバイス名"+" led"
	sprintf(tmp_device_name, "%s led", device_name);

	// すべてのLEDについて
	for (i = 0; i < n_leds; ++i) {
		// デバイス名が登録されているか照合
		if (strcmp(tmp_device_name, wb_device_get_name(leds[i])) == 0){
			// すべての色について
			for (j = 0; j < N_LED_COLORS; ++j) {
				if (strcmp(color, sensing_colors[j]) == 0){
					wb_led_set(leds[i], led_colors[j]);
					// 設定できたら関数を終了する．
					return;
				}
			}
			// 色が登録されていない場合，エラー出力
			printf("Error! No such color: %s\n", color);
			wb_robot_cleanup();
			exit(EXIT_FAILURE);
			return;
		}
	}
	// デバイス名が登録されていない場合，エラー出力
	printf("Error! No such device: %s\n", device_name);
	wb_robot_cleanup();
	exit(EXIT_FAILURE);
	return;
}

// ------------------------------------------------------------------
// 距離センサの値を取得する関数
// 第1引数：デバイス名
// ------------------------------------------------------------------
double get_distanceValue(char* device_name){
	int i;
	for (i = 0; i < n_distance_sensors; ++i) {
		if (strcmp(device_name, wb_device_get_name(distance_sensors[i])) == 0){
			return wb_distance_sensor_get_value(distance_sensors[i])*50.0 / 128.0;
		}
	}
	printf("Error! No such device: %s\n", device_name);
	wb_robot_cleanup();
	exit(EXIT_FAILURE);
	return 0.0;
}

// ------------------------------------------------------------------
// 角度センサ（エンコーダ）の値を取得する関数
// 第1引数：デバイス名
// ------------------------------------------------------------------
double get_rotationValue(char* device_name){
	int i;
	char tmp_device_name[64] = "\0";
	// 角度センサのデバイス名 = "モータのデバイス名"+" sensor"
	sprintf(tmp_device_name, "%s sensor", device_name);

	for (i = 0; i < n_position_sensors; ++i) {
		if (strcmp(tmp_device_name, wb_device_get_name(position_sensors[i])) == 0){
			//printf("%lf\n", wb_position_sensor_get_value(position_sensors[i]));
			return wb_position_sensor_get_value(position_sensors[i]);
		}
	}
	printf("Error! No such device: %s\n", device_name);
	wb_robot_cleanup();
	exit(EXIT_FAILURE);
	return 0.0;
}

// ------------------------------------------------------------------
// タッチセンサの値を取得する関数
// 第1引数：デバイス名
// ------------------------------------------------------------------
bool get_touchValue(char* device_name){
	int i;
	for (i = 0; i < n_touch_sensors; ++i) {
		if (strcmp(device_name, wb_device_get_name(touch_sensors[i])) == 0){
			if (wb_touch_sensor_get_value(touch_sensors[i])<0.5) return false;
			else return true;
		}
	}
	printf("Error! No such device: %s\n", device_name);
	wb_robot_cleanup();
	exit(EXIT_FAILURE);
	return 0.0;
}

// ------------------------------------------------------------------
// カラーセンサの値を取得する関数
// 第1引数：デバイス名
// ------------------------------------------------------------------
char* get_colorValue(char* device_name){
	int i;
	int num_votes[N_SENSING_COLORS] = { 0 };
	for (i = 0; i < n_color_sensors; ++i) {
		if (strcmp(device_name, wb_device_get_name(color_sensors[i])) == 0){
			const unsigned char *image = wb_camera_get_image(color_sensors[i]);
			// ピクセル毎で赤青緑黄白黒を判別，投票制で得票数の多い色を返す
			for (int x = 0; x < camera_width; x++){
				for (int y = 0; y < camera_height; y++){
					float r = wb_camera_image_get_red(image, camera_width, x, y) / 255.0;
					float g = wb_camera_image_get_green(image, camera_width, x, y) / 255.0;
					float b = wb_camera_image_get_blue(image, camera_width, x, y) / 255.0;
					// HSV色空間の計算
					float c_max = r > g ? r : g;
					c_max = c_max > b ? c_max : b;
					float c_min = r < g ? r : g;
					c_min = c_min < b ? c_min : b;
					float h = c_max - c_min;
					if (h > 0.0){
						if (c_max == r){
							h = (g - b) / h;
							if (h < 0.0) h += 6.0;
						}
						else if (c_max == g) h = 2.0 + (b - r) / h;
						else                h = 4.0 + (r - g) / h;
					}
					h /= 6.0;
					float s = (c_max - c_min);
					if (c_max != 0.0) s /= c_max;
					float v = c_max;

					// まず，彩度で赤青緑黄と白黒を判別
					if (s>0.5){
						// 彩度が大きい場合は，色相で赤青緑黄を判別
						// 彩度は0付近が赤，1/3付近が緑，2/3付近が緑
						if (h<20.0 / 240.0 || h >= 200.0 / 240.0)       num_votes[1]++; // 赤
						else if (h >= 20.0 / 240.0 && h<60.0 / 240.0)   num_votes[4]++; // 黄
						else if (h >= 60.0 / 240.0 && h<110.0 / 240.0)  num_votes[2]++; // 緑
						else if (h >= 110.0 / 240.0 && h<200.0 / 240.0) num_votes[3]++; // 青
						else                             num_votes[0]++; // 中途半端な場合は色なし（ここには入らないはず）
					}
					else if (s<0.2){
						// 彩度が小さい場合は，輝度で白黒を判別
						if (v>0.6)      num_votes[5]++; // 明るい場合は白
						else if (v<0.4) num_votes[6]++; // 暗い場合は黒
						else           num_votes[0]++; // 中途半端な場合は色なし
					}
					else{
						// 彩度が中途半端な場合は，色なしと判別
						num_votes[0]++;
					}
				}
			}
			int max_color = 0;
			int max_vote = 0;
			max_vote = num_votes[max_color];
			for (int j = 1; j<N_SENSING_COLORS; j++){
				if (num_votes[j]>max_vote){
					max_color = j;
					max_vote = num_votes[max_color];
				}
			}
			Led(device_name, sensing_colors[max_color]);
			return sensing_colors[max_color];
		}
	}
	printf("Error! No such device: %s\n", device_name);
	wb_robot_cleanup();
	exit(EXIT_FAILURE);
	return sensing_colors[0];
}

// ------------------------------------------------------------------
// 条件分岐．if文の()内に挿入する関数
// 第1引数：条件の種類
// 第2引数：デバイス名
// 第3引数：比較演算子
// 第4引数：値
// ------------------------------------------------------------------
bool Is_it_true(char* mode, char* device_name, char* sign, char* value){
	int i;
	if (strcmp(mode, "Distance") == 0){
		// 距離センサに関する比較の処理
		double d_value = atof(value);
		while (wb_robot_step(timestep) != -1){
			double current_value = get_distanceValue(device_name);
			if (sign[0] == '='){
				if (fabs(current_value - d_value) <= 1.0) return true;
				else return false;
			}
			else if (sign[0] == '<'){
				if (current_value < d_value) return true;
				else return false;
			}
			else if (sign[0] == '>'){
				if (current_value > d_value) return true;
				else return false;
			}
			else{
				printf("Error! No such sign: %s\n", sign);
				wb_robot_cleanup();
				exit(EXIT_FAILURE);
				return false;
			}
		}
	}
	else if ((strcmp(mode, "Motor") == 0) || (strcmp(mode, "Hand") == 0)){
		// 角度センサ（エンコーダ）に関する比較の処理
		double d_value = atof(value);
		double s_value = ILLEGAL_VALUE;
		char tmp_device_name[64] = "\0";
		char tmp_device_name2[64] = "\0";
		if (strcmp(mode, "Motor") == 0) sprintf(tmp_device_name2, "%s", device_name);
		else sprintf(tmp_device_name2, "%s finger A", device_name);
		sprintf(tmp_device_name, "%s sensor", tmp_device_name2);
		for (i = 0; i < n_position_sensors; ++i) {
			if (strcmp(tmp_device_name, wb_device_get_name(position_sensors[i])) == 0){
				s_value = g_position[i];
			}
		}
		if (s_value == ILLEGAL_VALUE){
			printf("Error! No such device: %s\n", device_name);
			wb_robot_cleanup();
			exit(EXIT_FAILURE);
			return false;
		}

		double current_value = get_rotationValue(tmp_device_name2);
		double difference_value = (current_value - s_value)*180.0 / PI;
		//printf("%lf - %lf = %lf\n", current_value, s_value, difference_value);
		if (sign[0] == '='){
			if (fabs(difference_value - d_value) <= 1.0) return true;
			else return false;
		}
		else if (sign[0] == '<'){
			if (difference_value < d_value) return true;
			else return false;
		}
		else if (sign[0] == '>'){
			if (difference_value > d_value) return true;
			else return false;
		}
		else{
			printf("Error! No such sign: %s\n", sign);
			wb_robot_cleanup();
			exit(EXIT_FAILURE);
			return false;
		}
	}
	else if (strcmp(mode, "Touch") == 0){
		// タッチセンサに関する比較の処理
		if (strcmp(value, "On") == 0){
			return(get_touchValue(device_name));
		}
		else if (strcmp(value, "Off") == 0){
			return(!get_touchValue(device_name));
		}
		else{
			printf("Error! No such value: %s\n", value);
			wb_robot_cleanup();
			exit(EXIT_FAILURE);
			return true;
		}
	}
	else if (strcmp(mode, "Color") == 0){
		// カラーセンサに関する比較の処理
		if (strcmp(value, get_colorValue(device_name)) == 0) return true;
		else return false;
	}
	printf("Error! No such mode: %s\n", mode);
	wb_robot_cleanup();
	exit(EXIT_FAILURE);
	return false;
}

// ------------------------------------------------------------------
// 動作の継続．引数の条件に到達するまで，動作を継続する．
// 第1引数：条件の種類
// 第2引数：デバイス名
// 第3引数：比較演算子
// 第4引数：値
// ------------------------------------------------------------------
void Keep_until(char* mode, char* device_name, char* sign, char* value){
	if (strcmp(mode, "Timer") == 0){
		// 時間に関する比較の処理
		double start_time = wb_robot_get_time();
		double d_time = atof(value);
		// 条件を満足するまで，シミュレーションを1ステップずつ進める
		while (wb_robot_step(timestep) != -1){
			double current_time = wb_robot_get_time();
			if (current_time - start_time >= d_time) return;
		}
	}
	else if (strcmp(mode, "Distance") == 0){
		// 距離センサに関する比較の処理
		double d_value = atof(value);
		while (wb_robot_step(timestep) != -1){
			double current_value = get_distanceValue(device_name);
			if (sign[0] == '='){
				if (fabs(current_value - d_value) <= 1.0) return;
			}
			else if (sign[0] == '<'){
				if (current_value < d_value) return;
			}
			else if (sign[0] == '>'){
				if (current_value > d_value) return;
			}
			else{
				printf("Error! No such sign: %s\n", sign);
				wb_robot_cleanup();
				exit(EXIT_FAILURE);
				return;
			}
		}
	}
	else if ((strcmp(mode, "Motor") == 0) || (strcmp(mode, "Hand") == 0)){
		// 角度センサ（エンコーダ）に関する比較の処理
		double d_value = atof(value);
		char tmp_device_name[64] = "\0";
		// 指Aのデバイス名 = "ベースのデバイス名"+" finger A"
		if (strcmp(mode, "Motor") == 0) sprintf(tmp_device_name, "%s", device_name);
		else sprintf(tmp_device_name, "%s finger A", device_name);
		double s_value = get_rotationValue(tmp_device_name);
		while (wb_robot_step(timestep) != -1){
			double current_value = get_rotationValue(tmp_device_name);
			double difference_value = (current_value - s_value)*180.0 / PI;
			//printf("%lf - %lf = %lf\n", current_value, s_value, difference_value);
			if (sign[0] == '='){
				if (fabs(difference_value - d_value) <= 1.0) return;
			}
			else if (sign[0] == '<'){
				if (difference_value < d_value) return;
			}
			else if (sign[0] == '>'){
				if (difference_value > d_value) return;
			}
			else{
				printf("Error! No such sign: %s\n", sign);
				wb_robot_cleanup();
				exit(EXIT_FAILURE);
				return;
			}
		}
	}
	else if (strcmp(mode, "Touch") == 0){
		// タッチセンサに関する比較の処理
		while (wb_robot_step(timestep) != -1){
			if (strcmp(value, "On") == 0){
				if (get_touchValue(device_name) == true) return;
			}
			else if (strcmp(value, "Off") == 0){
				if (get_touchValue(device_name) == false) return;
			}
			else{
				printf("Error! No such value: %s\n", value);
				wb_robot_cleanup();
				exit(EXIT_FAILURE);
				return;
			}
		}
	}
	else if (strcmp(mode, "Color") == 0){
		// カラーセンサに関する比較の処理
		while (wb_robot_step(timestep) != -1){
			if (strcmp(value, get_colorValue(device_name)) == 0) return;
		}
	}
	printf("Error! No such mode: %s\n", mode);
	wb_robot_cleanup();
	exit(EXIT_FAILURE);
	return;
}

// ------------------------------------------------------------------
// 基準点用グローバル変数の初期化処理・基準点の設定
// 第1引数：条件の種類
// 第2引数：デバイス名
// ------------------------------------------------------------------
void Reset(char* mode, char* device_name){
	int i;
	if (strcmp(mode, "Infinity") == 0){
		return;
	}
	else if (strcmp(mode, "Timer") == 0){
		g_timer = wb_robot_get_time();
		return;
	}
	else if (strcmp(mode, "Counter") == 0){
		g_counter = 0;
		return;
	}
	else if (strcmp(mode, "Gyro") == 0){
		for (i = 0; i < n_gyro_sensors; ++i) {
			if (strcmp(device_name, wb_device_get_name(gyro_sensors[i])) == 0){
				g_gyro[i] = 0.0;
				return;
			}
		}
		printf("Error! No such device: %s\n", mode);
		wb_robot_cleanup();
		exit(EXIT_FAILURE);
		return;
	}
	else if (strcmp(mode, "Distance") == 0){
		return;
	}
	else if ((strcmp(mode, "Motor") == 0) || (strcmp(mode, "Hand") == 0)){
		char tmp_device_name[64] = "\0";
		char tmp_device_name2[64] = "\0";
		if (strcmp(mode, "Motor") == 0) sprintf(tmp_device_name2, "%s", device_name);
		else sprintf(tmp_device_name2, "%s finger A", device_name);
		sprintf(tmp_device_name, "%s sensor", tmp_device_name2);
		for (i = 0; i < n_position_sensors; ++i) {
			if (strcmp(tmp_device_name, wb_device_get_name(position_sensors[i])) == 0){
				g_position[i] = get_rotationValue(tmp_device_name2);
				return;
			}
		}
		printf("Error! No such device: %s\n", device_name);
		wb_robot_cleanup();
		exit(EXIT_FAILURE);
		return;
	}
	else if (strcmp(mode, "Touch") == 0){
		return;
	}
	else if (strcmp(mode, "Color") == 0){
		return;
	}
	printf("Error! No such mode: %s\n", mode);
	wb_robot_cleanup();
	exit(EXIT_FAILURE);
	return;
}

/*
void prepare(void){
g_timer = wb_robot_get_time();
g_counter = 0;
return;
}
*/

// ------------------------------------------------------------------
// 動作の繰り返し．引数の条件に到達するまで，動作を継続する．
// 第1引数：条件の種類
// 第2引数：デバイス名
// 第3引数：比較演算子
// 第4引数：値
// ------------------------------------------------------------------
bool repeat_until(char* mode, char* device_name, char* sign, char* value){
	int i;
	// この関数に入るたび，シミュレーションを1ステップ進める
	int ret = wb_robot_step(timestep);
	if (ret == -1){
		wb_robot_cleanup();
		return EXIT_SUCCESS;
	}
	else{
		if (strcmp(mode, "Infinity") == 0){
			// 無限に繰り返す
			return true;
		}
		else if (strcmp(mode, "Timer") == 0){
			// 時間に関する比較の処理
			double d_time = atof(value);
			double current_time = wb_robot_get_time();
			if (current_time - g_timer >= d_time) return false;
			else return true;
		}
		else if (strcmp(mode, "Counter") == 0){
			// 繰り返し回数に関する比較の処理
			int i_conut = atoi(value);
			if (g_counter >= i_conut) return false;
			else{
				g_counter++;
				return true;
			}
		}
		else if (strcmp(mode, "Distance") == 0){
			// 距離センサに関する比較の処理
			double d_value = atof(value);
			double current_value = get_distanceValue(device_name);
			if (sign[0] == '='){
				if (fabs(current_value - d_value) <= 1.0) return false;
				else return true;
			}
			else if (sign[0] == '<'){
				if (current_value < d_value) return false;
				else return true;
			}
			else if (sign[0] == '>'){
				if (current_value > d_value) return false;
				else return true;
			}
			else{
				printf("Error! No such sign: %s\n", sign);
				wb_robot_cleanup();
				exit(EXIT_FAILURE);
				return true;
			}
		}
		else if ((strcmp(mode, "Motor") == 0) || (strcmp(mode, "Hand") == 0)){
			// 角度センサ（エンコーダ）に関する比較の処理
			double d_value = atof(value);
			double s_value = ILLEGAL_VALUE;
			char tmp_device_name[64] = "\0";
			char tmp_device_name2[64] = "\0";
			if (strcmp(mode, "Motor") == 0) sprintf(tmp_device_name2, "%s", device_name);
			else sprintf(tmp_device_name2, "%s finger A", device_name);
			sprintf(tmp_device_name, "%s sensor", tmp_device_name2);
			for (i = 0; i < n_position_sensors; ++i) {
				if (strcmp(tmp_device_name, wb_device_get_name(position_sensors[i])) == 0){
					s_value = g_position[i];
				}
			}
			if (s_value == ILLEGAL_VALUE){
				printf("Error! No such device: %s\n", device_name);
				wb_robot_cleanup();
				exit(EXIT_FAILURE);
				return true;
			}

			double current_value = get_rotationValue(tmp_device_name2);
			double difference_value = (current_value - s_value)*180.0 / PI;
			//printf("%lf - %lf = %lf\n", current_value, s_value, difference_value);
			if (sign[0] == '='){
				if (fabs(difference_value - d_value) <= 1.0) return false;
				else return true;
			}
			else if (sign[0] == '<'){
				if (difference_value < d_value) return false;
				else return true;
			}
			else if (sign[0] == '>'){
				if (difference_value > d_value) return false;
				else return true;
			}
			else{
				printf("Error! No such sign: %s\n", sign);
				wb_robot_cleanup();
				exit(EXIT_FAILURE);
				return true;
			}
		}
		else if (strcmp(mode, "Touch") == 0){
			// タッチセンサに関する比較の処理
			if (strcmp(value, "On") == 0){
				return(!get_touchValue(device_name));
			}
			else if (strcmp(value, "Off") == 0){
				return(get_touchValue(device_name));
			}
			else{
				printf("Error! No such value: %s\n", value);
				wb_robot_cleanup();
				exit(EXIT_FAILURE);
				return true;
			}
		}
		else if (strcmp(mode, "Color") == 0){
			// カラーセンサに関する比較の処理
			if (strcmp(value, get_colorValue(device_name)) == 0) return false;
			else return true;
		}

		printf("Error! No such mode: %s\n", mode);
		wb_robot_cleanup();
		exit(EXIT_FAILURE);
		return true;
	}
}

// ------------------------------------------------------------------
// 動作の繰り返し．引数の条件に到達するまで，動作を継続する．
// マクロでwhile文とその前の初期化処理を一括して記述できるようにしている．
// 第1引数：条件の種類
// 第2引数：デバイス名
// 第3引数：比較演算子
// 第4引数：値
// ------------------------------------------------------------------
#define Repeat_until(a, b, c, d) \
	Reset(a, b); \
while (repeat_until(a, b, c, d))
