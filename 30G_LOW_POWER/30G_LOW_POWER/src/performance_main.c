/* === INCLUDES ============================================================ */
#include <stdlib.h>
//#include "tal.h"
//#include "tal_helper.h"
//#include "ieee_const.h"
#include "app_init.h"
#include "app_peer_search.h"
#include "app_per_mode.h"
#include "app_range_mode.h"
#include "perf_api_serial_handler.h"
#include "app_config.h"
#if SAMD || SAMR21 || SAML21 || SAMR30
#include "system.h"
#else
#if (LED_COUNT > 0)
#include "led.h"
#endif /* (LED_COUNT > 0) */
#endif
#include "sio2host.h"
#include "board.h"
#include "performance_main.h"
#include "asf.h"//Runya
#include "usart.h"//Runya

extern struct usart_module usart_instance_ext;//Runya

/* === TYPES =============================================================== */

/**
 * \addtogroup group_perf_analyzer
 * \{
 */

/**
 * \brief This structure forms the jump table to address various main states in
 * this application.
 */
typedef struct {
	/* Function to initialize the main state */
	void (*func_main_state_init)(void *arg);
	/* Task function of main state */
	void (*func_task)(void);
	/* Tx done call back for main state */
	void (*func_tx_frame_done_cb)(retval_t status, frame_info_t *frame);
	/* Frame received call back for main state */
	void (*func_rx_frame_cb)(frame_info_t *frame);
	/* Energy scan result call back for main state */
	void (*func_ed_end_cb)(uint8_t energy_level);

	/* main state exit function : all timers should be stopped and other
	 * resources used in the state must be freed which is done here */
	void (*func_main_state_exit)(void);

	/* if main state has sub state, it can be initialized using this
	 * function */
	void (*func_sub_state_set)(uint8_t state, void *arg);
} state_function_t;

/* === MACROS ============================================================== */

/* === LOCALS ============================================================== */
static uint8_t storage_buffer[LARGE_BUFFER_SIZE];

/* === PROTOTYPES ========================================================== */
static void app_task(void);

void peer_timer_handler_cb(void);

void usr_task(void);

/* === GLOBALS ============================================================= */
static state_function_t const state_table[NUM_MAIN_STATES] = {
	{ /* INIT */
		init_state_init,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
	},
	{ /* WAIT_FOR_EVENT */
		wait_for_event_init,
		wait_for_event_task,
		NULL,
		wait_for_event_rx_cb,
		NULL,
		NULL,
		NULL,
	},
	//{ /* PEER_SEARCH_RANGE_TX */
//// 		peer_search_initiator_init,
//// 		peer_search_initiator_task,
//// 		peer_search_initiator_tx_done_cb,
//// 		peer_search_initiator_rx_cb,
//// 		NULL,
//// 		peer_search_initiator_exit,
//// 		peer_search_initiator_set_sub_state,
	//},
	{ /* PEER_SEARCH_PER_TX */
		peer_search_initiator_init,
		peer_search_initiator_task,
		peer_search_initiator_tx_done_cb,
		peer_search_initiator_rx_cb,
		NULL,
		peer_search_initiator_exit,
		peer_search_initiator_set_sub_state,
	},
	//{ /* PEER_SEARCH_RANGE_RX */
//// 		peer_search_receptor_init,
//// 		peer_search_receptor_task,
//// 		peer_search_receptor_tx_done_cb,
//// 		peer_search_receptor_rx_cb,
//// 		NULL,
//// 		peer_search_receptor_exit,
//// 		peer_search_receptor_set_sub_state,
	//},
	//{ /* PEER_SEARCH_PER_RX */
		//peer_search_receptor_init,
		//peer_search_receptor_task,
		//peer_search_receptor_tx_done_cb,
		//peer_search_receptor_rx_cb,
		//NULL,
		//peer_search_receptor_exit,
		//peer_search_receptor_set_sub_state,
	//},
	{ /* RANGE_TEST_TX_ON */
		range_test_tx_on_init,
		range_test_tx_on_task,
		NULL,
		range_test_rx_cb,
		NULL,
		range_test_tx_on_exit,
		NULL
	},
	{ /* RANGE_TEST_TX_OFF */
 		NULL,
 		range_test_tx_off_task,
 		NULL,
 		range_test_rx_cb,
	},
	//{ /* SINGLE_NODE_TESTS */
//// 		per_mode_initiator_init,
//// 		per_mode_initiator_task,
//// 		per_mode_initiator_tx_done_cb,
//// 		per_mode_initiator_rx_cb,
//// 		per_mode_initiator_ed_end_cb,
//// 		NULL,
//// 		NULL,
	//},
	{ /* PER_TEST_INITIATOR  */
		per_mode_initiator_init,
		per_mode_initiator_task,
		per_mode_initiator_tx_done_cb,
		per_mode_initiator_rx_cb,
		per_mode_initiator_ed_end_cb,
		per_mode_initiator_exit,
		NULL,
	},
	//{
		///* PER_TEST_RECEPTOR */
		//per_mode_receptor_init,
		//per_mode_receptor_task,
		//per_mode_receptor_tx_done_cb,
		//per_mode_receptor_rx_cb,
		//NULL,
		//NULL,
		//NULL,
	//}
};

