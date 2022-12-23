#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "wrap_master.h"
#include "ethercat.h"


char IOmap[4096];
int expectedWKC;


int slave_config(uint16 slave){
    printf("Slave is %d\n", slave);
    int8 zero = 0;
    int8 total_1600 = 7;
    CIA402_Object_address map_1600[7] = {
        TRACK_SPEED_ADDR,
        TRACK_ACCELERATION_ADDR,
        TRACK_DECELERATION_ADDR,
        TARGET_LOCATION_ADDR,   
        TARGET_VELOCITY_ADDR,
        CONTROL_WORD_ADDR,
        OPERATION_MODE_ADDR
    };
    int8 total_1a00 = 6;
    CIA402_Object_address map_1a00[6] = {
        STATUS_WORD_ADDR,
        ACTUAL_POSITION_ADDR,
        ACTUAL_SPEED_ADDR,           
        FAULT_CODE_ADDR,             
        OPERATION_MODE_DISPLAY_ADDR, 
        PROFILE_TARGET_POSITION_ADDR
    };
    int8 total_1c12 = 1;
    uint32 map_1c12[1] = {MAPPING_PARAMETER_1600};

    int8 total_1c13 = 1;
    TxPDO map_1c13[1] = {MAPPING_PARAMETER_1A00};

    ec_SDOwrite(slave, MAPPING_PARAMETER_1600, 0x00, FALSE, sizeof(zero), &zero, EC_TIMEOUTSAFE);
    ec_SDOwrite(slave, MAPPING_PARAMETER_1600, 0x01, TRUE, sizeof(map_1600), &map_1600, EC_TIMEOUTSAFE);
    ec_SDOwrite(slave, MAPPING_PARAMETER_1600, 0x00, FALSE, sizeof(total_1600), &total_1600, EC_TIMEOUTSAFE);

    ec_SDOwrite(slave, RxPDO_ASSIGN, 0x00, FALSE, sizeof(zero), &zero, EC_TIMEOUTSAFE);
    ec_SDOwrite(slave, RxPDO_ASSIGN, 0x01, TRUE, sizeof(map_1c12), &map_1c12, EC_TIMEOUTSAFE);
    ec_SDOwrite(slave, RxPDO_ASSIGN, 0x00, FALSE, sizeof(total_1c12), &total_1c12, EC_TIMEOUTSAFE);

    ec_SDOwrite(slave, MAPPING_PARAMETER_1A00, 0x00, FALSE, sizeof(zero), &zero, EC_TIMEOUTSAFE);
    ec_SDOwrite(slave, MAPPING_PARAMETER_1A00, 0x01, TRUE, sizeof(map_1a00), &map_1a00, EC_TIMEOUTSAFE);
    ec_SDOwrite(slave, MAPPING_PARAMETER_1A00, 0x00, FALSE, sizeof(total_1a00), &total_1a00, EC_TIMEOUTSAFE);

    ec_SDOwrite(slave, TxPDO_ASSIGN, 0x00, FALSE, sizeof(zero), &zero, EC_TIMEOUTSAFE);
    ec_SDOwrite(slave, TxPDO_ASSIGN, 0x01, TRUE, sizeof(map_1c13), &map_1c13, EC_TIMEOUTSAFE);
    ec_SDOwrite(slave, TxPDO_ASSIGN, 0x00, FALSE, sizeof(total_1c13), &total_1c13, EC_TIMEOUTSAFE);

    ec_dcsync0(slave, TRUE, 22000000, 0);

    return 1;
}


int setup(char *ifname){
    int i, chk, slc;

    printf("Starting simple test\n");

    /* initialise SOEM, bind socket to ifname */
    if (ec_init(ifname)){

        printf("ec_init on %s succeeded.\n",ifname);
        /* find and auto-config slaves */
        if ( ec_config_init(FALSE) > 0 ){
            printf("%d slaves found and configured.\n",ec_slavecount);

            if(ec_slavecount >= 1) {

                ec_configdc();

                for(slc = 1; slc <= ec_slavecount; slc++) {
                    ec_slave[slc].PO2SOconfig = &slave_config;
                }
            }

            ec_config_map(&IOmap);

            printf("Slaves mapped, state to SAFE_OP.\n");
            /* wait for all slaves to reach SAFE_OP state */
            ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);

            /* read indevidual slave state and store in ec_slave[] */
            printf("Request operational state for all slaves\n");
            expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
            printf("Calculated workcounter %d\n", expectedWKC);
            ec_slave[0].state = EC_STATE_OPERATIONAL;
            /* send one valid process data to make outputs in slaves happy*/
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);
            /* request OP state for all slaves */
            ec_writestate(0);
            chk = 200;
            /* wait for all slaves to reach OP state */
            do
            {
                printf("chk = %d", chk);
                ec_send_processdata();
                ec_receive_processdata(EC_TIMEOUTRET);
                ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
            }
            while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));

            if (ec_slave[0].state == EC_STATE_OPERATIONAL){
                printf("Operational state reached for all slaves.\n");
                return 1;
            }
            else{

                printf("Not all slaves reached operational state.\n");
                ec_readstate();

                for(i = 1; i<=ec_slavecount ; i++){

                    if(ec_slave[i].state != EC_STATE_OPERATIONAL){
                        printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                            i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
                    }
                }
            }
        }
        else{
            printf("No slaves found!\n");
        }
    }
    else{
        printf("No socket connection on %s\nExecute as root\n",ifname);
    }
    exit(-1);
}


int enable_csp_mode(){
    outPDO *out;

    for(int slc = 1; slc <= ec_slavecount; slc++) {
        out = (outPDO*) ec_slave[slc].outputs;

        out->cntrl_word = 0b00110;
        out->operation_mode = 8;

        ec_send_processdata();
        ec_receive_processdata(EC_TIMEOUTRET);
        usleep(1000);
        
        out->cntrl_word = 0b1111;

        ec_send_processdata();
        ec_receive_processdata(EC_TIMEOUTRET);
        usleep(1000);
    }
    return 1;
}


void complete(){
    printf("\nRequest init state for all slaves\n");
    ec_slave[0].state = EC_STATE_INIT;
    /* request INIT state for all slaves */
    ec_writestate(0);

    printf("End simple test, close socket\n");
    /* stop SOEM, close socket */
    ec_close();
}
