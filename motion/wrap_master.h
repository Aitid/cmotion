#ifndef _WRAPMASTER_H
#define _WRAPMASTER_H

#include "ethercat.h"

typedef struct PACKED{
    uint16   status_word;                 // 0x6041
    int32    position_actual_value;       // 0x6064
    uint32   velocity_actual_value;       // 0x606c
    uint16   fault_code;                  // 0x603F
    int8     operation_mode_display;      // 0x6061
    int32    target_position;             // 0x607A
} inPDO;


typedef struct PACKED{
    int32   p_velocity;           /* 0x6081 */ 
    int32   p_acceleration;       /* 0x6083 */
    int32   p_deceleration;       /* 0x6084 */
    int32   p_target_position;    /* 0x607a */
    int32   t_velocity;           /* 0x60FF */
    uint16  cntrl_word;           /* 0x6040 */
    int8    operation_mode;       /* 0x6060 */
} outPDO;


typedef enum {
    MAPPING_PARAMETER_1600 = 0x1600,
    MAPPING_PARAMETER_1601 = 0x1601,
    MAPPING_PARAMETER_1602 = 0x1602
} RxPDO;


typedef enum {
    RxPDO_ASSIGN = 0x1C12,
    TxPDO_ASSIGN = 0x1C13
} PDO_assign;


typedef enum {
    MAPPING_PARAMETER_1A00 = 0x1A00,
    MAPPING_PARAMETER_1A01 = 0x1A01,
    MAPPING_PARAMETER_1A02 = 0x1A02
} TxPDO;


typedef enum {
    TRACK_SPEED             = 0x6081,
    TRACK_ACCELERATION      = 0x6083,
    TRACK_DECELERATION      = 0x6084,
    TARGET_LOCATION         = 0x607a,
    TARGET_VELOCITY         = 0x60ff,
    CONTROL_WORD            = 0x6040,
    OPERATION_MODE          = 0x6060,
    STATUS_WORD             = 0x6041,
    ACTUAL_POSITION         = 0x6064,
    ACTUAL_SPEED            = 0x606c,
    FAULT_CODE              = 0x603f,
    OPERATION_MODE_DISPLAY  = 0x6061,
    PROFILE_TARGET_POSITION = 0x607a
} CIA402_Object_dictionary;


typedef enum {
    TRACK_SPEED_ADDR             = 0x60810020,
    TRACK_ACCELERATION_ADDR      = 0x60830020,
    TRACK_DECELERATION_ADDR      = 0x60840020,
    TARGET_LOCATION_ADDR         = 0x607a0020,
    TARGET_VELOCITY_ADDR         = 0x60ff0020,
    CONTROL_WORD_ADDR            = 0x60400010,
    OPERATION_MODE_ADDR          = 0x60600008,
    STATUS_WORD_ADDR             = 0x60410010,
    ACTUAL_POSITION_ADDR         = 0x60640020,
    ACTUAL_SPEED_ADDR            = 0x606c0020,
    FAULT_CODE_ADDR              = 0x603f0010,
    OPERATION_MODE_DISPLAY_ADDR  = 0x60610008,
    PROFILE_TARGET_POSITION_ADDR = 0x607a0020
} CIA402_Object_address;

typedef enum {
    SHUTDOWN = 0b00110,
    Switch_On__Enable_operation = 0b1111,
} control_word_bit;


typedef enum {
    PROFILE_POSITION            = 1,
    PROFILE_VELOCITY            = 3,
    HOMING                      = 6,
    CYCLIC_SYNCHRONOUS_POSITION = 8
} operation_mode_value;


int slave_config(uint16 slave);
void setup(char *ifname);
int enable_csp_mode();
void complete();


#endif /* _WRAPMASTER_H */