volatile node_ib_t node_info;

uint8_t uc_peer_mode;

/* ! \} */
/* === IMPLEMENTATION ====================================================== */

/**
 * \brief Init function of the Performance Analyzer application
 * \ingroup group_app_init
 */
void performance_analyzer_init(void)
{
	sio2host_init();
	
	printf("\r\n**** SNR firmware ****");
	/*
	 * Power ON - so set the board to INIT state. All hardware, PAL, TAL and
	 * stack level initialization must be done using this function
	 */
	set_main_state(INIT, NULL);

	/* INIT was a success - so change to WAIT_FOR_EVENT state */
	set_main_state(WAIT_FOR_EVENT, NULL);

}

/**
 * \brief This task needs to be called in a while(1) for performing
 *  Performance Analyzer tasks
 */
void performance_analyzer_task(void)
{
	pal_task(); /* Handle platform specific tasks, like serial
	             * interface */
	tal_task(); /* Handle transceiver specific tasks */
	app_task(); /* Application task */
	
	//serial_data_handler();
	
	//usr_task(); /* Handle user interface pushing button */
		

}

/**
 * \brief Application task
 */
static void app_task(void)
{
	void (*handler_func)(void)
		= state_table[node_info.main_state].func_task;

	if (handler_func) {
		handler_func();
	}
}


/*
 *	\brief User task for button logic
 */
void usr_task(void) {
	
	uint8_t key_press_user = 0;
	user_button_state_t btn_state = NO_ACTION;

	btn_state = app_debounce_button_user();
	
	if( btn_state != NO_ACTION ){
	
		if(btn_state == START_PAIRING_STATE){
			btn_state = NO_ACTION;
			printf("\r\n Pairing start");
			
		} else if (btn_state == RSSI_STATE){
			btn_state = NO_ACTION;
			printf("\r\n Rssi start");
			
		} else if (btn_state == NON_WORKING_STATE){
			btn_state = NO_ACTION;
			printf("\r\n Non working state");
			
		} else{
			
		}
	}
		
		
// 	if( uc_peer_mode != 0  &&  uc_peer_mode != 0xFF ){
// 		uc_peer_mode = 0xFF;
// 		printf("\r\n WAIT_FOR_EVENT \r\n");
// 		sw_timer_start(T_APP_TIMER_RANGE, PEER_MODE_INTERVAL_IN_MICRO_SEC, SW_TIMEOUT_RELATIVE, (FUNC_PTR)peer_timer_handler_cb, NULL);
// 		set_main_state(WAIT_FOR_EVENT, NULL);
// 	}
}


/*
 * \brief Callback that is called if data has been received by trx.
 *
 * \param frame Pointer to received frame
 */
void tal_rx_frame_cb(frame_info_t *frame)
{
	void (*handler_func)(frame_info_t *frame);

	handler_func = state_table[node_info.main_state].func_rx_frame_cb;
	if (handler_func) {
		handler_func(frame);
	}

	/* free buffer that was used for frame reception */
	bmm_buffer_free((buffer_t *)(frame->buffer_header));
}

