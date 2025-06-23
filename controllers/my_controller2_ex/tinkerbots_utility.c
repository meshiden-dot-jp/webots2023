// ------------------------------------------------------------------
// �w�b�_�t�@�C���̃C���N���[�h
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
// �萔�C�O���[�o���ϐ��̒�`
// ------------------------------------------------------------------

// ���[�^�C�Z���T�̊e�f�o�C�X�̌n�����Ƃ̍ő吔
#define MAX_DEVICES 10
// LED�̐F�̐��ƒ�`
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

// ���[�^�̊p���x�̍ő�l
#define MOTOR_MAX_SPEED 6.28
// ���[�^�̈����̍ő�l
#define MOTOR_MAX_INPUT 100
// �~����
#define PI 3.1415926535897932384626433832795
// �s���Ȓl�i���̒l�̂܂܂ł���ꍇ�́C�K���ȏ���������ĂȂ���ԂƂ���j
#define ILLEGAL_VALUE -10000000

// �J���[�Z���T�̐F�̐��Ɩ߂�l
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

// ��������
int timestep;

// �f�o�C�X�^�O�Ɗe��f�o�C�X���̐錾
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

// ��_�p�O���[�o���ϐ�
double g_timer = 0.0;
int g_counter = 0;
double g_gyro[MAX_DEVICES] = { 0.0 };
double g_position[MAX_DEVICES] = { 0.0 };

// �J���[�Z���T�i�J�����j�̃p�����[�^
int camera_width = ILLEGAL_VALUE;
int camera_height = ILLEGAL_VALUE;

//double get_rotationValue(char* device_name);

// ------------------------------------------------------------------
// tinkerbot�̂��ׂẴf�o�C�X�̏���������
// ------------------------------------------------------------------
void Initialize(void){
	// webots�̏�����
	wb_robot_init();
	timestep = (const int)wb_robot_get_basic_time_step();

	//�@�����̏�����
	const int seed = time(NULL);
	srand(seed);

	// ���ׂăf�o�C�X�ɂ��ă^�O�̎擾�Ə���������
	int i;
	for (i = 0; i < wb_robot_get_number_of_devices(); ++i) {
		WbDeviceTag device = wb_robot_get_device_by_index(i);
		WbNodeType device_type = wb_device_get_node_type(device);

		if (device_type == WB_NODE_ROTATIONAL_MOTOR) {
			// ���[�^�̏�����
			printf("- Ready to use Motor: %s\n", wb_device_get_name(device));
			motors[n_motors++] = device;
			wb_motor_set_position(device, INFINITY);
			wb_motor_set_velocity(device, 0.0);
		}
		else if (device_type == WB_NODE_LED) {
			// LED�̏�����
			printf("- LED: %s\n", wb_device_get_name(device));
			leds[n_leds++] = device;
			wb_led_set(device, led_colors[0]);
		}
		else if (device_type == WB_NODE_TOUCH_SENSOR) {
			// �^�b�`�Z���T�̏�����
			printf("- Touch Sensor: %s\n", wb_device_get_name(device));
			touch_sensors[n_touch_sensors++] = device;
			wb_touch_sensor_enable(device, timestep);
			//if(wb_touch_sensor_get_type(device)==WB_TOUCH_SENSOR_BUMPER) printf("bumper\n");
			//if(wb_touch_sensor_get_type(device)==WB_TOUCH_SENSOR_FORCE) printf("force\n");
		}
		else if (device_type == WB_NODE_DISTANCE_SENSOR) {
			// �����Z���T�̏�����
			printf("- Distance Sensor: %s\n", wb_device_get_name(device));
			distance_sensors[n_distance_sensors++] = device;
			wb_distance_sensor_enable(device, timestep);
		}
		else if (device_type == WB_NODE_CAMERA) {
			// �J���[�Z���T�̏�����
			printf("- Color Sensor: %s\n", wb_device_get_name(device));
			color_sensors[n_color_sensors++] = device;
			wb_camera_enable(device, timestep);
			camera_width = wb_camera_get_width(device);
			camera_height = wb_camera_get_height(device);
		}
		else if (device_type == WB_NODE_GYRO) {
			// �W���C���Z���T�̏�����
			printf("- Gyro Sensor: %s\n", wb_device_get_name(device));
			gyro_sensors[n_gyro_sensors++] = device;
			// �i�����ɏ�����������ǉ��j
		}
		else if (device_type == WB_NODE_POSITION_SENSOR) {
			// �p�x�Z���T�i�G���R�[�_�j�̏�����
			printf("- Rotational Sensor: %s\n", wb_device_get_name(device));
			position_sensors[n_position_sensors++] = device;
			wb_position_sensor_enable(device, timestep);
		}
	}
	// printf("n_motors: %d\n", n_motors);

	// 1�X�e�b�v�����V�~�����[�V�����𓮂����āC�Z���T�����X�V���Ă����D�i�����Ȃ���Nan�ɂȂ�j
	int ret = wb_robot_step(timestep);
	if (ret == -1){
		wb_robot_cleanup();
		exit(EXIT_FAILURE);
	}
	return;
}

