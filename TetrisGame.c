/*******************************************************************************
  @file     TetrisGame.c
  @brief    files with the game
  @author   Grupo 2 - Lab de Micros
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "TetrisGame.h"
//#include "gpio.h"
//#include "button.h"
#include "termlib.h"
#include "stdint.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
//! medio inutil para nosotros porque va a imprimirse en el display de puntos y no en consola
#define SQU 223 /* character code for a square on terminal */

//! antes eran 20 y 20
#define WIDTH  20 /* Width of play area */
#define HEIGHT 20 /* Height of play area */

//? Es necesario?
#define true 1
#define false 0

#define GAME_OVER 0
#define GAME_RUNNING 1

//* Actions that we can do with the pieces 
typedef enum {
  TETRIS_Action_None,
  TETRIS_Action_MoveLeft,
  TETRIS_Action_MoveRight,
  TETRIS_Action_MoveDown,
  TETRIS_Action_Rotate,
  TETRIS_Action_Drop,
} TETRIS_Action;

typedef struct {
  unsigned char numOfRotates;
  unsigned char currentRotate;
  unsigned char pieceType;
  unsigned char x;
  unsigned char y;
  char attached;
  const unsigned char *shapes;
} piece;

typedef enum {
  NO_TETRIS,
  TETRIS_INIT,
  TETRIS_WAIT_FOR_START,
  TETRIS_START,
  TETRIS_PLAY,
  TETRIS_LOST,
  TETRIS_END
} TETRIS_State;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void SCI_send(const char *str);

static uint8_t SCI_read_nb(void);

static TETRIS_Action read_keypad(void);

static void paintEdges(void);

static void initFramebuffer(void);

static void printFrameBuffer(void);

static void printPiece(piece *p, unsigned char c);

static void eatlines(void);

static char checkAttach(piece *p);

static char lost(piece *p);

static void movePieceLeft(piece *p);

static void movePieceLeft(piece *p);

static char createPiece(piece *p);

static void movePieceRight(piece *p);

static void rotatePiece(piece *p);

void TETRIS_Start(void);

static void PrintWelcome(void);

static TETRIS_Action ReadKey(void);

static unsigned char Play(void);

int TETRIS_Run(void);

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

//* used as delay counter for the piece drop-down
static int frame;     
//* pointer to the current piece
static int piece_ptr; 
//* Board of the game
static unsigned char framebuffer[WIDTH][HEIGHT];



///////////////////////////////////////////////////////////////////////
//*		Pieces 
///////////////////////////////////////////////////////////////////////

static const unsigned char pieces_long[2*4*4] = {
  ' ',SQU,' ',' ',
  ' ',SQU,' ',' ',
  ' ',SQU,' ',' ',
  ' ',SQU,' ',' ',

  ' ',' ',' ',' ',
  SQU,SQU,SQU,SQU,
  ' ',' ',' ',' ',
  ' ',' ',' ',' ',
};

static const unsigned char pieces_three[4*4*4] = {
  ' ',' ',' ',' ',
  ' ',' ',SQU,' ',
  ' ',SQU,SQU,SQU,
  ' ',' ',' ',' ',

  ' ',' ',SQU,' ',
  ' ',SQU,SQU,' ',
  ' ',' ',SQU,' ',
  ' ',' ',' ',' ',

  ' ',' ',' ',' ',
  ' ',SQU,SQU,SQU,
  ' ',' ',SQU,' ',
  ' ',' ',' ',' ',

  ' ',SQU,' ',' ',
  ' ',SQU,SQU,' ',
  ' ',SQU,' ',' ',
  ' ',' ',' ',' ',

};

static const unsigned char pieces_square[1*4*4] = {
  ' ',' ',' ',' ',
  ' ',SQU,SQU,' ',
  ' ',SQU,SQU,' ',
  ' ',' ',' ',' ',
};

static const unsigned char pieces_right[2*4*4] = {
  ' ',' ',' ',' ',
  ' ',SQU,SQU,' ',
  SQU,SQU,' ',' ',
  ' ',' ',' ',' ',

  ' ',' ',' ',' ',
  ' ',SQU,' ',' ',
  ' ',SQU,SQU,' ',
  ' ',' ',SQU,' ',
};

