/*
 * RTClock.h
 *
 *  Created on: 8 lut 2019
 *      Author: lechu
 */

#ifndef RTCLOCK_H_
#define RTCLOCK_H_

#include "systemDefs.h"
#include <ctime>

class RTClock {


private:

  static struct tm czas;

  static bool setTime(uint32_t date, uint32_t time){
    PWR->CR |= PWR_CR_DBP;                // Access to RTC and Backup registers enabled
    // RTC calendar configuration
    RTC->WPR = 0xCA; // (1) Write access for RTC registers
    RTC->WPR = 0x53; // (1)
    RTC->ISR |= RTC_ISR_INIT; // (2) Enable init phase
    while ((RTC->ISR & RTC_ISR_INITF) != RTC_ISR_INITF){;} // (3) Wait until it is allow to modify RTC register values
    RTC->PRER = 0x007F0137; // (4) set prescaler, 40kHz/128 => 312 Hz, 312Hz/312 => 1Hz
    RTC->TR = RTC_TR_PM | time; // (5) New time in TR
    RTC->DR = RTC_TR_PM | date; // (5)
    RTC->CR &= (~RTC_CR_FMT); // 0b0 -> 24-hour format
    RTC->ISR &=~ RTC_ISR_INIT; // (6) Disable init phase
    RTC->WPR = 0xFE; // (7) Disable write access for RTC registers
    RTC->WPR = 0x64; // (7)
    PWR->CR &= ~(PWR_CR_DBP);                // Access to RTC and Backup registers disabled
    return true;
  }

  static bool getTime(uint32_t *date, uint32_t *time){
    if((RTC->ISR & RTC_ISR_RSF) == RTC_ISR_RSF)    {
      *time = RTC->TR; /* get time */
      *date = RTC->DR; /* need to read date also */
      return true;
    }
    return false;
  }

  static bool fixTime(){
    if ((czas.tm_sec < 0) || (czas.tm_sec > 61)) czas.tm_sec = 0;
    if ((czas.tm_min < 0) || (czas.tm_min > 59)) czas.tm_min = 0;
    if ((czas.tm_hour < 0) || (czas.tm_hour > 23)) czas.tm_hour = 0;
    if ((czas.tm_mday < 0) || (czas.tm_mday > 31)) czas.tm_mday = 1;
    if ((czas.tm_mon < 0) || (czas.tm_mon > 11)) czas.tm_mon = 0;
    if ((czas.tm_year < 2015-1900) || (czas.tm_year > 3000)) czas.tm_year = 2015-1900; // od 1900, wiec co najmniej 2015-1900
    if ((czas.tm_wday < 0) || (czas.tm_wday > 6)) czas.tm_wday = 0;
    if ((czas.tm_yday < 0) || (czas.tm_yday > 365)) czas.tm_yday = 0;
    if ((czas.tm_isdst < 0) || (czas.tm_isdst > 365)) czas.tm_isdst = 0;
    return true;
  }


public:



  static bool init(){
    if (!isRTCInitialized()){
      initRTCDomain();
      setDateTime(2019, 01, 01, 12, 00, 00);
    }
  }

  static bool isRTCInitialized(){
    return(RTC->ISR & RTC_ISR_INITS);
  }


  static bool initRTCDomain(){
    // Use Backup registers to count reset events
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;    // Power interface clock enabled
    PWR->CR |= PWR_CR_DBP;                // Access to RTC and Backup registers enabled

    RCC->BDCR = (RCC->BDCR & (~RCC_BDCR_RTCSEL)) | RCC_BDCR_RTCSEL_0; // select LSE as RTC clock
    RCC->BDCR = (RCC->BDCR & (~RCC_BDCR_LSEDRV)) | RCC_BDCR_LSEDRV_1; // select Xtal drive cabability
    RCC->BDCR |= RCC_BDCR_LSEON;      // LSE oscillator on PC14
    RCC->BDCR &= (~RCC_BDCR_LSEBYP);  // LSE oscillator on PC15
    while (!(RCC->BDCR & RCC_BDCR_LSERDY)){;} // wait for LSE ready

    RCC->BDCR |= RCC_BDCR_RTCEN;  // enable RTC

    // Clear reset flags for next reset
    //    RCC->CSR |= RCC_CSR_RMVF;

    PWR->CR &= ~(PWR_CR_DBP);                // Access to RTC and Backup registers disabled

    return true;
  }


