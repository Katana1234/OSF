/*
* Description: This is based on the open source firmware for TSDZ2 motor made by mbrusa
* It is adapted for TSDZ8 
*
* Related Document: See README.md
*
* Copyright (C) Casainho, Leon, MSpider65 2020.
*
* Released under the GPL License, Version 3
*/
#include "cybsp.h"
#include "cy_utils.h"

#include "cy_retarget_io.h"

#include "SEGGER_RTT.h"


#define MY_ENABLED 1
#define MY_DISABLED 0
#define uCPROBE_GUI_OSCILLOSCOPE MY_DISABLED // ENABLED
#if(uCPROBE_GUI_OSCILLOSCOPE == MY_ENABLED)
#include "ProbeScope/probe_scope.h"
#endif

#include "uart.h"
#include "motor.h"
#include "ebike_app.h"
#include "eeprom.h"


/*******************************************************************************
* Macros
*******************************************************************************/
/* SysTick timer frequency in Hz  */
#define TICKS_PER_SECOND            1000 // 1 tick = 1msec


/*******************************************************************************
* Global Variables
*******************************************************************************/

/* Variable for keeping track of time */
volatile uint32_t system_ticks = 0;
uint32_t loop_25ms_ticks = 0;  
uint32_t start = 0 ; // mainly for debugging ; allow to print some variable every e.g. 5 sec

// maximum duty cycle
extern uint8_t ui8_pwm_duty_cycle_max; 

// for debugging only at the beginning
uint32_t count = 0;
uint32_t speed = 0; 
uint32_t pas_1 = 0;
uint32_t uart_rx = 0;
uint32_t brake = 0;
uint32_t unknown = 0;


extern volatile uint8_t ui8_received_package_flag ;
extern volatile uint8_t ui8_tx_buffer[];

// for debugging // probably to remove todo
extern volatile uint32_t hall_print_pos ; 
extern volatile uint32_t hall_print_angle ;
extern volatile uint32_t hall_print_pos2; // current hall pattern (after a sampling delay)
extern volatile uint32_t hall_print_interval ; // interval between 2 correct hall transitions
extern volatile uint32_t posif_SR0; 
extern volatile uint32_t posif_SR1;
extern volatile uint32_t posif_print_current_pattern ;

extern volatile uint32_t hall_pattern_irq;                   // current hall pattern
extern volatile uint16_t hall_pattern_change_ticks_irq; // ticks from ccu4 slice 2 for last pattern change
extern uint8_t  previous_hall_pattern; 

extern volatile uint16_t ui16_a ;
extern volatile uint16_t ui16_b ;
extern volatile uint16_t ui16_c ;
extern uint8_t ui8_svm_table_index_print  ; 
extern uint16_t ui16_temp_print  ;
extern uint16_t ui16_a_print ;
extern uint16_t ui16_new_angle_print; 
uint16_t ccu4_S2_timer ; 
extern uint16_t current_average ;
extern uint16_t hall_pattern_intervals[8];
// to measure time in irq
extern volatile uint16_t debug_time_ccu8_irq0; // to debug time in irq0 CCU8 (should be less than 25usec; 1 = 4 usec )
extern volatile uint16_t debug_time_ccu8_irq1; // to debug time in irq0 CCU8 (should be less than 25usec; 1 = 4 usec )
extern volatile uint16_t debug_time_ccu8_irq1b; // to debug time in irq0 CCU8 (should be less than 25usec; 1 = 4 usec )
extern volatile uint16_t debug_time_ccu8_irq1c; // to debug time in irq0 CCU8 (should be less than 25usec; 1 = 4 usec )
extern volatile uint16_t debug_time_ccu8_irq1d; // to debug time in irq0 CCU8 (should be less than 25usec; 1 = 4 usec )
extern volatile uint16_t debug_time_ccu8_irq1e; // to debug time in irq0 CCU8 (should be less than 25usec; 1 = 4 usec )

extern volatile uint8_t ui8_adc_battery_current_filtered;
extern uint8_t ui8_battery_current_filtered_x10;
extern uint16_t ui16_display_data_factor; 
extern uint8_t ui8_g_foc_angle;
extern uint8_t ui8_throttle_adc_in;