static const unsigned char pieces_left[2*4*4] = {
  ' ',' ',' ',' ',
  ' ',SQU,SQU,' ',
  ' ',' ',SQU,SQU,
  ' ',' ',' ',' ',

  ' ',' ',' ',' ',
  ' ',' ',SQU,' ',
  ' ',SQU,SQU,' ',
  ' ',SQU,' ',' ',
};

static const unsigned char pieces_L1[4*4*4] = {
  ' ',' ',' ',' ',
  ' ',' ',' ',SQU,
  ' ',SQU,SQU,SQU,
  ' ',' ',' ',' ',

  ' ',SQU,SQU,' ',
  ' ',' ',SQU,' ',
  ' ',' ',SQU,' ',
  ' ',' ',' ',' ',

  ' ',' ',' ',' ',
  ' ',SQU,SQU,SQU,
  ' ',SQU,' ',' ',
  ' ',' ',' ',' ',

  ' ',SQU,' ',' ',
  ' ',SQU,' ',' ',
  ' ',SQU,SQU,' ',
  ' ',' ',' ',' ',
};

static const unsigned char pieces_L2[4*4*4] = {
  ' ',' ',' ',' ',
  ' ',SQU,' ',' ',
  ' ',SQU,SQU,SQU,
  ' ',' ',' ',' ',

  ' ',' ',SQU,' ',
  ' ',' ',SQU,' ',
  ' ',SQU,SQU,' ',
  ' ',' ',' ',' ',

  ' ',' ',' ',' ',
  ' ',SQU,SQU,SQU,
  ' ',' ',' ',SQU,
  ' ',' ',' ',' ',

  ' ',SQU,SQU,' ',
  ' ',SQU,' ',' ',
  ' ',SQU,' ',' ',
  ' ',' ',' ',' ',
};

//*Array with the different types of pieces
static piece pieces[7] = {
  {2,0,0,WIDTH/2,1,false,&pieces_long[0]},
  {4,0,1,WIDTH/2,1,false,&pieces_three[0]},
  {1,0,2,WIDTH/2,1,false,&pieces_square[0]},
  {2,0,3,WIDTH/2,1,false,&pieces_right[0]},
  {2,0,4,WIDTH/2,1,false,&pieces_left[0]},
  {4,0,5,WIDTH/2,1,false,&pieces_L1[0]},
  {4,0,6,WIDTH/2,1,false,&pieces_L2[0]},
};
///////////////////////////////////////////////////////////////////////
//*		End of Pieces 
///////////////////////////////////////////////////////////////////////

static unsigned char updateScreen = false;
//* State of the game, at the beginning TETRIS_INIT is set.
static TETRIS_State TETRIS_state = NO_TETRIS;
/*******************************************************************************
 *                        GLOBAL FUNCTION DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 *                       LOCAL FUNCTION DEFINITIONS
 ******************************************************************************/
//! Deberiamos cambiar esta funcion para que mande lo necesario para imprimir al display de ptos 
//! esta funcion se usa en otras funciones como printFrameBuffer
//* Fuction to send the information to the screen
static void SCI_send(const char *str) {
  printf((const char*)str);
}

//! No se bien que hace pero deberiamos modificarlo para lo que necesitamos, se llama para saber los inputs del teclado segun tengo entendido y nosotros no vamos a usar inputs del teclado
static uint8_t SCI_read_nb(void) {
  /* read of character */
  uint8_t c;
  if(kbhit()==1)
    c = getch();
  else
    return '\0';
  return c;
}

// //* Reads the key entered by the user and returns it (if there was no key pressed, returns TETRIS_Action_None)
// static TETRIS_Action read_keypad(void) {
//   bool btn1, btn2;