// ------------------------------------------------------------------
// �P�̂̃��[�^�𓮂����֐�
// ��1�����F�f�o�C�X��
// ��2�����F��]�p���x���́D"-100"�`"100"��^����D"100"�̂Ƃ���1��]���b�ŉ�]�D
// ------------------------------------------------------------------
void Motor(char* device_name, char* speed){
	int i;
	// ���ׂẴ��[�^�ɂ���
	for (i = 0; i < n_motors; ++i) {
		// �f�o�C�X�����o�^����Ă��邩�ƍ�
		if (strcmp(device_name, wb_device_get_name(motors[i])) == 0){
			int i_speed = (int)(atof(speed) + 0.5);
			// �p���x���͂̏�������̏���
			if (i_speed > MOTOR_MAX_INPUT) i_speed = MOTOR_MAX_INPUT;
			if (i_speed < -MOTOR_MAX_INPUT) i_speed = -MOTOR_MAX_INPUT;
			// ���[�^�̊p���x[rad/s]�̌v�Z
			double d_speed = MOTOR_MAX_SPEED*(double)i_speed / (double)MOTOR_MAX_INPUT;
			// �p���x���͂ŉ�]����悤���[�^�ɐݒ�
			wb_motor_set_position(motors[i], INFINITY);
			wb_motor_set_velocity(motors[i], d_speed);
			// �ݒ�ł�����֐����I������D
			return;
		}
	}
	// �f�o�C�X�����o�^����Ă��Ȃ��ꍇ�C�G���[�o��
	printf("Error! No such device: %s\n", device_name);
	wb_robot_cleanup();
	exit(EXIT_FAILURE);
	return;
}