// to debug
extern uint16_t saved_current_min ;
extern uint16_t saved_current_max ;
//extern volatile uint32_t real_ticks_interval[8];  // real interval between 2 hall patterns
//extern volatile uint32_t expected_ticks_interval[8]; // expected interval based on the defined sensor positions and number ot tick for one electric rotation 
//extern volatile uint32_t interval_counter;
//uint32_t real_ticks_interval_avg[8]= {0};  // real interval between 2 hall patterns
//uint32_t expected_ticks_interval_avg[8] = {0}; // expected interval based on the defined sensor positions and number ot tick for one electric rotation 
//int32_t  tick_error_avg[8] = {0};
//int32_t  angle_error_avg[8] = {0} ;

// to debug
extern uint8_t current_hall_pattern_log;
extern uint16_t last_hall_pattern_change_ticks_log;
extern uint16_t previous_360_ref_ticks_log;
extern uint16_t ui16_hall_counter_total_log;
extern uint32_t ui32_angle_per_tick_X16shift_log;
extern uint16_t ui16_measured_angle_X16bits_log;
extern uint8_t best_ref_angle_log;
extern uint8_t ui8_hall_ref_angles_log;
extern volatile bool new_log_flag;
uint8_t test1 ;
uint8_t test2 ;
uint8_t test3 ;
uint8_t test4 ;
    

extern volatile uint8_t best_ref_angle[8] ;
extern uint32_t best_ref_angle_X16bits[8] ;

extern uint16_t ui16_adc_motor_phase_current;
extern volatile uint8_t ui8_adc_motor_phase_current_max;
extern volatile uint16_t ui16_hall_counter_total;
extern volatile uint16_t ui16_adc_voltage;
extern volatile uint16_t ui16_adc_voltage_cut_off;

extern uint8_t global_offset_angle;
/*******************************************************************************
* Function Name: SysTick_Handler
********************************************************************************
* Summary:
* This is the interrupt handler function for the SysTick timer interrupt.
* It counts the time elapsed in milliseconds since the timer started. 
*******************************************************************************/
void SysTick_Handler(void)
{
    system_ticks++;
}


#define CHANNEL_NUMBER_PIN_2_2              (7U) // Torque
#define CHANNEL_NUMBER_PIN_2_3              (5U) // unknown
#define CHANNEL_NUMBER_PIN_2_4              (6U) // Battery
#define CHANNEL_NUMBER_PIN_2_5              (7U) // Throttle
#define CHANNEL_NUMBER_PIN_2_6              (0U) // Vcc
#define CHANNEL_NUMBER_PIN_2_7              (1U) // Unknown

#define CHANNEL_NUMBER_PIN_2_8              (1U) // Current 1 if group 0
#define CHANNEL_NUMBER_PIN_2_9              (2U) // Current U if group 0
#define CHANNEL_NUMBER_PIN_2_10             (3U) // Current v if group 0
#define CHANNEL_NUMBER_PIN_2_11             (4U) // Current W if group 0


// ************* declaration **********
void jlink_print_system_state();


//*******************************************************************************
// Function Name: main
//********************************************************************************

