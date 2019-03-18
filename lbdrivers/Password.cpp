/*
 * Password.cpp
 *
 *  Created on: 26 lut 2019
 *      Author: lechu
 */

#include <Password.h>

const Keyboard::Key Password::servicePassword[Password::ServicePassLength] =  {
      Keyboard::Key::CANCEL, Keyboard::Key::CANCEL,
      Keyboard::Key::LEFT, Keyboard::Key::LEFT,
      Keyboard::Key::RIGHT, Keyboard::Key::RIGHT,
      Keyboard::Key::ENTER, Keyboard::Key::ENTER,
  };
