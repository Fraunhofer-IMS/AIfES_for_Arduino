/*
  www.aifes.ai
  https://github.com/Fraunhofer-IMS/AIfES_for_Arduino
  Copyright (C) 2020-2022  Fraunhofer Institute for Microelectronic Circuits and Systems.
  All rights reserved.

  AIfES is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  AIfES TicTacToe Q7 demo
  --------------------

  Versions:
    1.0.0   Initial version
  
  The sketch shows an example of how the inference of an already trained network is performed. 
  In the concrete example, a neural network was trained to play the game TicTacToe. 
  The neural network was trained in Keras and the configuration including the weights was imported into AIfES
  using the aifes_pytools. 
  The neural network related code is in the neural_network.ino file. The game related code is in the main file.
  The calculation is done in int 8 (Q7).
  
  On AVR controllers (e.g. Arduino Uno), the weights and stings will be stored and loaded directly from 
  the program memory, because the RAM might be too small.
  
  On ARM controllers (e.g. Arduino Nano 33 BLE), CMSIS can be enabled by uncommenting "#define USE_CMSIS_ACCELERATION_ON_ARM" to speed up the inference.
  For this, AIFES_WITH_CMSIS must be activated in aifes_config.h and CMSIS-NN library must be available inside the AIfES_for_Arduino library.
  See AIfES_for_Arduino/src/CMSIS/README.md for more information about using CMSIS library.
  
  Tested on:
    Arduino Nano
    Arduino Nano Every
    Arduino Nano 33 BLE
    Seeeduino XIAO
   
*/

// Only on ARM contollers (e.g. Arduino Nano 33 BLE). AIFES_WITH_CMSIS must be activated in aifes_config.h and CMSIS-NN library must be installed.
//#define USE_CMSIS_ACCELERATION_ON_ARM

#include <aifes.h>
#if __AVR__
#include <aifes_avr_pgm.h>
#endif // __AVR__
#if __arm__ && defined USE_CMSIS_ACCELERATION_ON_ARM
#include <aifes_cmsis.h>
#endif // __arm__

#define  O   -64  // (value=-64; shift=6; zero_point=0) -> -1.0f
#define  X    64  // (value=64; shift=6; zero_point=0)  ->  1.0f

// The TicTacToe board. Can be X (AI), O (Player) or 0 (Free).
int8_t board[9];

// Print the TicTacToe board to the console
void print_tictactoe_board(int8_t *board)
{
  uint8_t i, j;

  Serial.print(F("\n     a   b   c  \n"));
  Serial.print(F("   ------------- \n"));
  for(i = 0; i < 3; i++){
    Serial.print(i+1); Serial.print(F(" | "));
    for(j = 0; j < 3; j++){
      if(board[i*3 + j] == O){
          Serial.print(F("O | "));
      } else if(board[i*3 + j] == X){
          Serial.print(F("X | "));
      } else {
          Serial.print(F("  | "));
      }
    }
    Serial.print(F("\n   ------------- \n"));
  }
  Serial.println();
  return;
}

// Returns the winner of the TicTacToe board (X / O). If there is no winner, it returns 0.
int8_t winner(int8_t *board)
{
  int i, j;
  
  int x_count = 0;
  int o_count = 0;
  
  // check rows
  for(i = 0; i < 3; i++){
    for(j = 0; j < 3; j++){
      if(board[3*i+j] == X){
        x_count++;
      } else if(board[3*i+j] == O){
        o_count++;
      }
    }
    if(x_count == 3) return X;
    else if(o_count == 3) return O;
    else x_count = o_count = 0;
  }
  
  // check cols
  for(i = 0; i < 3; i++){
    for(j = 0; j < 3; j++){
      if(board[3*j+i] == X){
        x_count++;
      } else if(board[3*j+i] == O){
        o_count++;
      }
    }
    if(x_count == 3) return X;
    else if(o_count == 3) return O;
    else x_count = o_count = 0;
  }

  // check diagonals
  for(i = 0; i < 3; i++){
    if(board[3*i+i] == X){
      x_count++;
    } else if(board[3*i+i] == O){
      o_count++;
    }
  }
  if(x_count == 3) return X;
  else if(o_count == 3) return O;
  else x_count = o_count = 0;
  
  for(i = 0; i < 3; i++){
    if(board[3*i+2-i] == X){
      x_count++;
    } else if(board[3*i+2-i] == O){
      o_count++;
    }
  }
  if(x_count == 3) return X;
  else if(o_count == 3) return O;
  else x_count = o_count = 0;

  return 0;
}

void setup() {
  // Init serial port for communication
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  // Init neural network
  init_ai_agent();
}

void loop() {
  uint8_t action;
  char user_input[2];
  uint8_t i, j;

  Serial.print(F("############ New Game ############\n"));

  // Reset the board
  for(i = 0; i < 9; i++){
    board[i] = 0.0f;
  }
  print_tictactoe_board(board);

  // TicTacToe agent
  for(i = 0; i < 5; i++){
    user_input[0] = user_input[1] = 0;
    while(user_input[0] < 97 || user_input[0] > 99 || user_input[1] < 49 || user_input[1] > 51){
      Serial.print(F("\nYour turn: Please enter the coordinates (e.g. a3) you want to place an O and press >enter<\n"));
      while(!Serial.available());
      user_input[0] = Serial.read();
      while(!Serial.available());
      user_input[1] = Serial.read();
      while(!Serial.available());
      while(Serial.available()) Serial.read(); // Remove new line character

      action = (user_input[0] - 97) + (user_input[1] - 48 - 1) * 3;

      if((user_input[0] < 97 || user_input[0] > 99 || user_input[1] < 49 || user_input[1] > 51)){
        Serial.print(F("Wrong input. The coordinates have to be one of {a1, a2, a3, b1, b2, b3, c1, c2, c3}.\n"));
        user_input[0] = user_input[1] = 0;
      } else if(board[action] != 0){
        Serial.print(F("Wrong input. The field is already occupied.\n"));
        user_input[0] = user_input[1] = 0;
      }
    }
    
    board[action] = O;
    
    Serial.print(F("Your turn was ")); Serial.print(user_input[0]); Serial.println(user_input[1]);

    print_tictactoe_board(board);

    if(winner(board) == O){
      Serial.print(F("\n ******* Congratulations: You won the game! This sould be impossible in therory. ******* \n\n"));
      break;
    }

    unsigned long timer = micros(); // Start timer
    
    action = run_ai_agent(board);
    
    timer = micros() - timer; // Time measurement
    
    board[action] = X;

    Serial.print(F("The AI took ")); Serial.print(timer); Serial.println(F(" μs to think about the turn."));
    Serial.print(F("\nAIs turn was ")); Serial.print((char) (action % 3 + 97)); Serial.println((int)(action / 3 + 1));

    print_tictactoe_board(board);
    
    if(winner(board) == X){
      Serial.print(F("\n ******* You lost the game! ******* \n\n"));
      break;
    }
  }

}
