
/*
 * QuickTask.h
 *
 *  Created on: 23 sty 2014
 *      Author: lechu
 */

#ifndef QUICKTASK_H_
#define QUICKTASK_H_

#include <stdint.h>

/**
 * Klasa pozwalająca na uruchamianie podprocedur (tasków) na kilka sposobów.
 * Do działania wymaga przerwania SysTick, w którym tylko inkrementuje licznik czasu.
 * W pętli głównej programu należy umieścić funkcję statyczną poll()
 * i wywoływać ją z możliwie największa częstotliwością.
 * funkcja poll() sprawdza stan każdego ze zdefiniowanych tasków,
 * aktualizauje jego stan oraz uruchamia powiązaną z nim funkcję - w razie potrzeby.
 * Zdefiniować można taski następujących typów:
 * - QT_TIMER - tylko odmierza czas, po jego upływie jest zatrzymany.
 *            Można go użyć jako czasowmierza, którego stan jest zwracany przez metodę is_stopped()
 * - QT_ONCE  - po upływie zdefiniowanego czasu zostanie uruchomiona powiązana funkcja.
 * - QT_PERIODIC - powiązana funkcja będzie wywoływana cyklicznie co określony interwał.
 */
class QuickTask {
public:
  /** Typy tasków QuickTask */
  typedef enum {
    QT_TIMER,	// tylko odmierza czas - trzeba sprawdzic przez is_stopped()
    QT_ONCE,		// po okreslonym czasie wykona QuickTask jednokrotnie.
    QT_PERIODIC	// raz puszczony, puszcza sie sam co okreslony czas
  }QTType;

  /** Aktualny status w jakim może znajdować się task */
  typedef enum qtFlags{
    QT_DISABLED,				// nieaktywny
    QT_STOPPED,		// ukonczyl odliczanie
    QT_RUNNING,	// odlicza
  }QTFlags;

  /** interwał dla wywołania przerwania SysTick   */
  static constexpr uint32_t DEFAULT_TICK_INTERVAL_MILIS = 1;

private:
  /** licznik statyczny, aktualizowany przez Systick.
   * Do niego są porównywane wszystkie taski */
  static volatile uint32_t milisCounter;

  /** znacznik globalnego blokowania tasków. */
  static volatile bool holded;

  /** licznik obiektu QuickTask. Porównywany z licznikiem głównym 'milisCounter' w metodzie  'poll()'.
   * Gdy timectr == 0, to licznik jest zatrzymany - np. gdy odliczył czas.*/
  volatile uint32_t timectr;

  /** Wartośc interwału dla taska typu QT_PERIODIC.
   * Wartość jest wpisywana do 'milisCounter' po każdym odliczeniu do zera */
  uint32_t	updateValue;

  /** Element listy jednokierunkowej tasków. Wskaźnik do kolejnego obiektu QuickTask lub nullptr */
  QuickTask *next;

  /** Wskaźnik do funkcji powiązanej z taskiem.
   */
  void  (*qCallback)(void);

  /** Typ tasku. Wymagany przez konstruktor.  */
  QTType	type;

  /** Aktualny stan tasku.	 */
  QTFlags	flags;

public:
  /**
   * Jedyna metoda wywoływana w przerwaniu Systick. Inkrementuje licznik 'milisCounter'
   * @param milis Ilość milisekund od ostatniego wywołania tick();
   */
  static void tick(uint32_t milis);

  /**
   * Metoda zarządzajaca taskami. Sprawdza i uaktualnia ich stan,
   * uruchamia powiązane funkcje.
   */
  static void poll();

  /** Można użyć do odliczania czasu.
   * @return Zwraca tylko wartość licznika 'milisCounter'.
   */
  static uint32_t getCounter(){ return milisCounter; }

  /**
   * Konstruktor obiektu QuickTask. Inicjuje zmienne obiektu, oraz dopisuje obiekt
   * do listy tasków.
   * Sposób użycia:
   * QuickTask userQTask(QuickTask::QT_PERIODIC, funcName, 100);
   *
   * @param typ wymagany typ tasku
   * @param task  wskaźnik do funkcji bezargumentowej, zwracającej typ void.
   *    Funkcja będzie powiązana z taskiem i wywoływana w określonych warunkach.
   * @param updateMilis Wymagany czas lub interwał wywołania funkcji powiązanej.
   */
  QuickTask(QTType typ, void (*const task)(), const uint32_t updateMilis);

