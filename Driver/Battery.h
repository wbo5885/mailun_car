#ifndef __BATTERY_H__
#define __BATTERY_H__

#include "Config.h"

// 电池状态枚举
typedef enum {
    BATTERY_FULL,      // 电池电量充足
    BATTERY_MEDIUM,    // 电池电量中等
    BATTERY_LOW,       // 电池电量低
    BATTERY_CRITICAL   // 电池电量严重不足
} BatteryStatus_t;

// 初始化电池监测
void Battery_init(void);

// 获取电池电压
float Battery_get_voltage(void);

// 获取电池状态
BatteryStatus_t Battery_get_status(void);

// 获取电池电量百分比
uint8_t Battery_get_percentage(void);

// 检查电池是否处于低电量状态
bool Battery_is_low(void);

#endif