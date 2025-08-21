/*
 * tim.h
 *
 *  Created on: May 7, 2025
 *      Author: Jogindhiran
 */

#ifndef TIM_H_
#define TIM_H_

#define SR_UIF 		(1U<<0)	//UIF becomes 1 when timeout occurs
#define SR_CC1IF	(1U<<1) //For input capture

void tim2_1hz_init(void);
void tim2_pa5_output_compare(void);
void tim3_pa6_input_capture(void);

#endif /* TIM_H_ */
