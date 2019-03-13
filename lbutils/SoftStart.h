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
  static constexpr uint32_t PWM_FREQUENCY_HZ = 10000;
  static constexpr uint32_t PWM_RESOLUTION = 200;
  static constexpr uint16_t PWM_MAX = PWM_RESOLUTION + 1;
  static constexpr uint16_t PWM_MIN = 0;


private:
  Gpio leftH = Gpio(GPIOE, 9);
  Gpio rightH = Gpio(GPIOE, 11);

  //
  void setGpiosAsPWM(){
    Gpio::setup(GPIOE, 8, Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);

    //    Gpio::setup(GPIOE, 9, Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
    leftH.setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);

    Gpio::setup(GPIOE, 10, Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);

    //    Gpio::setup(GPIOE, 11, Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
    rightH.setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);

    Gpio::setAlternateFunc(GPIOE, 8, 0);  // AF0
    //    Gpio::setAlternateFunc(GPIOE, 9, 0);  // AF0
    Gpio::setAlternateFunc(GPIOE, 10, 0);  // AF0
    //    Gpio::setAlternateFunc(GPIOE, 11, 0);  // AF0
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


  void inline initTIM1(){
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;   // turn timer TIM1 enable
    TIM_TypeDef * tim = TIM1;

    uint32_t psc =  (Hardware::getPCLK() / (PWM_FREQUENCY_HZ * PWM_RESOLUTION*2)) -1;
    tim->PSC = uint16_t(psc); // 48MHz/(20kHz * 200*2) = 12 Set prescaler for clock i.e 8MHz
    tim->ARR = PWM_RESOLUTION; // (2) Set ARR = 200, as timer clock is 8MHz and center-aligned counting, the period is 50 us

    tim->CCR1 = 0; // no events at start
    tim->CCR2 = 0; // no events at start

    tim->CCMR1 |=
    // TIM1 channel 1
        TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | // Select PWM mode 1 on OC1 (OC1M = 110),
        TIM_CCMR1_OC1PE |                      // enable preload register on OC1 (OC1PE = 1, reset value)
    // TIM1 channel 2
        TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | // Select PWM mode 1 on OC1 (OC2M = 110),
        TIM_CCMR1_OC2PE;                      // enable preload register on OC1 (OC1PE = 1, reset value)
    tim->CCER |=
        TIM_CCER_CC1E |  // Select active high polarity on OC1 (CC1P = 0, reset value), enable the output on OC1 (CC1E = 1)
        TIM_CCER_CC2E;    // Select active high polarity on OC1 (CC1P = 0, reset value), enable the output on OC1 (CC1E = 1)

    // output config
    tim->CCER |=
        TIM_CCER_CC1NE |
        TIM_CCER_CC2NE |
        TIM_CCER_CC1NP |  // channel 1 polarisation
        TIM_CCER_CC2NP |  // channel 2 polarisation
        0;

    // let's run
    tim->BDTR |= TIM_BDTR_MOE; //  Enable output (MOE = 1)
    tim->CR1 |= TIM_CR1_CMS_1 | TIM_CR1_CMS_0    //  select center-aligned mode 3 (CMS = 11)
        | TIM_CR1_CEN; //Enable counter (CEN = 1)

//    tim->CR2 |= TIM_CR2_OIS1N | TIM_CR2_OIS2N;

    tim->EGR |= TIM_EGR_UG; //   Force update generation (UG = 1)

  }


public:
  void init(){
    initTIM1();
    setGpiosAsPWM();

  }

  inline bool getLeftH(){
    return leftH.getOutput();
  }

  inline bool getRightH(){
    return rightH.getOutput();
  }

  void setLeftPWM(uint32_t pwm){
    uint32_t val = pwm;
    if ((pwm > PWM_MIN) && (getLeftH())) val = PWM_MIN;
    TIM1->CCR1 = val;
    TIM1->EGR |= TIM_EGR_CC1G; //   Force update generation (CC1G = 1)
    //TIM1->EGR |= TIM_EGR_UG; //   Force update generation (UG = 1)
    }

  uint32_t getLeftPWM(){
    return TIM1->CCR1;
  }

  void setRightPWM(uint32_t pwm){
    uint32_t val = pwm;
    if ((pwm > PWM_MIN) && (getRightH())) val = PWM_MIN;
    TIM1->CCR2 = val;
    TIM1->EGR |= TIM_EGR_CC2G; //   Force update generation (CC1G = 1)
    //  TIM1->EGR |= TIM_EGR_UG; //   Force update generation (UG = 1)
  }

  uint32_t getRightPWM(){
    return TIM1->CCR2;
  }

  void setLeftL(bool level){
    bool lvl = level;
    if (level && (getLeftH())) lvl = false;
    setLeftPWM( lvl ? PWM_MAX : PWM_MIN);
    //TIM1->EGR |= TIM_EGR_UG; //   Force update generation (UG = 1)
  }

  void setRightL(bool level){
    bool lvl = level;
    if (level && (getRightH())) lvl = false;
    setRightPWM( lvl ? PWM_MAX : PWM_MIN);
    //  TIM1->EGR |= TIM_EGR_UG; //   Force update generation (UG = 1)
  }

  void setLeftH(bool level){
    //level = false;    //----------- usunac!
    if (level && (getLeftPWM() > PWM_MIN)){
      leftH.setOutput(false);
    }else{
      leftH.setOutput(level);
    }
  }

  void setRightH(bool level){
    //level = false;    //----------- usunac!
    if (level && (getRightPWM() > PWM_MIN)){
      rightH.setOutput(false);
    }else{
      rightH.setOutput(level);
    }
  }



};

#endif /* SOFTSTART_H_ */