//   btn1 = wasTap(PIN_SW2);
//   if (btn1) {
//     OSA_TimeDelay(20);
//     btn1 = wasTap(PIN_SW2);
//   }
//   btn2 = wasTap(PIN_SW3);
//   if (btn2) {
//     OSA_TimeDelay(20);
//     btn2 = wasTap(PIN_SW3);
//   }
//   if (btn1 && !btn2) {
//     return TETRIS_Action_MoveLeft;
//   }
//   if (!btn1 && btn2) {
//     return TETRIS_Action_MoveRight;
//   }
//   if (btn1 && btn2) {
//     return TETRIS_Action_Rotate;
//   }
//   return TETRIS_Action_None;
// }

//! Creo que esta funcion no nos interesa (salvo que queramos desperdiciar un borde de ptos)
//* This fuction prints the borders
static void paintEdges(void){
  int x,y;

  for(x=0; x<WIDTH; x++){
    framebuffer[x][0] = SQU;
    framebuffer[x][HEIGHT-1] = SQU;
  }
  for(y=0; y<HEIGHT; y++){
    framebuffer[0][y] = SQU;
    framebuffer[WIDTH-1][y] = SQU;
  }
}

//* This fuction initialize the board of the game without pieces
static void initFramebuffer(void){
  int x,y;

  for(y=0; y<HEIGHT; y++){
    for(x=0; x<WIDTH; x++){
       framebuffer[x][y] = ' ';
    }
  }
  paintEdges();  //comentada porque no nos interesa printear los bordes
}

//! Deberiamos modificar esta funcion para que imprima de la manera que queramos en el screen
//* This function prints the board
static void printFrameBuffer(void){
  int x,y;
  unsigned char c[2];
  if(updateScreen==true) {
    updateScreen = false;
    for(y=0; y<HEIGHT; y++){
      for(x=0; x<WIDTH; x++){
        c[0] = framebuffer[x][y];
        c[1] = '\0'; //? y esto
        SCI_send((char*)&c[0]);
      }
      SCI_send("\n\r\0");
    }
  }
}

//* This function print the piece into the board (the frame buffer), 
//* to do that checks for the shape and the rotation of the piece if there is a "SQU"
static void printPiece(piece *p, unsigned char c){
  int x,y;
  unsigned char v;

  for(x=0; x<4; x++){
    for(y=0; y<4; y++){
      v = *(p->shapes + (y*4) + ((p->currentRotate)*4*4) + x);
      if(v==SQU){
        framebuffer[p->x+x][p->y+y] = c;
      }
    }
  }
}

//* this function is called when there is a complete floor
static void eatlines(void) {
  int x,y,y2;

  updateScreen = true;
  for(y=HEIGHT-2; y>0; y--){
    unsigned char eraseline = 1;
    for(x=1; x<(WIDTH-1); x++){
      if(framebuffer[x][y]!=SQU){
        eraseline = 0;
        x = WIDTH;
      }
    }
    if(eraseline==1){
      for(y2=y; y2>0; y2--){
        for(x=1; x<(WIDTH-1); x++){
          framebuffer[x][y2+1] = framebuffer[x][y2];
        }
      }
      y++;
    }
  }
}


//* this function is called when the piece touch the floor 
static char checkAttach(piece *p){
  int x,y;
  unsigned char v;

  //p-> y = (p->y)+1;
  for(x=0; x<4; x++){
    for(y=3; y>=0; y--){
      v = *(p->shapes + (y*4) + ((p->currentRotate)*4*4) + x);
      if(v==SQU){
        if(framebuffer[(p->x)+x][(p->y)+y+1]==SQU){
          //p->y = (p->y)-1;
          p->attached = true;
          eatlines();
          return true;
        }
        break;
      }
    }
  }
  return false;
}

//* the x=1, y=1 is the upper-left corner 
//* this function checks the game over
static char lost(piece *p){
  int x;

  if(p->attached==true){
    for(x=1; x<WIDTH-1; x++){
       if(framebuffer[x][1]==SQU){
          return true;
       }
    }
  }
  return false;
}

//* This function moves the piece a position to the left
static void movePieceLeft(piece *p){
  int x,y;
  unsigned char v;

  p-> x = (p->x)-1;
  if(p-> x == 255){
    p-> x = 0;
  }
  for(x=0; x<4; x++){
    for(y=0; y<4; y++){
      v = *(p->shapes + (y*4) + ((p->currentRotate)*4*4) + x);
      if(v==SQU){
        if(framebuffer[((p->x)+x)][(p->y)+y]==SQU){
          p->x = (p->x)+1;
          x=4;
          y=4;
        }
      }
    }
  }
}