  static time_t getDateTimeUTC(){
    uint32_t rtcTR;
    uint32_t rtcDR;
    getTime(&rtcTR, &rtcDR);
    // czas
    czas.tm_hour = 10 * ((rtcTR & RTC_TR_HT) >> 20) + ((rtcTR & RTC_TR_HU) >> 16);  // 2 cyfry BCD to bin
    if (rtcTR & RTC_TR_PM) czas.tm_hour += 12;                                    // jesli format 12-g i PM to dodac 12 godzin
    czas.tm_min = 10 * ((rtcTR & RTC_TR_MNT) >> 12) + ((rtcTR & RTC_TR_MNU) >> 8);  // 2 cyfry BCD to bin
    czas.tm_sec = 10 * ((rtcTR & RTC_TR_ST) >> 4) + ((rtcTR & RTC_TR_SU) >> 0);  // 2 cyfry BCD to bin
    // data
    czas.tm_year = 10 * ((rtcTR & RTC_DR_YT) >> 20) + ((rtcTR & RTC_DR_YU) >> 16);  // 2 cyfry BCD to bin
    czas.tm_year += 2000 - 1900;  // liczony od 1900 r.
    czas.tm_mon = 10 * ((rtcTR & RTC_DR_MT) >> 12) + ((rtcTR & RTC_DR_MU) >> 8);  // 2 cyfry BCD to bin
    czas.tm_mon -= 1;           // miesiac liczony od 0;
    czas.tm_mday = 10 * ((rtcTR & RTC_DR_DT) >> 4) + ((rtcTR & RTC_DR_DU) >> 0);  // 2 cyfry BCD to bin

    return mktime(&czas);
  }


  static void getTime(){
    time_t timeUTC = getDateTimeUTC();

  }

  static bool setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second){
    setYear(year);
    setMonth(month);
    setDay(day);
    setHour(hour);
    setMinute(minute);
    setSecond(second);

    uint32_t rtcTR;
    uint32_t rtcDR;
    // czas

    rtcTR = (rtcTR & (~(RTC_TR_HT | RTC_TR_HU))) | (((czas.tm_hour / 10) << 20) | ((czas.tm_hour % 10) << 16));
    //     czas.tm_hour = 10 * ((rtcTR & RTC_TR_HT) >> 20) + ((rtcTR & RTC_TR_HU) >> 16);  // 2 cyfry BCD to bin
//    if (rtcTR & RTC_TR_PM) czas.tm_hour += 12;                                    // jesli format 12-g i PM to dodac 12 godzin
    rtcTR = rtcTR & (~(RTC_TR_PM));
    rtcTR = (rtcTR & (~(RTC_TR_MNT | RTC_TR_MNU))) | (((czas.tm_min / 10) << 12) | ((czas.tm_min % 10) << 8));
//    czas.tm_min = 10 * ((rtcTR & RTC_TR_MNT) >> 12) + ((rtcTR & RTC_TR_MNU) >> 8);  // 2 cyfry BCD to bin
    rtcTR = (rtcTR & (~(RTC_TR_ST | RTC_TR_SU))) | (((czas.tm_sec / 10) << 12) | ((czas.tm_sec % 10) << 0));
//    czas.tm_sec = 10 * ((rtcTR & RTC_TR_ST) >> 4) + ((rtcTR & RTC_TR_SU) >> 0);  // 2 cyfry BCD to bin
    // data
    uint32_t tmp = czas.tm_sec + 2000 - 1900;
    rtcTR = (rtcDR & (~(RTC_DR_YT | RTC_DR_YU))) | (((tmp % 10) << 20) | ((tmp / 10) << 16));
//    czas.tm_year = 10 * ((rtcTR & RTC_DR_YT) >> 20) + ((rtcTR & RTC_DR_YU) >> 16);  // 2 cyfry BCD to bin
//    czas.tm_year += 2000 - 1900;  // liczony od 1900 r.
    tmp = czas.tm_mon + 1;
    rtcTR = (rtcDR & (~(RTC_DR_MT | RTC_DR_MU))) | (((tmp % 10) << 12) | ((tmp / 8) << 16));
//    czas.tm_mon = 10 * ((rtcTR & RTC_DR_MT) >> 12) + ((rtcTR & RTC_DR_MU) >> 8);  // 2 cyfry BCD to bin
//    czas.tm_mon -= 1;           // miesiac liczony od 0;
    rtcTR = (rtcDR & (~(RTC_DR_DT | RTC_DR_DU))) | (((tmp % 10) << 4) | ((tmp / 8) << 0));
//    czas.tm_mday = 10 * ((rtcTR & RTC_DR_DT) >> 4) + ((rtcTR & RTC_DR_DU) >> 0);  // 2 cyfry BCD to bin

    return setTime(rtcTR, rtcDR);
  }

  static inline uint8_t getSecond(){return (uint8_t)czas.tm_sec;}
  static inline uint8_t getMinute(){return (uint8_t)czas.tm_min;}
  static inline uint8_t getHour(){return (uint8_t)((czas.tm_hour == 23)? 0 : czas.tm_hour + 1);}
  static inline uint8_t getDay(){return (uint8_t)czas.tm_mday;}
  static inline uint8_t getMonth(){return (uint8_t)(czas.tm_mon + 1);}
  static inline uint16_t getYear(){return (uint16_t)(czas.tm_year + 1900);}
  static inline uint8_t getWeek();

  static inline void setSecond(uint8_t sec){ czas.tm_sec = sec;}
  static inline void setMinute(uint8_t min){ czas.tm_min = min;}
  static inline void setHour(uint8_t hour){ czas.tm_hour = (hour == 0)? 23 : hour -1;}
  static inline void setDay(uint8_t day){ czas.tm_mday = day;}
  static inline void setMonth(uint8_t mon){ czas.tm_mon = mon - 1;}
  static inline void setYear(uint16_t year){ czas.tm_year = year - 1900;}

};

#endif /* RTCLOCK_H_ */
