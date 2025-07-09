#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>
#include <fstream>

using namespace std;

static bool allowMove = false;
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

int cellSize = 25;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> &deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

bool EventTriggered(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake
{
public:
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool addSegment = false;

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true)
        {
            addSegment = false;
        }
        else
        {
            body.pop_back();
        }
    }

    void Reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }
};

class Food
{
public:
    Vector2 position;
    Texture2D texture;

    Food()
    {
        
    }

    Food(deque<Vector2> &snakeBody, const char *filename)
    {
        Image image = LoadImage(filename);
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw()
    {
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> &snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    }

     virtual  void ApplyEffect(Snake &snake)
    {
        
        
    }
};

class NegativeFood : public Food
{
public:
    NegativeFood(deque<Vector2> &snakeBody) : Food(snakeBody, "Graphics/negative.png")
    {
       
    }
};

class ShrinkFood : public Food
{
private:
    int shrinkAmount; 

public:
    ShrinkFood(deque<Vector2> &snakeBody) : Food(snakeBody, "Graphics/shrink.png")
    {
        shrinkAmount = 1; 
    }

    void ApplyEffect(Snake &snake)
    {
        
        for (int i = 0; i < shrinkAmount; ++i)
        {
            if (snake.body.size() > 1)
            {
                snake.body.pop_back();
            }
            else
            {
                
                snake.Reset();
                break;
            }
        }
        
    }
};


class GrowthFood : public Food
{
private:
    int growthAmount; 

public:
    GrowthFood(deque<Vector2> &snakeBody) : Food(snakeBody, "Graphics/growth.png")
    {
        growthAmount = 1; 
    }

    void ApplyEffect(Snake &snake)
    {
        
        for (int i = 0; i < growthAmount; i++)
        {
            snake.addSegment = true;
        }
    }
};

class Scoreboard {
    friend class Game;
public:
    void SaveScore(const int score, const string& filename) {
        ofstream file(filename, ios::app); 
        if (file.is_open()) {
            file << "Score: " << score << endl;
            file.close();
            cout << "Score saved to " << filename << endl;
        } else {
            cout << "Unable to open file: " << filename << endl;
        }
    }

    int GetHighestScore(const string& filename) {
        int highestScore = 0;
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                size_t pos = line.find("Score: ");
                if (pos != string::npos) {
                    int score = stoi(line.substr(pos + 7)); // Extract score value
                    highestScore = max(highestScore, score);
                }
            }
            file.close();
        } else {
            cout << "Unable to open file: " << filename << endl;
        }
        return highestScore;
    }
};

class Game
{
public:
    Snake snake = Snake();
    Food food;
    NegativeFood negFood;
    NegativeFood negFood1;
     ShrinkFood shrinkFood;
     GrowthFood growthFood;
    bool running = true;
    int score = 0;
    int negScorePenalty = -1;
    Sound eatSound;
    Sound wallSound;
 Scoreboard scoreboard;
    Game() : food(snake.body, "Graphics/food.png"), negFood(snake.body),negFood1(snake.body), shrinkFood(snake.body),growthFood(snake.body)
    {

        InitAudioDevice();
        eatSound = LoadSound("Sounds/eat.mp3");
        wallSound = LoadSound("Sounds/wall.mp3");
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw()
    {
        food.Draw();
        negFood.Draw();
        negFood1.Draw();
        shrinkFood.Draw();
       growthFood.Draw();
        snake.Draw();
    }

    void Update()
    {
        if (running)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithNegFood();
            CheckCollisionWithNegFood1();
            CheckCollisionWithShrinkFood();
            CheckCollisionWithGrowthFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }

    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithNegFood()
    {
        if (Vector2Equals(snake.body[0], negFood.position))
        {
            negFood.position = negFood.GenerateRandomPos(snake.body);
            score += negScorePenalty; 
            PlaySound(wallSound);     
        }
    }

       void CheckCollisionWithNegFood1()
    {
        if (Vector2Equals(snake.body[0], negFood1.position))
        {
            negFood1.position = negFood1.GenerateRandomPos(snake.body);
            score += negScorePenalty; 
            PlaySound(wallSound);    
        }
    }

void CheckCollisionWithGrowthFood()
{
    if (Vector2Equals(snake.body[0], growthFood.position))
    {
        growthFood.ApplyEffect(snake);
        growthFood.position = growthFood.GenerateRandomPos(snake.body);
          score+=2;
        PlaySound(eatSound);
    }
}
 void CheckCollisionWithShrinkFood()
    {
        if (Vector2Equals(snake.body[0], shrinkFood.position))
        {
            shrinkFood.ApplyEffect(snake);
            shrinkFood.position = shrinkFood.GenerateRandomPos(snake.body);
          
            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        negFood.position = negFood.GenerateRandomPos(snake.body);
        running = false;
        PlaySound(wallSound);
        scoreboard.SaveScore(score, "score.txt");
       
    }

void LoadGame()
{
    score = 0; 
}

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }
};


int main()
{
    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Snake Game Using OOP");
    SetTargetFPS(60);

    Game game = Game();
    game.LoadGame();
    Scoreboard scoreboard;

    bool showRules = true;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        if (showRules)
        {
            ClearBackground(green);

           DrawText("Welcome to Snake Game!", 100, 50, 30,BLACK);
    DrawText("Rules:", 100, 100, 20, BLACK);
    DrawText("1. Use arrow keys to control the snake.", 100, 130, 20, BLACK);
    DrawText("2. Eat food to grow and increase score.", 100, 160, 20, BLACK);
    DrawText("3. Avoid hitting the walls or eating negative food.", 100, 190, 20, BLACK);
    DrawText("4. Red food gives negative points.", 100, 220, 20, RED);
    DrawText("5. Orange food reduces the size of the snake.", 100, 250, 20, BROWN);
    DrawText("6. Blue food increases your score by 2 points.", 100, 280, 20, BLUE);
    DrawText("7. Black food is normal and increases your score by 1 point.", 100, 310, 20, DARKGREEN);
    DrawText("8. Press Y to start the game.", 100, 340, 20, BLACK);

            if (IsKeyPressed(KEY_Y))
            {
                showRules = false;
            }
        }
        else
        {
            if (EventTriggered(0.2))
            {
                allowMove = true;
                game.Update();
            }

            if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1 && allowMove)
            {
                game.snake.direction = {0, -1};
                game.running = true;
                allowMove = false;
            }
            if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1 && allowMove)
            {
                game.snake.direction = {0, 1};
                game.running = true;
                allowMove = false;
            }
            if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1 && allowMove)
            {
                game.snake.direction = {-1, 0};
                game.running = true;
                allowMove = false;
            }
            if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1 && allowMove)
            {
                game.snake.direction = {1, 0};
                game.running = true;
                allowMove = false;
            }

            ClearBackground(green);
            DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, darkGreen);
            DrawText("Snake Game", offset - 5, 20, 40, darkGreen);
            DrawText(TextFormat("Score: %i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
            int highestScore = scoreboard.GetHighestScore("score.txt");
            DrawText(TextFormat("Highest Score: %i", highestScore), offset + 250, offset + cellSize * cellCount + 10, 40, darkGreen);
            game.Draw();
        }

        EndDrawing();
        
    }
    CloseWindow();
    return 0;
}
