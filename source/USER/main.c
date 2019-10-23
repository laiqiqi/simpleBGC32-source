
/*
  Sept 2013

  bgc32 Rev -

  Copyright (c) 2013 John Ihlein.  All rights reserved.

  Open Source STM32 Based Brushless Gimbal Controller Software

  Includes code and/or ideas from:

  1)AeroQuad
  2)BaseFlight
  3)CH Robotics
  4)MultiWii
  5)S.O.H. Madgwick
  6)UAVX

  Designed to run on the EvvGC Brushless Gimbal Controller Board

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
 
/*  

instructions:
	
	SimpleBGC ��Դ������ˢ��̨�㷨��ȫ��˵��
	An interpreter��������
	Email��547336083@qq.com
	date:2016/09/19
	
*/
///////////////////////////////////////////////////////////////////////////////

#include "board.h"

///////////////////////////////////////////////////////////////////////////////

eepromConfig_t eepromConfig;

sensors_t      sensors[2];

float          testPhase      = -1.0f * D2R;
float          testPhaseDelta = 10.0f * D2R;

uint16_t       timerValue;

///////////////////////////////////////////////////////////////////////////////

int main(void)
{
    uint32_t currentTime;

    systemInit();

    I2Cx = I2C1;
    initOrientation1();
    
    I2Cx = I2C2;
    initOrientation2();

//    mpu6050Calibration();

    systemReady = true;

    while (1)
    {
        ///////////////////////////////

        if (frame_50Hz)
        {
            frame_50Hz = false;

            currentTime      = micros();
            deltaTime50Hz    = currentTime - previous50HzTime;
            previous50HzTime = currentTime;

            // processPointingCommands();//����ң��������ָ�

            executionTime50Hz = micros() - currentTime;
        }

        ///////////////////////////////

        if (frame_10Hz)
        {
            frame_10Hz = false;

            currentTime      = micros();
            deltaTime10Hz    = currentTime - previous10HzTime;
            previous10HzTime = currentTime;

            /////////////////////////// HJI
            if (newMagData == true)
            {

                // sensors.mag10Hz[XAXIS] =   (float)rawMag[XAXIS].value * magScaleFactor[XAXIS] - eepromConfig.magBias[XAXIS];
                // sensors.mag10Hz[YAXIS] =   (float)rawMag[YAXIS].value * magScaleFactor[YAXIS] - eepromConfig.magBias[YAXIS];
                // sensors.mag10Hz[ZAXIS] = -((float)rawMag[ZAXIS].value * magScaleFactor[ZAXIS] - eepromConfig.magBias[ZAXIS]);
                
                newMagData = false;
                magDataUpdate = true;
            }
            /////////////////////////// HJI

            cliCom();//�������Դ��ڵ��������

            executionTime10Hz = micros() - currentTime;
        }

        ///////////////////////////////

        if (frame_500Hz)
        { //��if��֧��������̨�㷨����
#ifdef _DTIMING
            LA2_ENABLE;
#endif
            frame_500Hz = false;

            currentTime       = micros();//��ȡ��ǰʱ��
            deltaTime500Hz    = currentTime - previous500HzTime;//�õ�ʱ������
            previous500HzTime = currentTime;//���浱ǰʱ��

            TIM_Cmd(TIM6, DISABLE);//�ر�TIM6
            timerValue = TIM_GetCounter(TIM6);//�õ�TIM6����ֵ ����λ 0.5 uSec Tick
            TIM_SetCounter(TIM6, 0);//��� TIMx->CNT �Ĵ���
            TIM_Cmd(TIM6, ENABLE);//����TIM6 ��׼����ȡ��һ�ε�ʱ������

						//��ʱ������ֵ*0.5us,�õ�ʱ����������λ�� us 
            dt500Hz = (float)timerValue * 0.0000005f;  

						//(1/8192) * 9.8065  (8192 LSB = 1 G)
					  //���������������ĵ�ǰ���ٶ�����-�¶Ȳ���ƫ�* ��С�ֱ��� ����С�ֱ��ʾ��ǣ�(1/8192) * 9.8065�� 
					 //1G���̵�8192����������֮1����Ӧ�������ٶ�9.8065m/1G��8192��֮1
					//accelData500Hz��������Systick�ж����汻���µģ����µ�ֵ�Ǿ�������������ֵ������Ƶ����500hz
            sensors[0].accel500Hz[XAXIS] =  ((float)accelData500Hz[0][XAXIS] - accelTCBias[XAXIS]) * ACCEL_SCALE_FACTOR;
            sensors[0].accel500Hz[YAXIS] =  ((float)accelData500Hz[0][YAXIS] - accelTCBias[YAXIS]) * ACCEL_SCALE_FACTOR;
            sensors[0].accel500Hz[ZAXIS] = -((float)accelData500Hz[0][ZAXIS] - accelTCBias[ZAXIS]) * ACCEL_SCALE_FACTOR;

            sensors[1].accel500Hz[XAXIS] =  ((float)accelData500Hz[1][XAXIS] - accelTCBias[XAXIS]) * ACCEL_SCALE_FACTOR;
            sensors[1].accel500Hz[YAXIS] =  ((float)accelData500Hz[1][YAXIS] - accelTCBias[YAXIS]) * ACCEL_SCALE_FACTOR;
            sensors[1].accel500Hz[ZAXIS] = -((float)accelData500Hz[1][ZAXIS] - accelTCBias[ZAXIS]) * ACCEL_SCALE_FACTOR;

            // cliPrintF("\r\n accel_ROLL[0] %d ", (int)(sensors[0].accel500Hz[ROLL]*100));
            // cliPrintF(" accel_PITCH[0] %d ", (int)(sensors[0].accel500Hz[PITCH]*100));
            // cliPrintF(" accel_YAW[0] %d ", (int)(sensors[0].accel500Hz[YAW]*100));

            // cliPrintF("\r\n accel_ROLL[1] %d ", (int)(sensors[1].accel500Hz[ROLL]*100));
            // cliPrintF(" accel_PITCH[1] %d ", (int)(sensors[1].accel500Hz[PITCH]*100));
            // cliPrintF(" accel_YAW[1] %d \r\n", (int)(sensors[1].accel500Hz[YAW]*100));
            
            // cliPrintF("\r\n accel500Hz[XAXIS] %d ", (int)(sensors[0].accel500Hz[XAXIS]*100));
            // cliPrintF(" accel500Hz[YAXIS] %d ", (int)(sensors[0].accel500Hz[YAXIS]*100));
            // cliPrintF(" accel500Hz[ZAXIS] %d \r\n", (int)(sensors[0].accel500Hz[ZAXIS]*100));

						// (1/65.5) * pi/180   (65.5 LSB = 1 DPS)
						//���������������ĵ�ǰ����������-�����Ǿ�ֹ״̬�»���5000�ε�ƽ��ֵ-�¶Ȳ���ƫ�* ��С�ֱ��� 
						//��С�ֱ��ʾ��� (1/65.5) * pi/180  
						//1DPS����������ֵ��65.5, 65.5��֮1 ���� PI Ȼ�����180 ������С�ֱ���
						//gyroData500Hz��������Systick�ж����汻���µģ����µ�ֵ�Ǿ�������������ֵ������Ƶ����500hz

            sensors[0].gyro500Hz[ROLL ] =  ((float)gyroData500Hz[0][ROLL ] - gyroRTBias[0][ROLL ] - gyroTCBias[0][ROLL ]) * GYRO_SCALE_FACTOR;
            sensors[0].gyro500Hz[PITCH] =  ((float)gyroData500Hz[0][PITCH] - gyroRTBias[0][PITCH] - gyroTCBias[0][PITCH]) * GYRO_SCALE_FACTOR;
            sensors[0].gyro500Hz[YAW  ] = -((float)gyroData500Hz[0][YAW  ] - gyroRTBias[0][YAW  ] - gyroTCBias[0][YAW  ]) * GYRO_SCALE_FACTOR;

            sensors[1].gyro500Hz[ROLL ] =  ((float)gyroData500Hz[1][ROLL ] - gyroRTBias[1][ROLL ] - gyroTCBias[1][ROLL ]) * GYRO_SCALE_FACTOR;
            sensors[1].gyro500Hz[PITCH] =  ((float)gyroData500Hz[1][PITCH] - gyroRTBias[1][PITCH] - gyroTCBias[1][PITCH]) * GYRO_SCALE_FACTOR;
            sensors[1].gyro500Hz[YAW  ] = -((float)gyroData500Hz[1][YAW  ] - gyroRTBias[1][YAW  ] - gyroTCBias[1][YAW  ]) * GYRO_SCALE_FACTOR;

            // cliPrintF("\r\n ROLL[0] %d ", (int)(sensors[0].gyro500Hz[ROLL]*100));
            // cliPrintF(" PITCH[0] %d ", (int)(sensors[0].gyro500Hz[PITCH]*100));
            // cliPrintF(" YAW[0] %d ", (int)(sensors[0].gyro500Hz[YAW]*100));

            // cliPrintF("\r\n ROLL[1] %d ", (int)(sensors[1].gyro500Hz[ROLL]*100));
            // cliPrintF(" PITCH[1] %d ", (int)(sensors[1].gyro500Hz[PITCH]*100));
            // cliPrintF(" YAW[1] %d \r\n", (int)(sensors[1].gyro500Hz[YAW]*100));

            // cliPrintF("\r\n gyro500Hz-ROLL %d ", (int)(sensors.gyro500Hz[ROLL]*100));
            // cliPrintF(" gyro500Hz-PITCH %d ", (int)(sensors.gyro500Hz[PITCH]*100));
            // cliPrintF(" gyro500Hz-YAW %d \r\n", (int)(sensors.gyro500Hz[YAW]*100));

						//��ǰ��λ�������㣬���� accAngleSmooth ��ͨ�����ٶ����ݾ���atan2f�������������ŷ���ǣ����ҽ�����һ���ͺ��˲�
						//�����˲��㷨Ҳ���ڵ�ͨ�˲���һ�֣����ŵ㣺 �������Ը��ž������õ��������� �����ڲ���Ƶ�ʽϸߵĳ���,
						// ȱ�㣺 ��λ�ͺ������ȵ� �ͺ�̶�ȡ����aֵ��С ���������˲�Ƶ�ʸ��ڲ���Ƶ�ʵ�1/2�ĸ����ź�,
						//getOrientation�����ڲ�ʹ����accAngleSmoothŷ���������������ݽ����˻����˲��ں��㷨�õ��ȶ���ŷ���ǣ�����
						//��ŵ�sensors.evvgcCFAttitude500Hz���棬sensors.accel500Hz��sensors.gyro500Hz�Ǿ��������㷨�����ļ��ٶ�����
						//�����������ݣ�dt500Hz��ʱ��������Ҳ����ִ��if (frame_500Hz){} ����Ĵ�����
            getOrientation(accAngleSmooth[0], sensors[0].evvgcCFAttitude500Hz, sensors[0].accel500Hz, sensors[0].gyro500Hz, dt500Hz);

            getOrientation(accAngleSmooth[1], sensors[1].evvgcCFAttitude500Hz, sensors[1].accel500Hz, sensors[1].gyro500Hz, dt500Hz);

            // cliPrintF("\r\n evvgcCF-ROLL[0] %d ", (int)(sensors[0].evvgcCFAttitude500Hz[ROLL]*100));
            // cliPrintF(" evvgcCF-PITCH[0] %d ", (int)(sensors[0].evvgcCFAttitude500Hz[PITCH]*100));
            // cliPrintF(" evvgcCF-YAW[0] %d \r\n", (int)(sensors[0].evvgcCFAttitude500Hz[YAW]*100));

            // cliPrintF("\r\n evvgcCF-ROLL[1] %d ", (int)(sensors[1].evvgcCFAttitude500Hz[ROLL]*100));
            // cliPrintF(" evvgcCF-PITCH[1] %d ", (int)(sensors[1].evvgcCFAttitude500Hz[PITCH]*100));
            // cliPrintF(" evvgcCF-YAW[1] %d \r\n", (int)(sensors[1].evvgcCFAttitude500Hz[YAW]*100));

						//�Լ��ٶ����ݽ��� һ�׵�ͨ�˲� ������sensors.accel500Hz�Ǵ������˲���ֵ��firstOrderFilters���˲�������
						// Low Pass:
						// GX1 = 1 / (1 + A)
						// GX2 = 1 / (1 + A)
						// GX3 = (1 - A) / (1 + A)
            sensors[0].accel500Hz[ROLL ] = firstOrderFilter(sensors[0].accel500Hz[ROLL ], &firstOrderFilters[0][ACCEL_X_500HZ_LOWPASS ]);
            sensors[0].accel500Hz[PITCH] = firstOrderFilter(sensors[0].accel500Hz[PITCH], &firstOrderFilters[0][ACCEL_Y_500HZ_LOWPASS]);
            sensors[0].accel500Hz[YAW  ] = firstOrderFilter(sensors[0].accel500Hz[YAW  ], &firstOrderFilters[0][ACCEL_Z_500HZ_LOWPASS  ]);

            sensors[1].accel500Hz[ROLL ] = firstOrderFilter(sensors[1].accel500Hz[ROLL ], &firstOrderFilters[1][ACCEL_X_500HZ_LOWPASS ]);
            sensors[1].accel500Hz[PITCH] = firstOrderFilter(sensors[1].accel500Hz[PITCH], &firstOrderFilters[1][ACCEL_Y_500HZ_LOWPASS]);
            sensors[1].accel500Hz[YAW  ] = firstOrderFilter(sensors[1].accel500Hz[YAW  ], &firstOrderFilters[1][ACCEL_Z_500HZ_LOWPASS  ]);

            // cliPrintF("\r\n accel500Hz_ROLL[0] %d ", (int)(sensors[0].accel500Hz[ROLL]*100));
            // cliPrintF(" accel500Hz_PITCH[0] %d ", (int)(sensors[0].accel500Hz[PITCH]*100));
            // cliPrintF(" accel500Hz_YAW[0] %d ", (int)(sensors[0].accel500Hz[YAW]*100));

            // cliPrintF("\r\n accel500Hz_ROLL[1] %d ", (int)(sensors[1].accel500Hz[ROLL]*100));
            // cliPrintF(" accel500Hz_PITCH[1] %d ", (int)(sensors[1].accel500Hz[PITCH]*100));
            // cliPrintF(" accel500Hz_YAW[1] %d \r\n", (int)(sensors[1].accel500Hz[YAW]*100));
            
						//���˲ο�ϵͳ���£���ڲ������������������ݣ�������ٶ����ݣ��������������,�Լ�ָʾ�Ƿ���´��������ݵ�magDataUpdate������
						//magDataUpdate=false��ʾ�����´��������ݣ�magDataUpdate=true��ʾ���´��������ݣ����һ��������ʱ������Dt��Ҳ���Ǵ˺�����
						//��ִ�����ϴ�ִ�е�ʱ������
            MargAHRSupdate1(sensors[0].gyro500Hz[ROLL],   sensors[0].gyro500Hz[PITCH],  sensors[0].gyro500Hz[YAW],
                           sensors[0].accel500Hz[XAXIS], sensors[0].accel500Hz[YAXIS], sensors[0].accel500Hz[ZAXIS],
                           sensors[0].mag10Hz[XAXIS],    sensors[0].mag10Hz[YAXIS],    sensors[0].mag10Hz[ZAXIS],
                           magDataUpdate , dt500Hz);

            MargAHRSupdate2(sensors[1].gyro500Hz[ROLL],   sensors[1].gyro500Hz[PITCH],  sensors[1].gyro500Hz[YAW],
                           sensors[1].accel500Hz[XAXIS], sensors[1].accel500Hz[YAXIS], sensors[1].accel500Hz[ZAXIS],
                           sensors[1].mag10Hz[XAXIS],    sensors[1].mag10Hz[YAXIS],    sensors[1].mag10Hz[ZAXIS],
                           magDataUpdate , dt500Hz);

            magDataUpdate = false;//Ĭ�ϲ����´���������

            // computeMotorCommands(dt500Hz);//����������������ڲ���ʱ������Dt

            executionTime500Hz = micros() - currentTime;//��������ִ��ʱ�䳤�ȱ��浽executionTime500Hz

            LED1_OFF;
#ifdef _DTIMING
            LA2_DISABLE;
#endif
        }

        ///////////////////////////////

        if (frame_100Hz)
        {
            frame_100Hz = false;

            currentTime       = micros();
            deltaTime100Hz    = currentTime - previous100HzTime;
            previous100HzTime = currentTime;

            executionTime100Hz = micros() - currentTime;
        }

        ///////////////////////////////

        if (frame_5Hz)
        {
            frame_5Hz = false;

            currentTime     = micros();
            deltaTime5Hz    = currentTime - previous5HzTime;
            previous5HzTime = currentTime;

          //  LED2_TOGGLE;

            executionTime5Hz = micros() - currentTime;
        }

        ///////////////////////////////

        if (frame_1Hz)
        {
            frame_1Hz = false;

            currentTime     = micros();
            deltaTime1Hz    = currentTime - previous1HzTime;
            previous1HzTime = currentTime;

          //  LED1_TOGGLE;

            executionTime1Hz = micros() - currentTime;
        }

        ////////////////////////////////
    }
}

///////////////////////////////////////////////////////////////////////////////

