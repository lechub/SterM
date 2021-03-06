/*
 * Hardware.cpp
 *
 *  Created on: 28.06.2017
 *      Author: lechu
 */

#include "Hardware.h"
#include "Gpio.h"
#include "Pomiar.h"
#include "Fifo.h"
#include "QuickTask.h"

#ifndef DEBUG
#define WATCHDOG_ENABLE 1
#endif



/*
+=============================================================================+
| local functions
+=============================================================================+
 */



/*------------------------------------------------------------------------*//**
 * \brief Configures Flash latency.
 * \details Configures Flash latency (wait-states) which allows the chip to run
 * at higher speeds. The constants used for calculations are valid when chip's
 * voltage is in 2.7V to 3.6V range.
 *
 * \param [in] frequency defines the target frequency of the core
 *//*-------------------------------------------------------------------------*/

static void flash_latency(uint32_t frequency){

#ifdef STM32F072
  uint32_t latency = (frequency <= 24000000) ? 0x00 : 0x01; // do 24MHz latency = 0, powyzej =1
  FLASH->ACR = latency;				// set wait_states, disable prefetch
  FLASH->ACR = FLASH_ACR_PRFTBE | latency;	  // set latency, enable prefetch
#endif
#ifdef STM32F4xx
  uint32_t wait_states;
  wait_states = frequency / 30000000ul; // calculate wait_states (30M is valid for 2.7V to 3.6V voltage range, use 24M for 2.4V to 2.7V, 18M for 2.1V to 2.4V or 16M for  1.8V to 2.1V)
  wait_states &= 7;           // trim to max allowed value - 7

  FLASH->ACR = wait_states;       // set wait_states, disable all caches and prefetch
  FLASH->ACR = FLASH_ACR_DCRST | FLASH_ACR_ICRST | wait_states; // reset caches
  FLASH->ACR = FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | wait_states;  // enable caches and prefetch
#endif
}