  /** Sprawdza czy task jest uruchomiony */
  uint8_t is_stopped(){return (timectr == 0)? true: false;}

  /** Zatrzymuje i deaktywuje task */
  void disable(){	flags = QuickTask::QT_DISABLED;	timectr = 0;}

  /**
   * Ustawia licznik timera na określoną wartość i uruchamia task.
   * Po odliczeniu czasu powiązana funkcja będzie uruchomiona.
   * @param mili milisekundy
   */
  void set_milis(const uint32_t mili){
    timectr = mili;
    flags = QuickTask::QT_RUNNING;}

  /**
   * Ustawia licznik timera na określoną wartość i uruchamia task.
   * Po odliczeniu czasu powiązana funkcja będzie uruchomiona.
   * @param sek sekundy
   */
  void set_seconds(const uint8_t sek){set_milis(sek*1000); }

  /**
   * Ustawia licznik timera na określoną wartość i uruchamia task.
   * Po odliczeniu czasu powiązana funkcja będzie uruchomiona.
   * @param min
   */
  void set_minutes(const uint8_t min){set_milis(min*1000*60); }

  /**
   * Ustawia licznik timera na określoną wartość i uruchamia task.
   * Po odliczeniu czasu powiązana funkcja będzie uruchomiona.
   * @param godz
   */
  void set_hours(const uint8_t godz){set_milis(godz*1000*60*60); }

  /**
   * Wartość licznika dla tasku, czyli jego ilość milisekund do odliczenia
   * @return ilość milisekund do odliczenia
   */
  uint32_t get_milis(){return timectr;}

  /**
   * Funkcja do zmiany interwału pomiędzy uruchomieniami tasku w trybie QT_PERIODIC.
   * @param updateMs milisekundy
   */
  void setIntervalMs(uint32_t updateMs){updateValue = updateMs;}

  /**
   *
   */
  void setCallback(void (*callback)()){
    qCallback = callback;
  }

  /**
   * Zwraca ilosc milisekund od oldTime do teraz. oldTime musi byc w przeszlosci.
   * @param oldTime
   * @return różnica czasu w milisekundach
   */
  static uint32_t getTimeIntervalMilis(uint32_t oldTime){
    uint32_t cnt = getCounter();
    return uint32_t(cnt - oldTime);
  }

  /**
   * Sprawdza czy od pewnego czasu w przeszłości upłynął już określony okres.
   * @param oldTime czas w przeszłości
   * @param interval wymagany okres
   * @return
   */
  static uint32_t isOlderThanMs(uint32_t oldTime, uint32_t interval){
    return getTimeIntervalMilis(oldTime) > interval;
  }

  /**
   * Aktywne oczekiwanie przez określony czas. W tle wykonują się wszystkie taski.
   * MUSI działać przerwanie Systick
   * @param milis Milisekundy do odczekania
   */
  static void delayMsWithActiveTasks(uint32_t milis){
    volatile uint32_t tmp = getCounter();
    while(getCounter() - tmp < milis ){
      QuickTask::poll();
    }
  }

  /**
   * Aktywne oczekiwanie przez określony czas.
   * W tym czasie wszystkie taski są zamrożone.
   * MUSI działać przerwanie Systick
   * @param milis Milisekundy do odczekania
   */
  static void delayMsWithStoppedTasks(uint32_t milis){
    volatile uint32_t tmp = getCounter();
    while(getCounter() - tmp < milis ){ ; }
  }

  /***
   * Blokuje i odblokowuje wykonywanie powiązanych funkcji dla wszystkich taskow.
   * Liczniki taskow odliczaja dalej.
   * @param enable true - zatrzymuje, false - uruchamia.
   */
  static void hold(bool enable){
    holded = enable;
  }

  static bool isHolded(){
    return holded;
  }

};

/**
 * wskaźnik do pierwszego obiektu QuickTask, lub nullptr - gdy jeszcze nie ma żadnego
 */
extern QuickTask* taskbase;

//----------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------

#ifdef USE_HAL_DRIVER
void HAL_SYSTICK_Callback();
#else
void SysTick_Handler(void) __attribute__ ((interrupt));
#endif

//----------------------------------------------------
#ifdef __cplusplus
}
#endif
//----------------------------------------------------


#endif /* QUICKTASK_H_ */
