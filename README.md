# Snake-Game-Using-OOP
This is a modern implementation of the classic Snake game developed using C++ and [Raylib](https://www.raylib.com/). The game includes extended features such as growth food, shrink food, and negative food that affect gameplay in unique ways. Execute main.exe files to play the game

##  Features

- Classic Snake movement and collision mechanics.
- Multiple food types:
  -  **Normal Food**: Increases score by 1 and adds a segment.
  -  **Negative Food**: Decreases score by 1.
  -  **Shrink Food**: Shrinks the snake's body.
  -  **Growth Food**: Grows the snake by 1 segment and increases score by 2.
- Score tracking and high score saving (`score.txt`).
- Game over detection upon wall collision or hitting own tail.
- Rules screen before the game starts.
- Sounds for eating and collisions.
- Clean and modular OOP structure.

## Structure

- **Snake Class**: Handles the snake body, movement, drawing, and resetting.
- **Food Classes**: 
  - `Food` (base)
  - `NegativeFood`
  - `ShrinkFood`
  - `GrowthFood`
- **Scoreboard Class**: Manages saving and retrieving scores.
- **Game Class**: Central manager for game state, updates, drawing, and collisions.


>  Make sure these paths are correct or update them in the source code accordingly.
   
##  How to Build and Run

### Prerequisites

- C++ compiler (e.g., `g++`)
- Raylib library installed on your system

### Compilation Example (windows)

```bash
g++ main.cpp -o snake.exe -lraylib -lopengl32 -lgdi32 -lwinmm
snake.exe


