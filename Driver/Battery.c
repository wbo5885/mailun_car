#include "Battery.h"
#include "GPIO.h"
#include "ADC.h"
#include "NVIC.h"

// ��ص�ѹ��ֵ���壨��λ�����أ�
#define BATTERY_FULL_THRESHOLD      4.0f
#define BATTERY_MEDIUM_THRESHOLD    3.7f
#define BATTERY_LOW_THRESHOLD       3.5f
#define BATTERY_CRITICAL_THRESHOLD  3.3f

// ��������ѹ����͵�ѹ
#define BATTERY_MAX_VOLTAGE         4.2f
#define BATTERY_MIN_VOLTAGE         3.2f

static void GPIO_config(void) {
    GPIO_InitTypeDef	GPIO_InitStructure;		//�ṹ����
    GPIO_InitStructure.Pin  = GPIO_Pin_3;		//ָ��Ҫ��ʼ����IO,
    GPIO_InitStructure.Mode = GPIO_HighZ;	//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P1, &GPIO_InitStructure);//��ʼ��
}
/******************* AD���ú��� *******************/
void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;		//�ṹ����

	ADC_InitStructure.ADC_SMPduty   = 31;		//ADC ģ���źŲ���ʱ�����, 0~31��ע�⣺ SMPDUTY һ����������С�� 10��
	ADC_InitStructure.ADC_CsSetup   = 0;		//ADC ͨ��ѡ��ʱ����� 0(Ĭ��),1
	ADC_InitStructure.ADC_CsHold    = 1;		//ADC ͨ��ѡ�񱣳�ʱ����� 0,1(Ĭ��),2,3
	ADC_InitStructure.ADC_Speed     = ADC_SPEED_2X1T;		//���� ADC ����ʱ��Ƶ��	ADC_SPEED_2X1T~ADC_SPEED_2X16T
	ADC_InitStructure.ADC_AdjResult = ADC_RIGHT_JUSTIFIED;	//ADC�������,	ADC_LEFT_JUSTIFIED,ADC_RIGHT_JUSTIFIED
	ADC_Inilize(&ADC_InitStructure);				//��ʼ��
	ADC_PowerControl(ENABLE);								//ADC��Դ����, ENABLE��DISABLE
	NVIC_ADC_Init(DISABLE,Priority_0);			//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}
void Battery_init(){
	GPIO_config();
	ADC_config();
}

float Battery_get_voltage(){
	u16 result = Get_ADCResult(ADC_CH3);// chn: ADC_CH0, ADC_CH1, ADC_CH2 .... ADC_CH15

	// result / 4096 = vol / 2.5
	float Vadc = result * 2.5 / 4096;
	
	// Vcc / (51k�� + 10k��) = Vadc / 10k��
	return (Vadc * 6.1) * 1.005; 
	
}

BatteryStatus_t Battery_get_status(void) {
    float voltage = Battery_get_voltage();
    
    if (voltage >= BATTERY_FULL_THRESHOLD) {
        return BATTERY_FULL;
    } else if (voltage >= BATTERY_MEDIUM_THRESHOLD) {
        return BATTERY_MEDIUM;
    } else if (voltage >= BATTERY_LOW_THRESHOLD) {
        return BATTERY_LOW;
    } else {
        return BATTERY_CRITICAL;
    }
}

uint8_t Battery_get_percentage(void) {
    float voltage = Battery_get_voltage();
    float percentage;
    
    // ��������ٷֱ�
    percentage = (voltage - BATTERY_MIN_VOLTAGE) / 
                (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) * 100.0f;
    
    // ���ưٷֱȷ�Χ��0-100֮��
    if (percentage > 100.0f) {
        percentage = 100.0f;
    } else if (percentage < 0.0f) {
        percentage = 0.0f;
    }
    
    return (uint8_t)percentage;
}

bool Battery_is_low(void) {
    return Battery_get_voltage() <= BATTERY_LOW_THRESHOLD;
}