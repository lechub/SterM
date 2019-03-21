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
  uint8_t	pinNr;

public:

  //SetSpeed i setOType tylko dla trybu GPIO_Mode_OUT i GPIO_Mode_AF

  //***********************************************
  // Static methods for doing without objects

  static inline void setMODE(GPIO_TypeDef* gpioReg, uint8_t pin, const GpioMode mode){
    gpioReg->MODER  &= ~(GPIO_MODER_MODER0 << (pin * 2));
    gpioReg->MODER |= ((uint32_t)mode << (pin * 2));
  }

  static inline void setSpeed(GPIO_TypeDef* gpioReg, uint8_t pin, const GpioSpeed speed){
    gpioReg->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (pin * 2));
    gpioReg->OSPEEDR |= ((uint32_t)(speed) << (pin * 2));
  }

  static inline void setPullUpDn(GPIO_TypeDef* gpioReg, uint8_t pin, const GpioPuPd pupd){
    gpioReg->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)pin * 2));
    gpioReg->PUPDR |= (((uint32_t)pupd) << (pin * 2));
  }

  /* Output mode configuration */
  static inline void setOType(GPIO_TypeDef* gpioReg, uint8_t pin, const GpioOType otype){
    gpioReg->OTYPER &= (uint16_t)(~((GPIO_OTYPER_OT_0) << (pin)));
    gpioReg->OTYPER |= (uint16_t)(((uint16_t)otype) << (pin));
  }

  static inline GpioMode getMode(GPIO_TypeDef* gpioReg, uint8_t pin){
    uint32_t tmp = gpioReg->MODER >>(pin * 2);
    return GpioMode(tmp & GPIO_MODER_MODER0);
  }

  static inline GpioPuPd getPull(GPIO_TypeDef* gpioReg, uint8_t pin){
    uint32_t tmp = gpioReg->PUPDR >>(pin * 2);
    return GpioPuPd(tmp & GPIO_PUPDR_PUPDR0);
  }

  static inline GpioSpeed getSpeed(GPIO_TypeDef* gpioReg, uint8_t pin){
    uint32_t tmp = gpioReg->OSPEEDR >>(pin * 2);
    return GpioSpeed(tmp & GPIO_OSPEEDER_OSPEEDR0);
  }

  static inline GpioOType getOType(GPIO_TypeDef* gpioReg, uint8_t pin){
    uint32_t tmp = gpioReg->OTYPER >> pin;
    return GpioOType(tmp & GPIO_OTYPER_OT_0);
  }

  static inline void setup(GPIO_TypeDef* gpioReg, uint8_t pin, GpioMode mode, GpioOType oType, GpioPuPd pupd, GpioSpeed speed){
    setMODE(gpioReg, pin, mode);
    setOType(gpioReg, pin, oType);
    setPullUpDn(gpioReg, pin, pupd);
    setSpeed(gpioReg, pin, speed);
  }

  static inline void setOutputUp(GPIO_TypeDef* gpioReg, uint8_t pin){
    gpioReg->BSRR = (1<<pin);
  }
  static inline void setOutputDown(GPIO_TypeDef* gpioReg, uint8_t pin){
    gpioReg->BSRR = (1<<(pin + 16l));
  }

  static inline void setOutput(GPIO_TypeDef* gpioReg, uint8_t pin, bool newstate){
    newstate ? setOutputUp(gpioReg, pin) : setOutputDown(gpioReg, pin);
  }

  static inline bool getOutput(GPIO_TypeDef* gpioReg, uint8_t pin){
    return (gpioReg->ODR & (1<<pin));
  }

  static inline void toggleOutput(GPIO_TypeDef* gpioReg, uint8_t pin){
    getOutput(gpioReg, pin) ? setOutputDown(gpioReg, pin) : setOutputUp(gpioReg, pin);
  }


  static inline bool getInput(GPIO_TypeDef* gpioReg, uint8_t pin){
    return (gpioReg->IDR & (1<<pin));
  }

  static void setupFromClone(GPIO_TypeDef* gpioRegDest, uint8_t pinDest, GPIO_TypeDef* gpioRegSrc, uint8_t pinSrc){
    setMODE(gpioRegDest, pinDest, getMode(gpioRegSrc, pinSrc));
    setOType(gpioRegDest, pinDest, getOType(gpioRegSrc, pinSrc));
    setPullUpDn(gpioRegDest, pinDest, getPull(gpioRegSrc, pinSrc));
    setSpeed(gpioRegDest, pinDest, getSpeed(gpioRegSrc, pinSrc));
  }

  static bool setAlternateFunc(GPIO_TypeDef* gpioReg, uint8_t pin, uint8_t alternateFunction){
    if (alternateFunction > 7) return false;
    constexpr uint32_t BitMask = 0b00111; // najstarszy zarezerwowany
    uint32_t val = alternateFunction;
    if (pin < 8){
      uint8_t offset = uint8_t(pin * 4);
      uint32_t tmp = gpioReg->AFR[0];
      tmp &= ~(BitMask << offset);
      tmp |= val << offset;
      gpioReg->AFR[0] = tmp;
    }
    if ((pin >= 8)&&(pin < 16)){
      uint8_t offset = uint8_t((pin - 8) * 4);
      uint32_t tmp = gpioReg->AFR[1];
      tmp &= ~(BitMask << offset);
      tmp |= val << offset;
      gpioReg->AFR[1] = tmp;
    }
    return true;
  }

  static inline uint8_t getAlternativeFunc(GPIO_TypeDef* gpioReg, uint8_t pin){
    constexpr uint32_t BitMask = 0b00111;
    uint32_t tmp = 0;
    if (pin < 8){
      tmp = (gpioReg->AFR[0]) >> (pin * 4);
    }
    if ((pin >= 8)&&(pin < 16)){
      tmp = (gpioReg->AFR[1]) >> ((pin - 8) * 4);
    }
    return uint8_t(tmp  & BitMask);
  }


  //***********************************************
  // Methods for Gpio object

  inline void setMODE(const GpioMode mode){
    setMODE(gpio, pinNr, mode);
  }

  inline void setSpeed(const GpioSpeed speed){
    setSpeed(gpio, pinNr, speed);
  }

  inline void setPullUpDn(const GpioPuPd pupd){
    setPullUpDn(gpio, pinNr, pupd);
  }

  /* Output mode configuration */
  inline void setOType(const GpioOType otype){
    setOType(gpio, pinNr, otype);
  }


  // getting gpio registers

  inline GpioMode getMode(){
    return getMode(gpio, pinNr);
  }

  inline GpioPuPd getPull(){
    return getPull(gpio, pinNr);
  }

  inline GpioSpeed getSpeed(){
    return getSpeed(gpio, pinNr);
  }

  inline GpioOType getOType(){
    return getOType(gpio, pinNr);
  }

  inline void setOutputUp(){
    setOutputUp(gpio, pinNr);
  }

  inline void setOutputDown(){
    setOutputDown(gpio, pinNr);
  }

  inline void setOutput(bool newstate){
    setOutput(gpio, pinNr, newstate);
  }

  inline bool getOutput(){
    return getOutput(gpio, pinNr);
  }

  inline void toggleOutput(){
    toggleOutput(gpio, pinNr);
  }

  inline bool getInput(){
    return getInput(gpio, pinNr);
  }


  inline void setup(GpioMode mode, GpioOType oType, GpioPuPd pupd, GpioSpeed speed){
    setMODE(mode);
    setOType(oType);
    setPullUpDn(pupd);
    setSpeed(speed);
  }

  void setupFromClone(Gpio * dolly){
    setupFromClone(gpio, pinNr, dolly->gpio, dolly->pinNr);
  }

  bool setAlternateFunc(uint8_t alternateFunction){
    return setAlternateFunc(gpio, pinNr, alternateFunction);
  }

  inline uint8_t getAlternativeFunc(){
    return getAlternativeFunc(gpio, pinNr);
  }

  inline void mutate(GPIO_TypeDef * gpioDef, uint8_t pin_Nr){
    gpio = gpioDef;
    pinNr = pin_Nr;
  }


  Gpio(GPIO_TypeDef * gpioDef, uint8_t pin_Nr)noexcept : gpio(gpioDef), pinNr(pin_Nr) {

    setup(GpioMode::ANALOG, GpioOType::OpenDrain, GpioPuPd::NoPull, GpioSpeed::LowSpeed);
  }

  Gpio(GPIO_TypeDef * gpioDef, const uint8_t pin_Nr, GpioMode mode, GpioOType oType, GpioPuPd pupd, GpioSpeed speed){
    gpio = gpioDef;
    pinNr = pin_Nr;
    setup(mode, oType, pupd, speed);
  }


};

//} /* namespace STM32F0xx */

//----------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------

//----------------------------------------------------
#ifdef __cplusplus
}
#endif
//----------------------------------------------------



#endif /* GPIO_H_ */