// ------------------------------------------------------------------
// 2�̃��[�^�𓮂����֐�
// ��1�����F�f�o�C�X���i�E�ԗցj
// ��2�����F�f�o�C�X���i���ԗցj
// ��3�����F�Ȃ����D"-100"�`"100"��^����D
//          "100"�̂Ƃ��C�E�ɂ��̏����
//           "50"�̂Ƃ��C���ԗւ����𓮂����ĉE����
//            "0"�̂Ƃ��C���i
// ��4�����F��]�p���x���́D"-100"�`"100"��^����D"100"�̂Ƃ���1��]���b�ŉ�]�D
// ------------------------------------------------------------------
void Steering(char* device_name1, char* device_name2, char* steer, char* speed){
	int i;
	int cnt = 0;
	// ���͂̏�������̐ݒ�
	int i_speed = (int)(atof(speed) + 0.5);
	if (i_speed > MOTOR_MAX_INPUT) i_speed = MOTOR_MAX_INPUT;
	if (i_speed < -MOTOR_MAX_INPUT) i_speed = -MOTOR_MAX_INPUT;
	int i_steer = (int)(atof(steer) + 0.5);
	if (i_steer > MOTOR_MAX_INPUT) i_steer = MOTOR_MAX_INPUT;
	if (i_steer < -MOTOR_MAX_INPUT) i_steer = -MOTOR_MAX_INPUT;
	double d_speed1;
	double d_speed2;
	// ���E�̎ԗւ̊p���x�̌v�Z
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

	// ���[�^�ɑ��x�w��
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

	// �f�o�C�X�����o�^����Ă��Ȃ��ꍇ�C�G���[�o��
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
// �I�[�o�[���[�h�s��
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
// �ʒu���䖽�߂��@�\���Ȃ����ߖ����ɁD��xInfinity�ɐݒ肵�Ă��܂��ƕύX�ł��Ȃ��Ȃ�H
// ------------------------------------------------------------------
// �P�̂̃��[�^�𓮂����֐��i�ʒu����D���݂̊p�x������͒l������]�j
// ��1�����F�f�o�C�X��
// ��2�����F�p�x����[deg]�D���݂̊p�x������͒l������]�D
// ------------------------------------------------------------------
void Joint(char* device_name, char* angle){
int i;
// ���ׂẴ��[�^�ɂ���
for (i = 0; i < n_motors; ++i) {
// �f�o�C�X�����o�^����Ă��邩�ƍ�
if(strcmp(device_name, wb_device_get_name(motors[i]))==0){
// ���݂̊p�x�ɓ��͂����p�x�����Z���ĖڕW�p�x[rad]�Ƃ���D
double d_angle = atof(angle)*PI/180.0 + get_rotationValue(device_name);
// �p���x���͂ŉ�]����悤���[�^�ɐݒ�
wb_motor_set_position(motors[i], d_angle);
// �ݒ�ł�����֐����I������D
return;
}
}
// �f�o�C�X�����o�^����Ă��Ȃ��ꍇ�C�G���[�o��
printf("Error! No such device: %s\n", device_name);
wb_robot_cleanup();
exit(EXIT_FAILURE);
return;
}
*/

// ------------------------------------------------------------------
// �P�̂̃O���b�p�𓮂����֐��i���x����j
// ��1�����F�f�o�C�X��
// ��2�����F��]�p���x���́D"-100"�`"100"��^����D"100"�̂Ƃ���1��]���b�ŉ�]�D
// ------------------------------------------------------------------
void Hand(char* device_name, char* value){
	char tmp_device_name1[64] = "\0";
	char tmp_device_name2[64] = "\0";
	char tmp_device_name3[64] = "\0";
	// LED�̃f�o�C�X�� = "�x�[�X�̃f�o�C�X��"+" led"
	sprintf(tmp_device_name1, "%s finger A", device_name);
	sprintf(tmp_device_name2, "%s finger B", device_name);
	sprintf(tmp_device_name3, "%s finger C", device_name);

	Motor(tmp_device_name1, value);
	Motor(tmp_device_name2, value);
	Motor(tmp_device_name3, value);
	// �f�o�C�X�����o�^����Ă��Ȃ��ꍇ�̃G���[�o�͂�Motor�֐��ōs���D
	return;
}

/*
// �ʒu���䖽�߂��@�\���Ȃ����߃I�~�b�g�D
// ------------------------------------------------------------------
// �P�̂̃O���b�p�𓮂����֐��i�ʒu����D���x���䗼���j
// ��1�����F�f�o�C�X��
// ��2�����F���샂�[�h�D
// ��3�����F�p�x����[deg]or��]�p���x����[-100,+100]�D
// ------------------------------------------------------------------
void Hand(char* device_name, char* mode, char* value){
char tmp_device_name1[64] = "\0";
char tmp_device_name2[64] = "\0";
char tmp_device_name3[64] = "\0";
// LED�̃f�o�C�X�� = "�x�[�X�̃f�o�C�X��"+" led"
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
// �f�o�C�X�����o�^����Ă��Ȃ��ꍇ�C�G���[�o��
printf("Error! No such mode: %s\n", mode);
wb_robot_cleanup();
exit(EXIT_FAILURE);
return;
}
*/
// ------------------------------------------------------------------
// LED�𐧌䂷��֐�
// ��1�����F�f�o�C�X��
// ��2�����F�F
// ------------------------------------------------------------------
void Led(char* device_name, char* color){
	int i, j;
	char tmp_device_name[64] = "\0";
	// LED�̃f�o�C�X�� = "�x�[�X�̃f�o�C�X��"+" led"
	sprintf(tmp_device_name, "%s led", device_name);

	// ���ׂĂ�LED�ɂ���
	for (i = 0; i < n_leds; ++i) {
		// �f�o�C�X�����o�^����Ă��邩�ƍ�
		if (strcmp(tmp_device_name, wb_device_get_name(leds[i])) == 0){
			// ���ׂĂ̐F�ɂ���
			for (j = 0; j < N_LED_COLORS; ++j) {
				if (strcmp(color, sensing_colors[j]) == 0){
					wb_led_set(leds[i], led_colors[j]);
					// �ݒ�ł�����֐����I������D
					return;
				}
			}
			// �F���o�^����Ă��Ȃ��ꍇ�C�G���[�o��
			printf("Error! No such color: %s\n", color);
			wb_robot_cleanup();
			exit(EXIT_FAILURE);
			return;
		}
	}
	// �f�o�C�X�����o�^����Ă��Ȃ��ꍇ�C�G���[�o��
	printf("Error! No such device: %s\n", device_name);
	wb_robot_cleanup();
	exit(EXIT_FAILURE);
	return;
}

// ------------------------------------------------------------------
// �����Z���T�̒l���擾����֐�
// ��1�����F�f�o�C�X��
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
// �p�x�Z���T�i�G���R�[�_�j�̒l���擾����֐�
// ��1�����F�f�o�C�X��
// ------------------------------------------------------------------
double get_rotationValue(char* device_name){
	int i;
	char tmp_device_name[64] = "\0";
	// �p�x�Z���T�̃f�o�C�X�� = "���[�^�̃f�o�C�X��"+" sensor"
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
// �^�b�`�Z���T�̒l���擾����֐�
// ��1�����F�f�o�C�X��
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
// �J���[�Z���T�̒l���擾����֐�
// ��1�����F�f�o�C�X��
// ------------------------------------------------------------------
char* get_colorValue(char* device_name){
	int i;
	int num_votes[N_SENSING_COLORS] = { 0 };
	for (i = 0; i < n_color_sensors; ++i) {
		if (strcmp(device_name, wb_device_get_name(color_sensors[i])) == 0){
			const unsigned char *image = wb_camera_get_image(color_sensors[i]);
			// �s�N�Z�����ŐԐΉ������𔻕ʁC���[���œ��[���̑����F��Ԃ�
			for (int x = 0; x < camera_width; x++){
				for (int y = 0; y < camera_height; y++){
					float r = wb_camera_image_get_red(image, camera_width, x, y) / 255.0;
					float g = wb_camera_image_get_green(image, camera_width, x, y) / 255.0;
					float b = wb_camera_image_get_blue(image, camera_width, x, y) / 255.0;
					// HSV�F��Ԃ̌v�Z
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

					// �܂��C�ʓx�ŐԐΉ��Ɣ����𔻕�
					if (s>0.5){
						// �ʓx���傫���ꍇ�́C�F���ŐԐΉ��𔻕�
						// �ʓx��0�t�߂��ԁC1/3�t�߂��΁C2/3�t�߂���
						if (h<20.0 / 240.0 || h >= 200.0 / 240.0)       num_votes[1]++; // ��
						else if (h >= 20.0 / 240.0 && h<60.0 / 240.0)   num_votes[4]++; // ��
						else if (h >= 60.0 / 240.0 && h<110.0 / 240.0)  num_votes[2]++; // ��
						else if (h >= 110.0 / 240.0 && h<200.0 / 240.0) num_votes[3]++; // ��
						else                             num_votes[0]++; // ���r���[�ȏꍇ�͐F�Ȃ��i�����ɂ͓���Ȃ��͂��j
					}
					else if (s<0.2){
						// �ʓx���������ꍇ�́C�P�x�Ŕ����𔻕�
						if (v>0.6)      num_votes[5]++; // ���邢�ꍇ�͔�
						else if (v<0.4) num_votes[6]++; // �Â��ꍇ�͍�
						else           num_votes[0]++; // ���r���[�ȏꍇ�͐F�Ȃ�
					}
					else{
						// �ʓx�����r���[�ȏꍇ�́C�F�Ȃ��Ɣ���
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
// ��������Dif����()���ɑ}������֐�
// ��1�����F�����̎��
// ��2�����F�f�o�C�X��
// ��3�����F��r���Z�q
// ��4�����F�l
// ------------------------------------------------------------------
bool Is_it_true(char* mode, char* device_name, char* sign, char* value){
	int i;
	if (strcmp(mode, "Distance") == 0){
		// �����Z���T�Ɋւ����r�̏���
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
		// �p�x�Z���T�i�G���R�[�_�j�Ɋւ����r�̏���
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
		// �^�b�`�Z���T�Ɋւ����r�̏���
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
		// �J���[�Z���T�Ɋւ����r�̏���
		if (strcmp(value, get_colorValue(device_name)) == 0) return true;
		else return false;
	}
	printf("Error! No such mode: %s\n", mode);
	wb_robot_cleanup();
	exit(EXIT_FAILURE);
	return false;
}

// ------------------------------------------------------------------
// ����̌p���D�����̏����ɓ��B����܂ŁC������p������D
// ��1�����F�����̎��
// ��2�����F�f�o�C�X��
// ��3�����F��r���Z�q
// ��4�����F�l
// ------------------------------------------------------------------
void Keep_until(char* mode, char* device_name, char* sign, char* value){
	if (strcmp(mode, "Timer") == 0){
		// ���ԂɊւ����r�̏���
		double start_time = wb_robot_get_time();
		double d_time = atof(value);
		// �����𖞑�����܂ŁC�V�~�����[�V������1�X�e�b�v���i�߂�
		while (wb_robot_step(timestep) != -1){
			double current_time = wb_robot_get_time();
			if (current_time - start_time >= d_time) return;
		}
	}
	else if (strcmp(mode, "Distance") == 0){
		// �����Z���T�Ɋւ����r�̏���
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
		// �p�x�Z���T�i�G���R�[�_�j�Ɋւ����r�̏���
		double d_value = atof(value);
		char tmp_device_name[64] = "\0";
		// �wA�̃f�o�C�X�� = "�x�[�X�̃f�o�C�X��"+" finger A"
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
		// �^�b�`�Z���T�Ɋւ����r�̏���
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
		// �J���[�Z���T�Ɋւ����r�̏���
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
// ��_�p�O���[�o���ϐ��̏����������E��_�̐ݒ�
// ��1�����F�����̎��
// ��2�����F�f�o�C�X��
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
// ����̌J��Ԃ��D�����̏����ɓ��B����܂ŁC������p������D
// ��1�����F�����̎��
// ��2�����F�f�o�C�X��
// ��3�����F��r���Z�q
// ��4�����F�l
// ------------------------------------------------------------------
bool repeat_until(char* mode, char* device_name, char* sign, char* value){
	int i;
	// ���̊֐��ɓ��邽�сC�V�~�����[�V������1�X�e�b�v�i�߂�
	int ret = wb_robot_step(timestep);
	if (ret == -1){
		wb_robot_cleanup();
		return EXIT_SUCCESS;
	}
	else{
		if (strcmp(mode, "Infinity") == 0){
			// �����ɌJ��Ԃ�
			return true;
		}
		else if (strcmp(mode, "Timer") == 0){
			// ���ԂɊւ����r�̏���
			double d_time = atof(value);
			double current_time = wb_robot_get_time();
			if (current_time - g_timer >= d_time) return false;
			else return true;
		}
		else if (strcmp(mode, "Counter") == 0){
			// �J��Ԃ��񐔂Ɋւ����r�̏���
			int i_conut = atoi(value);
			if (g_counter >= i_conut) return false;
			else{
				g_counter++;
				return true;
			}
		}
		else if (strcmp(mode, "Distance") == 0){
			// �����Z���T�Ɋւ����r�̏���
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
			// �p�x�Z���T�i�G���R�[�_�j�Ɋւ����r�̏���
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
			// �^�b�`�Z���T�Ɋւ����r�̏���
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
			// �J���[�Z���T�Ɋւ����r�̏���
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
// ����̌J��Ԃ��D�����̏����ɓ��B����܂ŁC������p������D
// �}�N����while���Ƃ��̑O�̏������������ꊇ���ċL�q�ł���悤�ɂ��Ă���D
// ��1�����F�����̎��
// ��2�����F�f�o�C�X��
// ��3�����F��r���Z�q
// ��4�����F�l
// ------------------------------------------------------------------
#define Repeat_until(a, b, c, d) \
	Reset(a, b); \
while (repeat_until(a, b, c, d))
