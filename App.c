#include "TetrisGame.h"
//#include "gpio.h"
//#include "button.h"
#include <time.h> 

  
void delay(int number_of_mseconds) 
{ 
    // Storing start time 
    clock_t start_time = clock(); 
  
    // looping till required time is not achieved 
    while (clock() < start_time + number_of_mseconds) 
        ; 
} 


/* Función que se llama 1 vez, al comienzo del programa */
void main(void)
{
  //buttonsInit();
  //buttonConfiguration(PIN_SW2, LKP, 20); //20*50=1seg
  //buttonConfiguration(PIN_SW3, LKP, 20); //20*50=1seg
  TETRIS_Start();
  while(1)
  {
    delay(100); 
    TETRIS_Run();
  }
}

/* Función que se llama constantemente en un ciclo infinito */
// void App_Run(void)
// {
//   TETRIS_Run();
// }
