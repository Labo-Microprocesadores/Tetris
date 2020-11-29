/*******************************************************************************
  @file     TetrisGame.h
  @brief    files with the game
  @author   Grupo 2 - Lab de Micros
 ******************************************************************************/

#ifndef SOURCES_TETRIS_H_
#define SOURCES_TETRIS_H_


typedef enum {
  EASY = 15,
  MEDIUM = 7,
  HARD = 1
} TETRIS_LEVEL;

/*
 * @brief Function that is called when you want to start the game
 */
void TETRIS_Start(void);


/*
 * @brief The state machine that controlls the game
 * @return GAME_RUNNING or GAME_OVER according to the state of the game
 */
int TETRIS_Run(void);

/*
 * @brief getter
 * @return return the level of the game with the enum type TETRIS_LEVEL
 */
TETRIS_LEVEL getLevel(void);


/*
 * @brief setter
 * @param the level of the game requiered with the enum type TETRIS_LEVEL
 */
void setLevel(TETRIS_LEVEL wantedLevel);

/*
 * @brief getter
 * @return return the points of the user (10 points per line completed)
 */
TETRIS_LEVEL getPoints(void);
#endif /* SOURCES_TETRIS_H_ */             