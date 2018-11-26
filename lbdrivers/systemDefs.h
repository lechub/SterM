/*
 * systemDefs.h
 *
 *  Created on: 9 lis 2018
 *      Author: lechu
 */

#ifndef SYSTEMDEFS_H_
#define SYSTEMDEFS_H_

#ifdef STM32F411
#include "stm32f4xx.h"
#endif

#ifdef STM32F072
#include "stm32f0xx.h"
#endif


//********** makra

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#define READ_REG_AND_FORGET(REG) ({volatile unsigned int tmp = REG; do{UNUSED(tmp);}while(0);})

//******************

#endif /* SYSTEMDEFS_H_ */
