#include "TetrisGame.h"
//#include "gpio.h"
//#include "button.h"




/* Función que se llama 1 vez, al comienzo del programa */
void main(void)
{
  //buttonsInit();
  //buttonConfiguration(PIN_SW2, LKP, 20); //20*50=1seg
  //buttonConfiguration(PIN_SW3, LKP, 20); //20*50=1seg
  TETRIS_Start();
  while(1)
  {
    TETRIS_Run();
  }
}

/* Función que se llama constantemente en un ciclo infinito */
// void App_Run(void)
// {
//   TETRIS_Run();
// }
