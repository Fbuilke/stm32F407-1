#include "Kalman.h"

#define Kp 2.0f                	// proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.00f                	// 0.001  integral gain governs rate of convergence of gyroscope biases
#define T 0.02f

#define IMU_INTEGRAL_LIM  ( 2.0f *DEG_TO_RAD )
//����У׼���������
float ax_cali=0,ay_cali=0,az_cali=0;
short gx_cali=0,gy_cali=0,gz_cali=0;

float sigma_a=0.01,sigma_g=0.1;//������������������ͼ��ٶȼƹ۲���������
//����������
float K_roll[2];
float K_pitch[2];
//��С����������,M[n|n]��M[n-1|n-1]
float mmse_roll[2][2] = { { 1, 0 },{ 0, 1 } };
float mmse_pitch[2][2] = { { 1, 0 },{ 0, 1 } };
//��СԤ�����������,M[n|n-1]
float mPmse_roll[2][2];
float mPmse_pitch[2][2];
//�����ںϿ������˲��㷨
void IMUupdate(short *gx,short *gy,short *gz,short ax,short ay,short az,float *roll,float *pitch,float *yaw)
{
	float temp;//Ϊ���ټ���������ʱ�������ݣ���ʵ������
	float roll_temp,pitch_temp;//״̬����Ԥ��ֵ,s[n|n-1]
	//Ԥ��
	*gx-=gx_cali;
	*gy-=gz_cali;
	*gz-=gz_cali;
	*yaw+=GYRO_TO_DEG(*gz)*T;
	roll_temp=*roll+GYRO_TO_DEG(*gx)*T;
	pitch_temp=*pitch+GYRO_TO_DEG(*gy)*T;
	//��СԤ��MSE
	mPmse_roll[0][0]=mmse_roll[0][0]+(mmse_roll[1][1]+sigma_g)*T*T-(mmse_roll[0][1]+mmse_roll[1][0])*T;
	mPmse_roll[0][1]=mmse_roll[0][1]-mmse_roll[1][1]*T;
	mPmse_roll[1][0]=mmse_roll[1][0]-mmse_roll[1][1]*T;
	mPmse_roll[1][1]=mmse_roll[1][1];
	mPmse_pitch[0][0]=mmse_pitch[0][0]+(mmse_pitch[1][1]+sigma_g)*T*T-(mmse_pitch[0][1]+mmse_pitch[1][0])*T;
	mPmse_pitch[0][1]=mmse_pitch[0][1]-mmse_pitch[1][1]*T;
	mPmse_pitch[1][0]=mmse_pitch[1][0]-mmse_pitch[1][1]*T;
	mPmse_pitch[1][1]=mmse_pitch[1][1];
	//����������
	K_roll[0]=mPmse_roll[0][0]/(mPmse_roll[0][0]+sigma_a);
	K_roll[1]=mPmse_roll[1][0]/(mPmse_roll[0][0]+sigma_a);
	K_pitch[0]=mPmse_pitch[0][0]/(mPmse_pitch[0][0]+sigma_a);
	K_pitch[1]=mPmse_pitch[1][0]/(mPmse_pitch[0][0]+sigma_a);
	//����
	temp=atan2(ay,az)*180/PI-roll_temp;
	*roll=roll_temp+K_roll[0]*temp;
	gx_cali=gx_cali+K_roll[1]*temp;
	temp=atan2(-ax,az)*180/PI-pitch_temp;
	*pitch=pitch_temp+K_pitch[0]*temp;
	gy_cali=gy_cali+K_pitch[1]*temp;
	//��СMSE
	mmse_roll[0][0]=mPmse_roll[0][0]-K_roll[0]*mPmse_roll[0][0];
	mmse_roll[0][1]=mPmse_roll[0][1]-K_roll[0]*mPmse_roll[0][1];
	mmse_roll[1][0]=mPmse_roll[1][0]-K_roll[1]*mPmse_roll[0][0];
	mmse_roll[1][1]=mPmse_roll[1][1]-K_roll[1]*mPmse_roll[0][1];
	mmse_pitch[0][0]=mPmse_pitch[0][0]-K_pitch[0]*mPmse_pitch[0][0];
	mmse_pitch[0][1]=mPmse_pitch[0][1]-K_pitch[0]*mPmse_pitch[0][1];
	mmse_pitch[1][0]=mPmse_pitch[1][0]-K_pitch[1]*mPmse_pitch[0][0];
	mmse_pitch[1][1]=mPmse_pitch[1][1]-K_pitch[1]*mPmse_pitch[0][1];
}