static bool pll_start_lb(uint32_t crystal, uint32_t frequency){
#ifdef stm32F4xx
  uint32_t pllm = 8;
  uint32_t plln = 128;
  uint32_t pllp = 4;

  RCC->CR |=  RCC_CR_HSEON;				// enable HSE clock
  while (!RCC->CR & RCC_CR_HSERDY); // wait for stable clock

  flash_latency(frequency);				// configure Flash latency for desired frequency

  RCC->PLLCFGR = (pllm << RCC_PLLCFGR_PLLM_bit) | (plln << RCC_PLLCFGR_PLLN_bit) | ((pllp / 2 - 1) << RCC_PLLCFGR_PLLP_bit) | RCC_PLLCFGR_PLLQ_DIV9 | RCC_PLLCFGR_PLLSRC;	// configure PLL factors, always divide USB clock by 9

  // prescalery - wszystkie x1
  RCC->CFGR = RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV1 | RCC_CFGR_HPRE_DIV1;	// AHB - no prescaler, APB1 - divide by 1, APB2 - divide by 1

  while (!RCC->CR & RCC_CR_HSERDY);// RCC_CR_HSERDY_bb);				// wait for stable clock

  //RCC_CR_PLLON_bb = 1;					// enable PLL
  RCC->CR |=  RCC_CR_PLLON;

  //while (!RCC_CR_PLLRDY_bb);				// wait for PLL lock
  while (!RCC->CR & RCC_CR_PLLRDY);				// wait for PLL lock

  RCC->CFGR |= RCC_CFGR_SW_PLL;			// change SYSCLK to PLL
  while (((RCC->CFGR) & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);	// wait for switch

#endif
#ifdef STM32F072
  //	uint32_t pllm = 8;
  //	  uint32_t plln = 128;
  //	  uint32_t pllp = 4;

  RCC->CR |= RCC_CR_HSION;       // enable HSI clock
  while (!(RCC->CR & RCC_CR_HSIRDY)); // wait for stable HSI clock

  RCC->CFGR &= (~RCC_CFGR_SW); // HSI as system clock
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);  // wait for HSI as system clock

  RCC->CR |=  RCC_CR_HSEON;       // enable HSE clock
  while (!(RCC->CR & RCC_CR_HSERDY)); // wait for stable HSE clock

  flash_latency(frequency);       // configure Flash latency for desired frequency

  RCC->CFGR = RCC_CFGR_PLLNODIV // nie dziel PLL /2
      | RCC_CFGR_MCO_PRE_1        // MCO is divided by 1
      | RCC_CFGR_MCO_NOCLOCK      // Clock output disabled
      | RCC_CFGR_PLLMUL6          // PLL input clock x 6
      | RCC_CFGR_PLLXTPRE_PREDIV1 // PREDIV input clock not divided
      | RCC_CFGR_PLLSRC_HSE_PREDIV  // HSE/PREDIV selected as PLL input clock
      | RCC_CFGR_HPRE_DIV1  // SYSCLK not divided to HCLK
      | RCC_CFGR_PPRE_DIV1  // HCLK not divided to APB
      | RCC_CFGR_SW_HSI     // ciagle taktowanie z HSI
      ;
  RCC->CFGR2 = RCC_CFGR2_PREDIV1_DIV1;  // PREDIV input clock not divided

  RCC->CR |=  RCC_CR_PLLON;     // turn PLL on
  while (!(RCC->CR & RCC_CR_PLLRDY));       // wait for PLL lock

  RCC->CFGR |= RCC_CFGR_SW_PLL; // change SYSCLK to PLL
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // wait for PLL as system clock


  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while (((RCC->CFGR) & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // wait for switch

  RCC->CFGR &= RCC_CR_HSION;    // wylaczenie HSI

#endif

  return true;

}


/*------------------------------------------------------------------------*//**
 * \brief Enables FPU
 * \details Enables FPU in Cortex-M4 for both privileged and user mode. This is
 * done by enabling CP10 and CP11 coprocessors in CPACR register (possible only
 * when in privileged mode).
 *//*-------------------------------------------------------------------------*/

//static void fpu_enable(void)
//{
//#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
//	SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));	// set CP10 and CP11 Full Access
//#endif
//}


static void rccInit(void){
  // reset gpio's i obu DMA
  //	RCC->AHB1RSTR = RCC_AHB1RSTR_GPIOARST | RCC_AHB1RSTR_GPIOBRST
  //			|RCC_AHB1RSTR_GPIOCRST | RCC_AHB1RSTR_GPIODRST
  //			|RCC_AHB1RSTR_GPIOERST | RCC_AHB1RSTR_GPIOHRST
  //			|RCC_AHB1RSTR_DMA1RST | RCC_AHB1RSTR_DMA2RST
  //			;

  __IO uint32_t tmpreg = 0x00U;

  do {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN
        |RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN
        |RCC_AHBENR_GPIOEEN | RCC_AHBENR_GPIOFEN
        //	|RCC_AHB1ENR_DMA1EN
        //  | RCC_AHB1ENR_DMA2EN
        ;
    tmpreg = RCC->AHBENR;
    (void)(tmpreg);
  } while(0U);


  do {
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
    /* Delay after an RCC peripheral clock enabling */
    tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
    (void)(tmpreg);
  } while(0U);


  do {
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN
        ;
    /* Delay after an RCC peripheral clock enabling */
    tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN);
    (void)(tmpreg);
  } while(0U);

}

void Hardware::init(){

  // Procesor i zegar zainicjowany

  //flash_latency(32000000);
  pll_start_lb(8000000, 32000000);
  //fpu_enable();

  SystemCoreClockUpdate();	// na wszelki wypadek

  rccInit();

  if (SysTick_Config( SystemCoreClock / SYSTICK_FREQUENCY_HZ)) {
    errorDispatch(Hardware::ErrorCode::Failure);
  }

  NVIC_init();	// przerwania

  //gpioInit();

  // adcInit();

  //i2cInit();

  WDOG_Init();


}

/*------------------------------------------------------------------------*//**
 * \brief GPIO initialization.
 * \details Enables all GPIO ports.
 *//*-------------------------------------------------------------------------*/

//void Hardware::gpioInit(){
//
//	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN |
//			RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN |RCC_AHB1ENR_GPIOHEN
//			;			// enable all possible GPIO ports
//
//}

//I2C_TypeDef * i2c = I2C1;

//	IN0	PC0	reg.pok.		ADC_1_10
//	IN1	PC3	czujnikPodaj.	ADC_1_13
//	IN2	PC1	czujnikCWU		ADC_1_11
//	IN3	PC2	CzujnikCo		ADC_1_12
//	IN4	PA4	termik			ADC_1_4
//	IN5	PA1	NC				ADC_1_1
//	in6	Temp_uP				ADC_1_IN18

Gpio gpioPomSygnL = Gpio(GPIOC, 0);
Gpio gpioPomZas24V = Gpio(GPIOC, 1);


//void Hardware::adcInit(){
//
//  const uint32_t dataLength = Pomiar::Analogi::count;
//  volatile uint16_t *dataPtr = Pomiar::getDataTablePtr();
//
//  // ----------GPIO ---------------
//
//  gpioPomSygnL.setup(Gpio::GpioMode::ANALOG, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
//  gpioPomZas24V.setup(Gpio::GpioMode::ANALOG, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
////  analog2.setup(Gpio::GpioMode::ANALOG, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
////  analog3.setup(Gpio::GpioMode::ANALOG, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
////  analog4.setup(Gpio::GpioMode::ANALOG, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
////  analog5.setup(Gpio::GpioMode::ANALOG, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
//
//  // ---------- RCC ---------------
//
//  // wlaczenie zegara dla DMA2
////  RCC->AHBENR |= RCC_AHBENR_DMA2EN;
//
//
//  // ----------DMA ---------------
//  /* Disable the peripheral */
//
//  DMA2_Stream0->CR  &=  ~(DMA_SxCR_EN);   // wylaczenie DMA
//  while(DMA2_Stream0->CR & DMA_SxCR_EN){;}	// odczekanie na wylaczenie
//
//
//  DMA2_Stream0->CR  &= (uint32_t)(~DMA_SxCR_DBM);		// reset double buffer
//
//  //	   /* Peripheral to Memory */
//  DMA2_Stream0->PAR = uint32_t(&(ADC1->DR));	//Peripheral
//
//  //	     /* Configure DMA Stream source address */
//  DMA2_Stream0->M0AR = (uint32_t)dataPtr;
//
//  /* Configure DMA Stream data length */
//  DMA2_Stream0->NDTR = dataLength;
//
//  DMA2_Stream0->CR  = 0ul
//      | DMA_SxCR_MSIZE_0					// 16 bit mem transfer
//      | DMA_SxCR_PSIZE_0					// 16 bit periph
//      | DMA_SxCR_MINC					// memory incremented
//      | DMA_SxCR_CIRC					// circular
//      //| DMA_SxCR_DIR_					// periph to mem
//      //| DMA_SxCR_PFCTRL					// no periph flow controler
//      //| DMA_SxCR_TCIE					// transfer complete interrupt
//      | DMA_SxCR_DMEIE					// direct mode error interrupt
//      | DMA_SxCR_TEIE					// transfer error interrupt
//      ;		//
//
//
//  // kasowanie zdarzen
//  uint32_t lisr = DMA2->LISR;
//  if (lisr != 0) DMA2->LIFCR = lisr;
//  uint32_t hisr = DMA2->HISR;
//  if (hisr != 0) DMA2->HIFCR = hisr;
//
//
//  //	/* Enable all interrupts */
//  //	DMA2_Stream0->CR  |= 0l
//  //			| DMA_SxCR_TCIE 		// transfer complete
//  //			| DMA_SxCR_HTIE 		// half transfer
//  //			| DMA_SxCR_TEIE 		// transfer error
//  //			| DMA_SxCR_DMEIE;		//
//  //	//	  DMA2_Stream0->CR |= DMA_SxCR_HTIE | DMA_SxCR_TCIE | DMA_SxCR_TEIE | DMA_SxCR_DMEIE;
//  //	//	  DMA2_Stream0->FCR |= DMA_SxFCR_FEIE; //DMA_IT_FE;
//
//
//  /* Enable the Peripheral */
//  DMA2_Stream0->CR |=  DMA_SxCR_EN;	// poszly konie
//
//  //  -------------------------------------
//
//
//
//  //  ---------------- ADC ------------------
//
//  // wlaczenie zegara dla ADC
//  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
//
//  /* Set ADC parameters */
//  /* Set the ADC clock prescaler */
//  ADC->CCR |=
//      ADC_CCR_TSVREFE |							// Temperature sensor enabled
//      ADC_CCR_ADCPRE_0 | ADC_CCR_ADCPRE_1			// prescaler PCLK2 /8 -> ADCCLK = 32MHz/8 = 4MHz
//
//      ;
//
//  /* Set ADC scan mode */
//  ADC1->CR1 |= ADC_CR1_SCAN
//      //			  | ADC_CR1_RES_			// RES = 12bit 15ADCCLK,
//      // | ADC_CR1_EOCIE			// Interrupt enable on end of conversion
//      ;
//
//  // Sample time register
//  // Tconv = 56 + 12 cycles = 68 AdcCycles -> 68/4MHz = 17us
//  constexpr uint32_t SMPR56_cycles =  ADC_SMPR1_SMP10_1 | ADC_SMPR1_SMP10_0;	// 56 cycles /sampling time na wszystkich kanalach
//  constexpr uint32_t SMPR84_cycles =  ADC_SMPR1_SMP10_2;
//  constexpr uint32_t SMPR112_cycles =  ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_0;
//  constexpr uint32_t SMPR144_cycles =  ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_1;
//  constexpr uint32_t SMPR480_cycles =  ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_1 | ADC_SMPR1_SMP10_0;
//
//  uint32_t smp =  SMPR480_cycles;
//  uint32_t val = 0;
//  for (int i = 0 ; i < 9; i++){
//    val |= smp << (3 * i);
//  }
//  ADC1->SMPR1 = val;
//
//  val |= smp << (3 * 9);
//  ADC1->SMPR2 = val;		// 56 cycles /sampling time na wszystkich kanalach
//
//  // ilosc wejsc
//  ADC1->SQR1 |= (dataLength -1) << 20U;		// 7 - ilosc sekwencji
//
//  //	IN0	PC0	reg.pok.		ADC_1_10
//  //	IN1	PC3	czujnikPodaj.	ADC_1_13
//  //	IN2	PC1	czujnikCWU		ADC_1_11
//  //	IN3	PC2	CzujnikCo		ADC_1_12
//  //	IN4	PA4	termik			ADC_1_4
//  //	IN5	PA1	NC				ADC_1_1
//  //	in6	Temp_uP				ADC_1_IN18
//
//  ADC1->SQR3 =
//      (10 << (0*5)) |
//      (13 << (1*5)) |
//      (11 << (2*5)) |
//      (12 << (3*5)) |
//      (4 << (4*5)) |
//      (1 << (5*5))
//      ;
//  ADC1->SQR2 =
//      (18 << (0*5))
//      ;
//
//  /* Set ADC resolution */
//  /* Set ADC data alignment */
//
//
//  /* Enable or disable ADC DMA continuous request */
//  ADC1->CR2 |= 0ul
//      //			  | ADC_CR2_ALIGN
//      | ADC_CR2_EOCS	// EOC ustawione po kazdej konwersji, Overrun wlaczone
//      | ADC_CR2_DMA 	// lecimy DMA - pozniej
//      | ADC_CR2_DDS 	// wznawiamy DMA po ostatniej konwersji
//      | ADC_CR2_CONT
//      | ADC_CR2_ADON		// ADC wlaczone
//      ;
//
//
//  /* Delay for ADC stabilization time */
//  /* Compute number of CPU cycles to wait for */
//  __IO uint32_t counter = (3U * (SystemCoreClock / 1000000U));
//  while(counter != 0U)
//  {
//    counter--;
//  }
//
//
//  //	ADC1->SR &= ~ADC_SR_EOC;
//
//
//  ADC1->CR2 |= ADC_CR2_ADON;
//
//  // przerwania NVIC
//  NVIC_SetPriority(IRQn_Type::DMA2_Stream0_IRQn, NVIC_EncodePriority(Hardware::nvicPriority, 0,0));
//  NVIC_EnableIRQ(IRQn_Type::DMA2_Stream0_IRQn);
//
//  NVIC_SetPriority(IRQn_Type::ADC_IRQn, NVIC_EncodePriority(Hardware::nvicPriority, 0,0));
//  NVIC_EnableIRQ(IRQn_Type::ADC_IRQn);
//
//
//  ADC1->CR2 |= ADC_CR2_SWSTART;
//
//}
//

#define IWDG_KEY_RELOAD                 0x0000AAAAU  /*!< IWDG Reload Counter Enable   */
#define IWDG_KEY_ENABLE                 0x0000CCCCU  /*!< IWDG Peripheral Enable       */
#define IWDG_KEY_WRITE_ACCESS_ENABLE    0x00005555U  /*!< IWDG KR Write Access Enable  */
#define IWDG_KEY_WRITE_ACCESS_DISABLE   0x00000000U  /*!< IWDG KR Write Access Disable */

#define IWDG_PRESCALER_4                0x00000000U                   /*!< IWDG prescaler set to 4   */
#define IWDG_PRESCALER_8                IWDG_PR_PR_0                  /*!< IWDG prescaler set to 8   */
#define IWDG_PRESCALER_16               IWDG_PR_PR_1                  /*!< IWDG prescaler set to 16  */
#define IWDG_PRESCALER_32               (IWDG_PR_PR_1 | IWDG_PR_PR_0) /*!< IWDG prescaler set to 32  */
#define IWDG_PRESCALER_64               IWDG_PR_PR_2                  /*!< IWDG prescaler set to 64  */
#define IWDG_PRESCALER_128              (IWDG_PR_PR_2 | IWDG_PR_PR_0) /*!< IWDG prescaler set to 128 */
#define IWDG_PRESCALER_256              (IWDG_PR_PR_2 | IWDG_PR_PR_1) /*!< IWDG prescaler set to 256 */

void Hardware::WDOG_Init(){
#ifdef WATCHDOG_ENABLE
  // Enable write access to IWDG_PR and IWDG_RLR registers by writing 0x5555 in KR
  IWDG->KR = IWDG_KEY_WRITE_ACCESS_ENABLE;

  // Write to IWDG registers the Prescaler & Reload values to work with
  IWDG->PR = IWDG_PRESCALER_256;		// 32kHz/256 = 125Hz == 8ms
  IWDG->RLR = 125 * 8 * 2;	// 2 sekundy

  // Enable IWDG. LSI is turned on automaticaly
  IWDG->KR =  IWDG_KEY_ENABLE;	// aktywacja IWDG
#endif	//WATCHDOG_ENABLE
}

void Hardware::WDOG_Reload(){
#ifdef WATCHDOG_ENABLE
  IWDG->KR =  IWDG_KEY_RELOAD;	// przeladowanie IWDG
#endif	//WATCHDOG_ENABLE
}



void Hardware::NVIC_init(){
  //NVIC_SetPriorityGrouping(nvicPriority);

}

void Hardware::delayMsWithBackground(uint32_t milis){
    QuickTask::delayMsWithActiveTasks(milis);

}

void Hardware::delayMsDirty(uint32_t milis){
  QuickTask::delayMsWithStoppedTasks(milis);
}


bool Hardware::adcDmaError  = false;

//void DMA2_Stream0_IRQHandler(void){
//  static volatile uint32_t dmairq = 1;
//  dmairq++;
//  // kasowanie zdarzen
//  uint32_t lisr = DMA2->LISR;
//  if (lisr != 0) DMA2->LIFCR = lisr;
//  uint32_t hisr = DMA2->HISR;
//  if (hisr != 0) DMA2->HIFCR = hisr;
//
//  // if (lisr & DMA_LISR_TCIF0) ; //  transfer complete
//  // if (lisr & DMA_LISR_HTIF0) ; // half transfer
//  if (lisr & DMA_LISR_DMEIF0) Hardware::errorDispatch(Hardware::ErrorCode::DMA_FAIL);	// transfer error
//  if (lisr & DMA_LISR_FEIF0) Hardware::errorDispatch(Hardware::ErrorCode::DMA_FAIL);	// direct mode error
//}
//
//void DMA1_Stream0_IRQHandler(void){
//  static volatile uint32_t dmairq = 1;
//  dmairq++;
//}

//void ADC_IRQHandler(void){
//  static volatile uint32_t adcirq = 1;
//  adcirq++;
//  uint32_t sr = ADC1->SR;
//  if (sr & ADC_SR_EOC){;}		// end of channel conversion
//  if (sr & ADC_SR_OVR) {
//    ADC1->SR &= ~ ADC_SR_OVR;
//    Hardware::errorDispatch(Hardware::ErrorCode::ADC_FAIL);	// direct mode error
//  }
//  if (sr & ADC_SR_AWD) {;}	// analog Watchdog event
//}



