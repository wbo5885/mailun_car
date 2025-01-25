#include "Battery.h"
#include "GPIO.h"
#include "ADC.h"
#include "NVIC.h"

// 电池电压阈值定义（单位：伏特）
#define BATTERY_FULL_THRESHOLD      4.0f
#define BATTERY_MEDIUM_THRESHOLD    3.7f
#define BATTERY_LOW_THRESHOLD       3.5f
#define BATTERY_CRITICAL_THRESHOLD  3.3f

// 电池满电电压和最低电压
#define BATTERY_MAX_VOLTAGE         4.2f
#define BATTERY_MIN_VOLTAGE         3.2f

static void GPIO_config(void) {
    GPIO_InitTypeDef	GPIO_InitStructure;		//结构定义
    GPIO_InitStructure.Pin  = GPIO_Pin_3;		//指定要初始化的IO,
    GPIO_InitStructure.Mode = GPIO_HighZ;	//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P1, &GPIO_InitStructure);//初始化
}
/******************* AD配置函数 *******************/
void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;		//结构定义

	ADC_InitStructure.ADC_SMPduty   = 31;		//ADC 模拟信号采样时间控制, 0~31（注意： SMPDUTY 一定不能设置小于 10）
	ADC_InitStructure.ADC_CsSetup   = 0;		//ADC 通道选择时间控制 0(默认),1
	ADC_InitStructure.ADC_CsHold    = 1;		//ADC 通道选择保持时间控制 0,1(默认),2,3
	ADC_InitStructure.ADC_Speed     = ADC_SPEED_2X1T;		//设置 ADC 工作时钟频率	ADC_SPEED_2X1T~ADC_SPEED_2X16T
	ADC_InitStructure.ADC_AdjResult = ADC_RIGHT_JUSTIFIED;	//ADC结果调整,	ADC_LEFT_JUSTIFIED,ADC_RIGHT_JUSTIFIED
	ADC_Inilize(&ADC_InitStructure);				//初始化
	ADC_PowerControl(ENABLE);								//ADC电源开关, ENABLE或DISABLE
	NVIC_ADC_Init(DISABLE,Priority_0);			//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}
void Battery_init(){
	GPIO_config();
	ADC_config();
}

float Battery_get_voltage(){
	u16 result = Get_ADCResult(ADC_CH3);// chn: ADC_CH0, ADC_CH1, ADC_CH2 .... ADC_CH15

	// result / 4096 = vol / 2.5
	float Vadc = result * 2.5 / 4096;
	
	// Vcc / (51kΩ + 10kΩ) = Vadc / 10kΩ
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
    
    // 计算电量百分比
    percentage = (voltage - BATTERY_MIN_VOLTAGE) / 
                (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) * 100.0f;
    
    // 限制百分比范围在0-100之间
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