//* it puts the piece on the board (framebuffer) if it can, if is not able to do that it gives up and return false
static char createPiece(piece *p){
  int x,y;
  unsigned char v;

  for(x=0; x<4; x++){
    for(y=0; y<4; y++){
      v = *(p->shapes + (y*4) + ((p->currentRotate)*4*4) + x);
      if(v==SQU){
        if(framebuffer[(p->x)+x][(p->y)+y]==SQU){
          if(p->pieceType != 0){
            p->y=(p->y)-1;
          }
          p->attached = true;
          return false;
        }
      }
    }
  }
  return true;
}

//* This function moves the piece a position to the right
static void movePieceRight(piece *p){
  int x,y;
  unsigned char v;

  p-> x = (p->x)+1;
  for(x=0; x<4; x++){
    for(y=0; y<4; y++){
      v = *(p->shapes + (y*4) + ((p->currentRotate)*4*4) + x);
      if(v==SQU){
        if(framebuffer[(p->x)+x][(p->y)+y]==SQU){
          p->x = (p->x)-1;
          x=4;
          y=4;
        }
      }
    }
  }
}

//* This function rotate the piece.
static void rotatePiece(piece *p){
  int x,y;
  unsigned char minX,minY,maxX,maxY,previousState,v;

  updateScreen = true;
  maxY=0;
  maxX=0;
  minX=3;
  minY=3;
  previousState = p -> currentRotate;
  p -> currentRotate = (p->currentRotate)+1;
  if(p->currentRotate>((p->numOfRotates)-1)){
    p->currentRotate = 0;
  }

  for(x=0; x<4; x++){
    for(y=0; y<4; y++){
      v = *(p->shapes + (y*4) + ((p->currentRotate)*4*4) + x);
      if(v==SQU){
        if(x>maxX){
          maxX = x;
        }
        if(x<minX){
          minX = x;
        }
        if(y>maxY){
          maxY = y;
        }
        if(y<minY){
          minY = y;
        }
      }
    }
  }
  maxX += p->x;
  minX += p->x;
  maxY += p->y;
  minY += p->y;

  if(maxX>=19||maxY>=19||minX<=0){
    p->currentRotate = previousState;
  }
}

//* Just change the state
void TETRIS_Start(void) {
  TETRIS_state = TETRIS_INIT;
}

//! Creo que no nos sirve pero podriamos usar la idea de imprimir algo al inicio del juego
//* This function is called at the beginning of the game is like the instructions manual 
static void PrintWelcome(void) {
  /* clear any pending events */
  SCI_send("***********\r\n");
  SCI_send("* TETRIS  *\r\n");
  SCI_send("***********\r\n");
  SCI_send("Keyboard:\r\n");
  SCI_send(" w: rotate\r\n");
  SCI_send(" a: move left\r\n");
  SCI_send(" s: drop\r\n");
  SCI_send(" d: move right\r\n");
  SCI_send(" x: move down\r\n");
  SCI_send("Board:\r\n");
  SCI_send(" SW3:     move left\r\n");
  SCI_send(" SW2:     move right\r\n");
  SCI_send(" SW2+SW3: rotate\r\n");
  SCI_send("Press any to start game. \r\n");
}

//! Modificar segun los inputs que tengamos
//* this function reads the new key entered by the user
static TETRIS_Action ReadKey(void) {
  TETRIS_Action action;

  //action = read_keypad(); /* read push buttons from board */
   //if (action==TETRIS_Action_None) { /* read keys from terminal */
     switch (SCI_read_nb()) { /* keyboard handling */
       case 'w': action = TETRIS_Action_Rotate;    break;
       case 'a': action = TETRIS_Action_MoveLeft;  break;
       case 's': action = TETRIS_Action_Drop;      break;
       case 'd': action = TETRIS_Action_MoveRight; break;
       case 'x': action = TETRIS_Action_MoveDown;  break;
       default:  action = TETRIS_Action_None;      break;
     }
   //}
  return action;
}