/*
 * \brief Callback that is called once tx is done.
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void tal_tx_frame_done_cb(retval_t status, frame_info_t *frame)
{
	void (*handler_func)(retval_t status, frame_info_t *frame);

	/* some spurious transmissions call back or app changed its state
	 * so neglect this call back */
	if (!node_info.transmitting) {
		return;
	}

	/* After transmission is completed, allow next transmission.
	 * Locking to prevent multiple transmissions simultaneously */
	node_info.transmitting = false;

	handler_func = state_table[node_info.main_state].func_tx_frame_done_cb;

	if (handler_func) {
		handler_func(status, frame);
	}
}

/*
 * \brief User call back function for finished ED Scan
 *
 * \param energy_level Measured energy level during ED Scan
 */
void tal_ed_end_cb(uint8_t energy_level)
{
	void (*handler_func)(uint8_t energy_level);

	handler_func = state_table[node_info.main_state].func_ed_end_cb;

	if (handler_func) {
		handler_func(energy_level);
	}
}

/*
 * \brief function to init the information base for device
 */
void config_node_ib(void)
{
	node_info.transmitting = false;

	/* Init tx frame info structure value that do not change during program
	 * execution */
	node_info.tx_frame_info = (frame_info_t *)storage_buffer;

	/* random number initialized for the sequence number */
	node_info.msg_seq_num = rand();

	/* Set peer addr to zero */
	node_info.peer_short_addr = 0;

	/* Set peer_found status as false */
	node_info.peer_found = false;

	/* Set config_mode to false */
	node_info.configure_mode = false;
}

/*
 * \brief Function to set the main state of state machine
 *
 * \param state   main state to be set
 * \param arg     argument passed in the state
 */
void set_main_state(main_state_t state, void *arg)
{
	void (*handler_func_exit)(void);
	void (*handler_func_init)(void *arg);
	void (*handler_sub_state_set)(uint8_t state, void *arg);

	handler_func_exit
		= state_table[node_info.main_state].func_main_state_exit;
	/* Exit the old state if not init state */
	if (handler_func_exit && state) {
		handler_func_exit();
	}

	/* Nullify all the previous tx call backs. In case of change in main
	 * state
	 * TX call back prevention is taken care here. If the state has sub
	 * states
	 * TX call back during sub state change must be taken care during sub
	 * state
	 * set exclusively
	 */
	node_info.transmitting = false;

	/* Welcome to new state */
	node_info.main_state = state;

	handler_func_init = state_table[state].func_main_state_init;

	/* Do init for new state and then change state */
	if (handler_func_init) {
		handler_func_init(arg);
	}

	handler_sub_state_set = state_table[state].func_sub_state_set;

	if (handler_sub_state_set) {
		handler_sub_state_set(0, arg);
	}
}

/*
 * \brief Function to transmit frames as per 802.15.4 std.
 *
 * \param dst_addr_mode     destination address mode - can be 16 or 64 bit
 * \param dst_addr          destination address
 * \param src_addr_mode     source address mode - can be 16 or 64 bit
 * \param msdu_handle       msdu handle for the upper layers to track packets
 * \param payload           data payload pointer
 * \param payload_length    data length
 * \param ack_req           specifies ack requested for frame if set to 1
 *
 * \return MAC_SUCCESS      if the TAL has accepted the data for frame
 *                          transmission
 *         TAL_BUSY         if the TAL is busy servicing the previous tx
 *                          request
 */
retval_t transmit_frame(uint8_t dst_addr_mode,
		uint8_t *dst_addr,
		uint8_t src_addr_mode,
		uint8_t msdu_handle,
		uint8_t *payload,
		uint16_t payload_length,
		uint8_t ack_req)
{
	uint8_t i;
	uint16_t temp_value;
	uint16_t frame_length;
	uint8_t *frame_ptr;
	uint8_t *temp_frame_ptr;
	uint16_t fcf = 0;

	/* Prevent multiple transmissions , this code is not reentrant*/
	if (node_info.transmitting) {
		return FAILURE;
	}

	node_info.transmitting = true;

	/* Get length of current frame. */
	frame_length = (FRAME_OVERHEAD + payload_length);

	/* Set payload pointer. */
	frame_ptr = temp_frame_ptr = (uint8_t *)node_info.tx_frame_info +
					LARGE_BUFFER_SIZE -
					payload_length - FCS_LEN;

	/*
	 * Payload is stored to the end of the buffer avoiding payload
	 * copying by TAL.
	 */
	for (i = 0; i < payload_length; i++) {
		*temp_frame_ptr++ = *(payload + i);
	}

	/* Source address */
	if (FCF_SHORT_ADDR == src_addr_mode) {
		frame_ptr -= SHORT_ADDR_LEN;
		convert_16_bit_to_byte_array(tal_pib.ShortAddress, frame_ptr);

		fcf |= FCF_SET_SOURCE_ADDR_MODE(FCF_SHORT_ADDR);
	} else {
		frame_ptr -= EXT_ADDR_LEN;
		frame_length += FCF_2_SOURCE_ADDR_OFFSET;

		convert_64_bit_to_byte_array(tal_pib.IeeeAddress, frame_ptr);

		fcf |= FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR);
	}

	/* Source PAN-Id */
