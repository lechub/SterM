/*
 * Gpio.h
 *
 *  Created on: 23 sty 2014
 *      Author: lechu
 */

#ifndef GPIO_H_
#define GPIO_H_


#include "systemDefs.h"


class Gpio{

public:
  typedef enum { //MODER
    INPUT 			= 0b00,
    OUTPUT			= 0b01,
    ALTERNATE		= 0b10,
    ANALOG 			= 0b11,
  }GpioMode;

  typedef enum {	//OTYPER
    PushPull 	= 0b0,
    OpenDrain 	= 0b1,
    NoMatter	= 0b1
  }GpioOType;

  typedef enum {
    NoPull		= 0b00,
    PullUp		= 0b01,
    PullDown 	= 0b10
  }GpioPuPd;

  typedef enum {
    LowSpeed		= 0b00,	//LowSpeed
    MediumSpeed		= 0b01,
    HighSpeed		= 0b10,
    MaximumSpeed	= 0b11	//HighSpeed
  }GpioSpeed;

protected:
  GPIO_TypeDef *	gpio;
  uint16_t	pinNr;

public:

  // setting gpio registers

  inline void setMODE(const GpioMode mode){
    gpio->MODER  &= ~(GPIO_MODER_MODER0 << (pinNr * 2));
    gpio->MODER |= ((uint32_t)mode << (pinNr * 2));
  }

  //SetSpeed i setOType tylko dla trybu GPIO_Mode_OUT i GPIO_Mode_AF

  inline void setSpeed(const GpioSpeed speed){
    gpio->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (pinNr * 2));
    gpio->OSPEEDR |= ((uint32_t)(speed) << (pinNr * 2));
  }

  inline void setPullUpDn(const GpioPuPd pupd){
    gpio->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)pinNr * 2));
    gpio->PUPDR |= (((uint32_t)pupd) << (pinNr * 2));
  }

  /* Output mode configuration */
  inline void setOType(const GpioOType otype){
    gpio->OTYPER &= (uint16_t)(~((GPIO_OTYPER_OT_0) << (pinNr)));
    gpio->OTYPER |= (uint16_t)(((uint16_t)otype) << (pinNr));
  }


  // getting gpio registers

  inline GpioMode getMode(){
    uint32_t tmp = gpio->MODER >>(pinNr * 2);
    return GpioMode(tmp & GPIO_MODER_MODER0);
  }

  inline GpioPuPd getPull(){
    uint32_t tmp = gpio->PUPDR >>(pinNr * 2);
    return GpioPuPd(tmp & GPIO_PUPDR_PUPDR0);
  }

  inline GpioSpeed getSpeed(){
    uint32_t tmp = gpio->OSPEEDR >>(pinNr * 2);
    return GpioSpeed(tmp & GPIO_OSPEEDER_OSPEEDR0);
  }

  inline GpioOType getOType(){
    uint32_t tmp = gpio->OTYPER >> pinNr;
    return GpioOType(tmp & GPIO_OTYPER_OT_0);
  }


  Gpio(GPIO_TypeDef * gpioDef, uint16_t pin_Nr){
    gpio = gpioDef;
    pinNr = pin_Nr;
    setMODE(GpioMode::ANALOG);  // for power save
    setPullUpDn(GpioPuPd::NoPull);
    setSpeed(GpioSpeed::LowSpeed);
    setOType(GpioOType::OpenDrain);
  }

  Gpio(GPIO_TypeDef * gpioDef, const uint16_t pin_Nr, GpioMode mode, GpioOType oType, GpioPuPd pupd, GpioSpeed speed){
    gpio = gpioDef;
    pinNr = pin_Nr;
    setMODE(mode);
    setOType(oType);
    setPullUpDn(pupd);
    setSpeed(speed);
  }


  inline void setOutputUp(){
    gpio->BSRR = (1<<pinNr);
  }
  inline void setOutputDown(){
    gpio->BSRR = (1<<(pinNr + 16l));
  }

  inline void setOutput(bool newstate){ newstate? setOutputUp() : setOutputDown(); }

  inline bool getOutput(){
    return (gpio->ODR & (1<<pinNr));
  }

  inline void toggleOutput(){ getOutput()? setOutputDown() : setOutputUp(); }


  inline bool getInput(){
    return (gpio->IDR & (1<<pinNr));
  }


  void setup(GpioMode mode, GpioOType oType, GpioPuPd pupd, GpioSpeed speed){
    setMODE(mode);
    setOType(oType);
    setPullUpDn(pupd);
    setSpeed(speed);
  }

  void setupFromClone(Gpio * dolly){
    setMODE(dolly->getMode());
    setOType(dolly->getOType());
    setPullUpDn(dolly->getPull());
    setSpeed(dolly->getSpeed());
  }

  bool setAlternateFunc(uint8_t alternateFunction){
    if (alternateFunction > 7) return false;
    constexpr uint32_t BitMask = 0b00111; // najstarszy zarezerwowany
    uint32_t val = alternateFunction;
    if (pinNr < 8){
      uint8_t offset = uint8_t(pinNr * 4);
      uint32_t tmp = gpio->AFR[0];
      tmp &= ~(BitMask << offset);
      tmp |= val << offset;
      gpio->AFR[0] = tmp;
    }
    if ((pinNr >= 8)&&(pinNr < 16)){
      uint8_t offset = uint8_t((pinNr - 8) * 4);
      uint32_t tmp = gpio->AFR[1];
      tmp &= ~(BitMask << offset);
      tmp |= val << offset;
      gpio->AFR[1] = tmp;
    }
    return true;
  }

 inline uint8_t getAlternativeFunc(){
   constexpr uint32_t BitMask = 0b00111;
   uint32_t tmp = 0;
   if (pinNr < 8){
     tmp = (gpio->AFR[0]) >> (pinNr * 4);
   }
   if ((pinNr >= 8)&&(pinNr < 16)){
     tmp = (gpio->AFR[1]) >> ((pinNr - 8) * 4);
   }
   return uint8_t(tmp  & BitMask);
 }

};

//} /* namespace STM32F0xx */

//----------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------

//void gpio_init(void);
//----------------------------------------------------
#ifdef __cplusplus
}
#endif
//----------------------------------------------------



#endif /* GPIO_H_ */
