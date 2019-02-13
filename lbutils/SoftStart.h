/*
 * SoftStart.h
 *
 *  Created on: 13 lut 2019
 *      Author: lechu
 */

#ifndef SOFTSTART_H_
#define SOFTSTART_H_

#include "Gpio.h"
#include "Hardware.h"

class SoftStart {
public:
  static constexpr uint32_t PWM_FREQUENCY_HZ = 20000;
  static constexpr uint32_t PWM_RESOLUTION = 200;

private:
  static void setGpiosAsPWM(){
    Gpio::setup(GPIOE, 8, Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
    Gpio::setup(GPIOE, 9, Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
    Gpio::setup(GPIOE, 10, Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
    Gpio::setup(GPIOE, 11, Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
    Gpio::setAlternateFunc(GPIOE, 8, 0);  // AF0
    Gpio::setAlternateFunc(GPIOE, 9, 0);  // AF0
    Gpio::setAlternateFunc(GPIOE, 10, 0);  // AF0
    Gpio::setAlternateFunc(GPIOE, 11, 0);  // AF0
  }

//  static void setGpiosAsOutput(){
//    Gpio::setOutputDown(GPIOE, 8);
//    Gpio::setOutputDown(GPIOE, 9);
//    Gpio::setOutputDown(GPIOE, 10);
//    Gpio::setOutputDown(GPIOE, 11);
//    Gpio::setup(GPIOE, 8, Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
//    Gpio::setup(GPIOE, 9, Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
//    Gpio::setup(GPIOE, 10, Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
//    Gpio::setup(GPIOE, 11, Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
//  }

  static void inline initTIM1(){
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;   // turn timer TIM1 enable

    TIM1->PSC = (Hardware::getPCLK() / (PWM_FREQUENCY_HZ * PWM_RESOLUTION*2)) -1; // 48MHz/(20kHz * 200*2) = 12 Set prescaler for clock i.e 8MHz
    TIM1->ARR = PWM_RESOLUTION; // (2) Set ARR = 200, as timer clock is 8MHz and center-aligned counting, the period is 50 us

    // TIM1 channel 1
    TIM1->CCR1 = 0; // no events at start
    TIM1->CCMR1 |=
        TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | // Select PWM mode 1 on OC1 (OC1M = 110),
        TIM_CCMR1_OC1PE;                      // enable preload register on OC1 (OC1PE = 1, reset value)

    // TIM1 channel 2
    TIM1->CCR2 = 0; // no events at start
    TIM1->CCMR1 |=
        TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | // Select PWM mode 1 on OC1 (OC2M = 110),
        TIM_CCMR1_OC2PE;                      // enable preload register on OC1 (OC1PE = 1, reset value)
    TIM1->CCER |= TIM_CCER_CC2E; // Select active high polarity on OC1 (CC1P = 0, reset value), enable the output on OC1 (CC1E = 1)

    // let's run
    TIM1->CCER |= TIM_CCER_CC1E; // Select active high polarity on OC1 (CC1P = 0, reset value), enable the output on OC1 (CC1E = 1)
    TIM1->BDTR |= TIM_BDTR_MOE; //  Enable output (MOE = 1)
    TIM1->CR1 |= TIM_CR1_CMS_0    //  select center-aligned mode 1 (CMS = 01)
        | TIM_CR1_CEN; //Enable counter (CEN = 1)
    TIM1->EGR |= TIM_EGR_UG; //   Force update generation (UG = 1)

  }


public:
  static void init(){
    initTIM1();
    setGpiosAsPWM();

  }

};

#endif /* SOFTSTART_H_ */