#if (DST_PAN_ID == SRC_PAN_ID)
	/* No source PAN-Id included, but FCF updated. */
	fcf |= FCF_PAN_ID_COMPRESSION;
#else
	frame_ptr -= PAN_ID_LEN;
	temp_value = CCPU_ENDIAN_TO_LE16(SRC_PAN_ID);
	convert_16_bit_to_byte_array(temp_value, frame_ptr);
#endif

	/* Destination address */
	if (FCF_SHORT_ADDR == dst_addr_mode) {
		frame_ptr -= SHORT_ADDR_LEN;
		/* convert_16_bit_to_byte_array(*((uint16_t *)dst_addr), */
		/*		frame_ptr); */
		memcpy(frame_ptr, (uint8_t *)dst_addr, sizeof(uint16_t));
		fcf |= FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR);
	} else {
		frame_ptr -= EXT_ADDR_LEN;
		frame_length += PL_POS_DST_ADDR_START;

		/* convert_64_bit_to_byte_array(*((uint64_t *)dst_addr), */
		/*		frame_ptr); */
		memcpy(frame_ptr, (uint8_t *)dst_addr, sizeof(uint64_t));
		fcf |= FCF_SET_DEST_ADDR_MODE(FCF_LONG_ADDR);
	}

	/* Destination PAN-Id */
	temp_value = CCPU_ENDIAN_TO_LE16(DST_PAN_ID);
	frame_ptr -= PAN_ID_LEN;
	convert_16_bit_to_byte_array(temp_value, frame_ptr);

	/* Set DSN. */
	frame_ptr--;
	*frame_ptr = node_info.msg_seq_num;
	node_info.msg_seq_num++;

	/* Set the FCF. */
	fcf |= FCF_FRAMETYPE_DATA;
	if (ack_req) {
		fcf |= FCF_ACK_REQUEST;
	}

	frame_ptr -= FCF_LEN;
	convert_16_bit_to_byte_array(CCPU_ENDIAN_TO_LE16(fcf), frame_ptr);

	/* First element shall be length of PHY frame. */
	frame_ptr--;
	*frame_ptr = (uint8_t)frame_length;

	/* Finished building of frame. */
	node_info.tx_frame_info->mpdu = frame_ptr;

	/* Place msdu handle for tracking */
	node_info.tx_frame_info->msduHandle = msdu_handle;

	/* transmit the frame */
	return(tal_tx_frame(node_info.tx_frame_info, CSMA_UNSLOTTED, true));
}

void app_alert()
{
	while (1) {
		#if LED_COUNT > 0
		#ifdef LED0_ACTIVE_LEVEL
		LED_Toggle(LED0);
		#else
		LED_Toggle(LED0_GPIO);
		#endif
		#endif

		#if LED_COUNT > 1
		LED_Toggle(LED1_GPIO);
		#endif

		#if LED_COUNT > 2
		LED_Toggle(LED2_GPIO);
		#endif

		#if LED_COUNT > 3
		LED_Toggle(LED3_GPIO);
		#endif

		#if LED_COUNT > 4
		LED_Toggle(LED4_GPIO);
		#endif

		#if LED_COUNT > 5
		LED_Toggle(LED5_GPIO);
		#endif

		#if LED_COUNT > 6
		LED_Toggle(LED6_GPIO);
		#endif

		#if LED_COUNT > 7
		LED_Toggle(LED7_GPIO);
		#endif
		delay_us(0xFFFF);
	}
}

void peer_timer_handler_cb(void){
	
	printf("\r\n SNR READ STATE");
	uc_peer_mode = 0x00;
	set_main_state(PER_TEST_INITIATOR, 0);
}

/* EOF */