int main(void)
{
    cy_rslt_t result;

    uint32_t wait_time = 600000;
    while (wait_time > 0){  // wait a little at power on to let VCC be stable and so get probably better ADC conversions
        wait_time--;
    }
    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    

    // fill table used for knowing the best hall pattern positions
    for (uint8_t i = 0; i<8 ; i++) { 
        best_ref_angle[i] = ui8_hall_ref_angles[i];
        best_ref_angle_X16bits[i] = ui8_hall_ref_angles[i] << 8;
    } // init best value with reference values.

    #if(uCPROBE_GUI_OSCILLOSCOPE == MY_ENABLED)
    ProbeScope_Init(20000);
    #endif
    // init segger to allow kind of printf
    SEGGER_RTT_Init ();     
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
    SEGGER_RTT_WriteString(0, RTT_CTRL_CLEAR); // clear the RTT terminal

    // set the PWM in such a way that PWM are set to passive levels when processor is halted for debugging (safety)
    XMC_CCU8_SetSuspendMode(ccu8_0_HW, XMC_CCU8_SUSPEND_MODE_SAFE_STOP);
    XMC_CCU4_SetSuspendMode(ccu4_0_HW, XMC_CCU4_SUSPEND_MODE_SAFE_STOP);

    /* Initialize printf retarget  when printf on uart is used*/
    //cy_retarget_io_init(CYBSP_DEBUG_UART_HW);

    /* System timer configuration */
    SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);
    // CCU8 slice 3 (IRQ at mid point) generates a SR3 when period match and this trigger a VADC group 0 for queue
    // CCU8 slice 2 (PWM) is configured in device generator to generate a sr2 when ONE match
    //  but device configurator does not allow to setup a second trigger for vadc queue conversion
    // some setup has to be done manually in the group1 queue
     // we have to connect sr2 to vadc group1 queue, to activate trigering and to disable gating.

    //Here I overwite the config defined in device manage (and generated in cycfg_peripherals.c)     
    const XMC_VADC_QUEUE_CONFIG_t vadc_0_group_0_queue_config2 =
    {
        .conv_start_mode = (uint32_t) XMC_VADC_STARTMODE_WFS,
        .req_src_priority = (uint32_t) XMC_VADC_GROUP_RS_PRIORITY_3,
        .src_specific_result_reg = (uint32_t) 0,
        .trigger_signal = (uint32_t) XMC_VADC_REQ_TR_P, // use gate set up
        .trigger_edge = (uint32_t) XMC_VADC_TRIGGER_EDGE_ANY,
        .gate_signal = (uint32_t) XMC_VADC_REQ_GT_E, //use CCU8_ST3A = when timer is at mid period counting up
        .timer_mode = (uint32_t) false,
        .external_trigger = (uint32_t) true,
    };
    XMC_VADC_GROUP_QueueSetGatingMode(vadc_0_group_0_HW, (XMC_VADC_GATEMODE_t) XMC_VADC_GATEMODE_IGNORE);
    XMC_VADC_GROUP_QueueInit(vadc_0_group_0_HW, &vadc_0_group_0_queue_config2);

    const XMC_VADC_QUEUE_CONFIG_t vadc_0_group_1_queue_config2 = {
            .conv_start_mode = (uint32_t) XMC_VADC_STARTMODE_WFS,
            .req_src_priority = (uint32_t) XMC_VADC_GROUP_RS_PRIORITY_2,
            .src_specific_result_reg = (uint32_t) 0,
            .trigger_signal = (uint32_t) XMC_VADC_REQ_TR_P,  // use gate set up
            .trigger_edge = (uint32_t) XMC_VADC_TRIGGER_EDGE_ANY,
            .gate_signal = (uint32_t) XMC_VADC_REQ_GT_E, ////use CCU8_ST3A = when timer is at mid period counting up
            .timer_mode = (uint32_t) false,
            .external_trigger = (uint32_t) true,
        };
    XMC_VADC_GROUP_QueueSetGatingMode(vadc_0_group_1_HW, (XMC_VADC_GATEMODE_t) XMC_VADC_GATEMODE_IGNORE);
    XMC_VADC_GROUP_QueueInit(vadc_0_group_1_HW, &vadc_0_group_1_queue_config2);

    /* Start the temperature measurement */
    XMC_SCU_StartTempMeasurement();

    // **** load the config from flash
    XMC_WDT_Service();
    // todo should be adapted to get them from flash memory; currently we only use default)
    init_extra_fields_config (); // get the user parameters (
    // todo : change when eeprom is coded properly add some initialisation (e.g. m_configuration_init() and ebike_app.init)
    // currently it is filled with parameters from user setup + some dummy values (e.g. for soc)
    m_configuration_init();
    // add some initialisation in ebike_app.init
    ebike_app_init();
    XMC_WDT_Service();
    // set initial position of hall sensor and first next expected one in shadow and load immediately in real register
    //posif_init_position();
    get_hall_pattern();
    previous_hall_pattern = 0; // use a different hall pattern to force the angle. 
    XMC_POSIF_Start(HALL_POSIF_HW);
    
    // set interrupt 
    NVIC_SetPriority(CCU40_1_IRQn, 0U); //capture hall pattern and slice 2 time when a hall change occurs
	NVIC_EnableIRQ(CCU40_1_IRQn);
    /* CCU80_0_IRQn and CCU80_1_IRQn. slice 3 interrupt on counting up and down. at 19 khz to manage rotating flux*/
	NVIC_SetPriority(CCU80_0_IRQn, 1U);
	NVIC_EnableIRQ(CCU80_0_IRQn);
    NVIC_SetPriority(CCU80_1_IRQn, 1U);
	NVIC_EnableIRQ(CCU80_1_IRQn);

    /* Enable Global Start Control CCU80  in a synchronized way*/
    XMC_SCU_SetCcuTriggerHigh(SCU_GENERAL_CCUCON_GSC80_Msk);
    XMC_SCU_SetCcuTriggerLow(SCU_GENERAL_CCUCON_GSC80_Msk);
    uint32_t retry_start_counter = 10;
    while ((!XMC_CCU8_SLICE_IsTimerRunning(PHASE_U_TIMER_HW)) && (retry_start_counter > 0)){ // to be sure it is running
        XMC_SCU_SetCcuTriggerHigh(SCU_GENERAL_CCUCON_GSC80_Msk);
        XMC_SCU_SetCcuTriggerLow(SCU_GENERAL_CCUCON_GSC80_Msk);
    
        //SEGGER_RTT_printf(0, "Retry CCU8 start; still %u try\r\n", retry_counter);
    }

    //XMC_VADC_GLOBAL_EnablePostCalibration(vadc_0_HW, 0U);
    //XMC_VADC_GLOBAL_EnablePostCalibration(vadc_0_HW, 1U);
    //XMC_VADC_GLOBAL_StartupCalibration(vadc_0_HW);
    
    start = system_ticks;
    
