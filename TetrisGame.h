/*******************************************************************************
  @file     TetrisGame.h
  @brief    files with the game
  @author   Grupo 2 - Lab de Micros
 ******************************************************************************/

#ifndef SOURCES_TETRIS_H_
#define SOURCES_TETRIS_H_


typedef enum {
  EASY,
  MEDIUM,
  HARD
} TETRIS_LEVEL;

/*
 * @brief Function that is called when you want to start the game
 */

void TETRIS_Start(void);

/*
 * @brief The state machine that controlls the game
 * @return GAME_RUNNING or GAME_OVER according to the state of the game
 */
//*
int TETRIS_Run(void);

/*
 * @brief getter
 * @return return the level of the game with the enum type TETRIS_LEVEL
 */
//*
TETRIS_LEVEL getLevel(void);

#endif /* SOURCES_TETRIS_H_ */             