//*This function is called when it is time to play
/* return true if game is lost (with the variable lostFlag) */
static unsigned char Play(void) {
  system("cls");
  unsigned char lostFlag = false;
  TETRIS_Action action;

  printPiece(&pieces[piece_ptr],SQU);
  printFrameBuffer();
  if(checkAttach(&pieces[piece_ptr])==false){
    printPiece(&pieces[piece_ptr],' ');
  } else {
    //* Configurate the piece 
    pieces[piece_ptr].y = 1;
    pieces[piece_ptr].x = WIDTH/2;
    pieces[piece_ptr].currentRotate = 0;
    pieces[piece_ptr].attached = false;

    //* increments the pointer to the limit and complete the lap
    piece_ptr++;
    if(piece_ptr>6){
      piece_ptr = 0;
    }

    //* It trys to create the piece and if it can not => game over
    if(createPiece(&pieces[piece_ptr])==false){
      lostFlag = true;
    }
  }

  //* if you pass through here 10 times then the piece descends
  frame++;
  if(frame==10){
    pieces[piece_ptr].y++;
    updateScreen = true;
    frame = 0;
  }

  //* now the program reads the key and according to the value does different things
  action = ReadKey();
  updateScreen = true;
  switch (action)
  {
  case TETRIS_Action_MoveRight:
    movePieceRight(&pieces[piece_ptr]);
    break;
  case TETRIS_Action_MoveLeft:
    movePieceLeft(&pieces[piece_ptr]);
    break;
  case TETRIS_Action_Drop:
    while (checkAttach(&pieces[piece_ptr])==false){
      pieces[piece_ptr].y++;
    }
    break;
  case TETRIS_Action_MoveDown:
    updateScreen = true;
    if (checkAttach(&pieces[piece_ptr])==false){
      pieces[piece_ptr].y++;
    }
    break;
  case TETRIS_Action_Rotate:
    printPiece(&pieces[piece_ptr],' ');   /* clear */
    rotatePiece(&pieces[piece_ptr]);      /* rotate */
    printPiece(&pieces[piece_ptr],SQU);   /* redraw */
    break;
  case TETRIS_Action_None:
    updateScreen = true;
    break;
  default:
    lostFlag = true;
    break;
  }
  lostFlag = lost(&pieces[piece_ptr]) || lostFlag;
  return !lostFlag;
}

//* this is a very simple state machine that controlls the game
int TETRIS_Run(void) {
  switch(TETRIS_state) {
    case NO_TETRIS:
      break;
    case TETRIS_INIT:
      PrintWelcome();
      TETRIS_state = TETRIS_WAIT_FOR_START;
      break;
    case TETRIS_WAIT_FOR_START:
      // if (SCI_read_nb()!='\0' || read_keypad()!=TETRIS_Action_None) {
      //   TETRIS_state = TETRIS_START;
      // }
      if (SCI_read_nb()!='\0') {
        TETRIS_state = TETRIS_START;
      }
      break;
    case TETRIS_START:
      initFramebuffer();
      printFrameBuffer();
      frame = 0;
      piece_ptr = 0;
      TETRIS_state = TETRIS_PLAY;
      break;
    case TETRIS_PLAY:
      if (!Play()) {
        TETRIS_state = TETRIS_LOST;
      }
      break;
    case TETRIS_LOST:
      system("cls");
      printPiece(&pieces[piece_ptr],SQU);
      printFrameBuffer();
      SCI_send("You lost!\n\rPress any key...\n\0");
      TETRIS_state = TETRIS_END;
      break;
    case TETRIS_END:
      // if (SCI_read_nb()!='\0' || read_keypad()!=TETRIS_Action_None) {
      //   TETRIS_state = TETRIS_START;
      //   return GAME_OVER;/* end */
      // }
      if (SCI_read_nb()!='\0') {
        TETRIS_state = TETRIS_START;
        return GAME_OVER;/* end */
      }
      break;
  }/* switch */
  return GAME_RUNNING;/* continue */
}