//***************************** while ************************************
    while (1) // main loop
    {     
	    // avoid a reset
        XMC_WDT_Service(); // reset if we do not run here within the 0,5 sec
    
        // when there is no frame waiting for process (ui8_received_package_flag == 0), try to get incoming data from UART but do not process them
        // When frame is full, data are processed in ebike_app.c once every 100 msec
        if (ui8_received_package_flag == 0) {
            fillRxBuffer();
        }
        // must be activated for real production
        // Here we should call a funtion every 25 msec (based on systick or on an interrupt based on a CCU4 timer)
        if ((system_ticks - loop_25ms_ticks) > 25) { 
            loop_25ms_ticks = system_ticks;
            ebike_app_controller();  // this performs some checks and update some variable every 25 msec
        }

              
        #if (uCPROBE_GUI_OSCILLOSCOPE == MY_ENABLED)
        ProbeScope_Sampling(); // this should be moved e.g. in a interrupt that run faster
        #endif
        
        // for debug
        
        #if (DEBUG_ON_JLINK == 1)
         // do debug if communication with display is working
        //if( take_action(1, 250)) SEGGER_RTT_printf(0,"Light is= %u\r\n", (unsigned int) ui8_lights_button_flag);
        if (ui8_system_state) { // print a message when there is an error detected
            if( take_action(1,200)) jlink_print_system_state();
        }

        if (new_log_flag){
            SEGGER_RTT_printf(0,"chp=%u lhpct=%u    prt=%u hct=%u apt=%u  ma=%u bra%u hra=%u \r\n",
                (unsigned int) current_hall_pattern_log,
                (unsigned int) last_hall_pattern_change_ticks_log,
                (unsigned int) previous_360_ref_ticks_log,
                (unsigned int) ui16_hall_counter_total_log,
                (unsigned int) ui32_angle_per_tick_X16shift_log,
                (unsigned int) ui16_measured_angle_X16bits_log,
                (unsigned int) best_ref_angle_log,
                (unsigned int) ui8_hall_ref_angles_log
                //(unsigned int) test1,
                //(unsigned int) test2,
                //(unsigned int) test3,
                //(unsigned int) test4
            );
            new_log_flag = false;
        }    



//        if( take_action(2, 500)) SEGGER_RTT_printf(0,"Adc current= %u  current_Ax10=%u  factor=%u\r\n", 
//            (unsigned int) ui8_adc_battery_current_filtered ,
//            (unsigned int) ui8_battery_current_filtered_x10 , 
//            (unsigned int) ui16_display_data_factor
//            );
        // monitor duty cycle, current when motor is running
        /*
        if( take_action(3, 1000)) SEGGER_RTT_printf(0,"dctarg=%u dc=%u    ctarg=%u cfilt=%u Throttle=%u  erps=%u foc%u\r\n",
            (unsigned int) ui8_controller_duty_cycle_target,
            (unsigned int) ui8_g_duty_cycle,
            (unsigned int) ui8_controller_adc_battery_current_target,
            (unsigned int) ui8_adc_battery_current_filtered,
            (unsigned int) ui8_throttle_adc_in,
            (unsigned int) ui16_motor_speed_erps,
            (unsigned int) ui8_g_foc_angle
            //(unsigned int) XMC_CCU8_SLICE_IsTimerRunning(PHASE_U_TIMER_HW),
            //(unsigned int) ui8_motor_enabled
        );
        */
        
        #define DEBUG_HALL_POSITIONS (1)
        #if (DEBUG_HALL_POSITIONS == (1) )
        if( take_action(6, 5000)) {
            //for (uint8_t i = 1; i<7; i++){
            //    real_ticks_interval_avg[i]= real_ticks_interval[i] *6 / INTERVAL_COUNTER;  // real interval between 2 hall patterns
            //    expected_ticks_interval_avg[i] = expected_ticks_interval[i] *6 / INTERVAL_COUNTER; // expected interval based on the defined sensor positions and number ot tick for one electric rotation 
            //    tick_error_avg[i] = (int32_t) expected_ticks_interval_avg[i] - (int32_t) real_ticks_interval_avg[i];
            //    angle_error_avg[i] = tick_error_avg[i] * 256 / ui16_hall_counter_total ;
            //}
            SEGGER_RTT_printf(0,
            "c10b=%u  dc=%u erps=%u t360=%u  best1=%u best2=%u best3=%u best4=%u best5=%u best6=%u\r\n",
                (unsigned int) ui8_adc_battery_current_filtered,
                (unsigned int) ui8_g_duty_cycle,
                (unsigned int) ui16_motor_speed_erps,
                (unsigned int) ui16_hall_counter_total,
                best_ref_angle[1], best_ref_angle[2], best_ref_angle[3], best_ref_angle[4], best_ref_angle[5], best_ref_angle[6]
            );
            //for (uint8_t i = 1; i<7; i++){
            //    real_ticks_interval[i] = 0 ;  // reset real interval between 2 hall patterns
            //    expected_ticks_interval[i] = 0; // reset expected interval based on the defined sensor positions and number ot tick for one electric rotation 
            //}
            //interval_counter = INTERVAL_COUNTER; // allow next measurement    
        }
        #endif
        #endif    // end DEBUG_ON_JLINK
       
    } // end while main loop
}


void jlink_print_system_state(){
    switch (ui8_system_state) {
        case 1: 
            SEGGER_RTT_printf(0,"Error : overvoltage\r\n");
            break;
        case 2: 
            SEGGER_RTT_printf(0,"Error : torque_sensor\r\n");
            break;
        case 3: 
            SEGGER_RTT_printf(0,"Error : cadence_sensor\r\n");
            break;
        case 4: 
            SEGGER_RTT_printf(0,"Error : motor_blocked\n");
            break;
        case 5: 
            SEGGER_RTT_printf(0,"Error : throttle\r\n");
            break;
        case 6: 
            SEGGER_RTT_printf(0,"Error : overtemperature\r\n");
            break;
        case 7: 
            SEGGER_RTT_printf(0,"Error : battery_overcurrent\r\n");
            break;
        case 8: 
            SEGGER_RTT_printf(0,"Error : speed_sensor\r\n");
            break;
        case 9: 
            SEGGER_RTT_printf(0,"Error : motor_check\r\n");
            break;
             
    }
}    

/* END OF FILE */
