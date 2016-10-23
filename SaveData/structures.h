#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#define NAMESIZE 32
#define MODULE_LIST_SIZE 5

#pragma pack(1)
typedef struct{
	unsigned char address;
	unsigned char registered;
	unsigned char hwType;
	unsigned char moduleType;
	unsigned short firmwareVersion;
	unsigned short firmwareDate;
} header_t;

typedef struct{
    uint8 wasFlushed;
    uint8 sprayerIsOn;
    uint16 bowlLevel;
    uint16 urineLevel;
} bowlData_t;

typedef struct{

} bowlSettings_t;

typedef struct{
	uint8 filename[28];
	uint8 key[2];
} result_t;

typedef struct{
	int32 duration;
	uint8 resolution;
	uint8 option;
	uint8 key;
} spectrometerScan_t;

typedef struct{
	uint16 lightsource;
	uint16 probe;
	uint8 spectroID[10];
	uint8 machineID[10];
} spectrometerSettings_t;

typedef struct{
	uint8 deviceStatus;
	uint8 devicePower;
	uint8 lastBackgroundTime[12];
} spectrometerData_t;

typedef struct{
	header_t header;
	spectrometerData_t data;
	spectrometerSettings_t settings;
} spectrometer_t;


typedef struct{
    header_t header;
    bowlData_t data;
    bowlSettings_t settings;
} bowl_t;

typedef struct{
} guiData_t;

typedef struct{
	header_t header;
	guiData_t data;
} gui_t;

typedef struct{
	unsigned short idle_timeout;
	unsigned short warmup_value;
	unsigned short normal_value;
	unsigned short active_timeout;
	unsigned short average_ADC_times;
	unsigned short high_VOC_detect;
	unsigned short sense_duration;
} moxSettings_t;

typedef struct{
	//	format not yet defined
} moxData_t;

typedef struct{
	moxData_t data;
	moxSettings_t settings;
} mox_t;

typedef struct{
	unsigned char serial_number_0;
	unsigned char serial_number_1;
	unsigned char serial_number_2;
	unsigned char serial_number_3;
	unsigned char chip_ID;
} flowMisc_t;

typedef struct{
	unsigned char Ch1_sensitivity;
	unsigned char Ch1_timeout;
	unsigned char Ch1_setup;
	unsigned char Ch2_sensitivity;
	unsigned char Ch2_timeout;
	unsigned char Ch2_setup;
	unsigned char Configuration;
	unsigned char Ch1_CAPDAC;
	unsigned char Ch2_CAPDAC;
} flowSettings_t;

typedef struct{
	unsigned char Ch1_data_high;
	unsigned char Ch1_data_low;
	unsigned char Ch2_data_high;
	unsigned char Ch2_data_low;
	unsigned char Ch1_average_high;
	unsigned char Ch1_average_low;
	unsigned char Ch2_average_high;
	unsigned char Ch2_average_low;
} flowData_t;

typedef struct{
	flowData_t data;
	flowSettings_t settings;
	flowMisc_t misc;
} flow_t;

typedef struct{
	unsigned char fanStatus;
	unsigned char fanSpeed[2];
	unsigned char valveStatus;
} exhaustData_t;

typedef struct{
	exhaustData_t data;
} exhaust_t;

typedef struct{
	flow_t flow;
	exhaust_t exhaust;
	mox_t mox;
} superModuleData_t;

typedef struct{
	header_t header;
	superModuleData_t data;
} superModule_t;

typedef struct{
    float specificGravity;
    float currentSlotTemperature;
    float setPointTemperature;
    uint8 pwm;
    uint8 light;
    uint8 heater;
} refractometerData_t;

typedef struct{

} refractometerSettings_t;

typedef struct{
	header_t header;
    refractometerData_t data;
    refractometerSettings_t settings;
} refractometer_t;


typedef struct{
	uint8 fan;
	uint8 valve;
} ppgData_t;

typedef struct{
	header_t header;
	ppgData_t data;
} ppg_t;


typedef struct{
	unsigned char one;
	unsigned char two;
	unsigned char three;
	unsigned char four;
	unsigned char five;
	unsigned char six;
	unsigned char seven;
	unsigned char eight;
	unsigned char nine;
} miscData_t;

typedef struct{
	header_t header;
	miscData_t data;
} misc_t;


typedef struct{
	int wandPosition;
	unsigned char extend;
	unsigned char retract;
	unsigned char flush;
} bidetData_t;

typedef struct {
	header_t header;
	bidetData_t data;
} bidet_t;

typedef struct{
	superModule_t *superModule;
	gui_t *gui;
	refractometer_t *refractometer;
	bowl_t *bowl;
	spectrometer_t *spectrometer;
//	exhaust_t *exhaust;
//	bidet_t *bidet;
//	misc_t *misc;
} modules_t;

typedef struct{
//	exhaustData_t exhaustData;
//	miscData_t miscData;
	superModuleData_t superModuleData;
	refractometerData_t refractometerData;
	bowlData_t bowlData;
	spectrometerData_t spectrometerData;
} toiletData_t;

typedef struct{
	modules_t modules;
	toiletData_t data;
} toilet_t;

#pragma pack(0)

#endif // STRUCTURES_H_INCLUDED
