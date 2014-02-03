/* linux/include/linux/exynos_thermal.h
  *
  * Copyright (c) 2010-2011 Samsung Electronics Co., Ltd.
  *		http://www.samsung.com
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License version 2 as
  * published by the Free Software Foundation.
  */

#include <linux/platform_data/exynos4_tmu.h>

#ifndef THERMAL_INTERFACE_H
#define THERMAL_INTERFACE_H
/* CPU Zone information */

#define SENSOR_NAME_LEN	16
#define MAX_TRIP_COUNT	8

#define PANIC_ZONE      4
#define WARN_ZONE       3
#define MONITOR_ZONE    2
#define SAFE_ZONE       1
#define NO_ACTION       0

#define FREQ_IN_PLL                     24000000  /* 24MHZ in Hz */
#define AUTO_REFRESH_PERIOD_TQ0         1950
#define AUTO_REFRESH_PERIOD_NORMAL      3900
#define TIMING_AREF_OFFSET              0x30

struct	thermal_trip_point_conf {
	int trip_val[MAX_TRIP_COUNT];
	int trip_count;
};

struct	thermal_cooling_conf {
	struct freq_pctg_table freq_data[MAX_TRIP_COUNT];
	int freq_pctg_count;
};

/**
  * struct exynos4_tmu_platform_data
  * @name: name of the temperature sensor
  * @read_temperature: A function pointer to read temperature info
  * @private_data: Temperature sensor private data
  * @sensor_data: Sensor specific information like trigger temperature, level
  */
struct thermal_sensor_conf {
	char	name[SENSOR_NAME_LEN];
	int	(*read_temperature)(void *data);
	struct	thermal_trip_point_conf trip_data;
	struct	thermal_cooling_conf cooling_data;
	struct	tmu_tc tc_data;
	void	*private_data;
};

/**
  * exynos4_register_thermal: Register to the exynos thermal interface.
  * @sensor_conf:   Structure containing temperature sensor information
  *
  * returns zero on success, else negative errno.
  */
int exynos4_register_thermal(struct thermal_sensor_conf *sensor_conf);

/**
  * exynos4_unregister_thermal: Un-register from the exynos thermal interface.
  *
  * return not applicable.
  */
void exynos4_unregister_thermal(void);

/**
  * exynos4_report_trigger: Report any trigger level crossed in the
  *	temperature sensor. This may be useful to take any cooling action.
  *
  * return not applicable.
  */
extern void exynos4_report_trigger(void);
#endif
