#include <iostream>
#include <raylib.h>
#include <vector>
#include <iomanip>
#include <cmath>
#include <random>

#include "MathUtil.h"

class Circle
{
public:
    int id;                     //ID for the vector
    Vector2 position;           //Position of the circle in worldspace
    float radius = 30;
    float thickness_offset = 3.0f;
    float thickness = radius - thickness_offset;
    float diameter = radius * 2.0f;
    float start_angle = 0.0f;   // 0 -> 360 defines the full circle
    float end_angle = 360.0f;   // 0 -> 360 defines the full circle
    float reflectiveness = 1.0f; //Represents how much energy remains after contact, 0 means laser stops, 1 means no energy loss. Should be clamped between 0-1

    Circle(int circle_id, Vector2 circle_position, float circle_radius) //Will add reflectiveness later for more control, right now its always set to 1
    {
        id = circle_id;
        position = circle_position;
        radius = circle_radius;
    }
    void render()
    {
        DrawRing(position, radius, thickness, 0.0f, 360.0f, 100, BLACK);
    }
    bool did_collide(Vector2 current_position, Vector2 previous_position)
    {
        return CheckCollisionCircleLine(position, (radius+thickness_offset+2), current_position, previous_position);
    }
};


class Laser
{
public:
    int id;
    Vector2 start_position;
    Vector2 current_position;
    Vector2 current_direction;
    Vector2 original_direction; //Store Values for Reset
    Vector2 original_position;  //Store Values for Reset
    float current_angle;        //Radians
    float step_value = 1.0f;     //This should translate to the number of pixels moved per tick
    Vector2 step;
    float energy = 10000;           //This is going to be the value which interacts with the reflectiveness inside the circle object
    float original_energy = energy;
    float radiation = 1;

    Circle* last_hit = nullptr;

    std::vector<Vector2> laser_history;         //Stores position  history
    std::vector<Vector2> direction_history;     //Stores direction history
    Laser(int laser_id, Vector2 laser_start_position, Vector2 laser_direction, float laser_energy)
    {
        id = laser_id;
        start_position = laser_start_position;

        original_position = laser_start_position;
        current_position = start_position;
        current_direction = laser_direction;

        original_direction = current_direction;
        energy = laser_energy;
        step = MathUtil::multiplication(current_direction, step_value);
    }
    void reflect(Vector2& reflecting_object_position) //This method is going to take in the 
    {
        Vector2 incident_ray = current_direction;
        Vector2 normal = (MathUtil::get_vector2_direction(reflecting_object_position, current_position )); //New Direction

        //Main formula used in the simulation, mirrors the ray off of the rings
        //i - 2-dotproduct(i,n) * n

        float product = (2 * MathUtil::dot_product(incident_ray, normal));                                          //Product Calculation
        Vector2 reflected_direction = MathUtil::sub(incident_ray, MathUtil::multiplication(normal, product));       //Finds the reflected direction
        current_direction = reflected_direction;                        //Sets Direction

        step = MathUtil::multiplication(current_direction, step_value); //Updates Step Direction
    }

    void extend_laser()
    {
        laser_history.push_back(current_position);
        current_position = MathUtil::add(current_position, step);
        energy = energy - radiation;
    }

    void render_line()
    {
        for (size_t i = 1; i < laser_history.size(); i++)
        {
            DrawLine(laser_history[i].x, laser_history[i].y, laser_history[i - 1].x, laser_history[i - 1].y, RED);
        }
    }
    void set_last_hit(Circle* new_last_hit)
    {
        last_hit = new_last_hit;
    }
    void change_angle(Vector2 new_direction)
    {
        original_direction = new_direction;
    }

    void reset_laser()
    {
        current_direction = original_direction;
        current_position = original_position;
        laser_history.clear();
        direction_history.clear();
        energy = 10000;
        step = MathUtil::multiplication(current_direction, step_value);
        last_hit = nullptr;
    }
};

//Uses linear logic, laser extends sequentially not at the "Speed of Light"
void sim_loop(Laser& laser, std::vector<Circle>& circle_arr)
{
    laser.extend_laser();
    for (size_t i = 0; i < circle_arr.size(); i++)
    {        
        if (circle_arr[i].did_collide(laser.current_position, laser.laser_history[laser.laser_history.size() - 1]))
        {
            if (laser.last_hit != &circle_arr[i])
            {
                laser.reflect(circle_arr[i].position);
                laser.set_last_hit(&circle_arr[i]);
            }
        }
        
    }
}


void run_entire_loop(Laser& laser, std::vector<Circle>& circle_arr)
{
    laser.reset_laser();

    while (laser.energy > 0)
    {
        sim_loop(laser, circle_arr);
    }
}


float getRandomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}


void render_loop (Laser& laser, std::vector<Circle>& circle_arr)
{
    for (size_t i = 0; i < circle_arr.size(); i++)
    {
        circle_arr[i].render();
    }
    laser.render_line();
}





int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 800;
    std::vector<Circle> circle_arr;

    InitWindow(screenWidth, screenHeight, "Laser-Simulation"); // Initialize window with dimensions and title

    SetTargetFPS(60); // Set desired framerate (frames-per-second)

    bool simulation_running = false; //Used to start the simulation 
    bool laser_type = false;    //Defines the laser as either static or variable movement
    float step_value = .01;
    float angle = 0;
    Vector2 current_starting_direction = { 0, cosf(0) };
    std::cout << current_starting_direction.y;



    float ring_thickness = 3.0f;
    float ring_radius = 20.0f + ring_thickness;
    


    /*
    
    */

    float random_offset = 300;
    Vector2 ring_center = { screenWidth / 2.0f , screenHeight / 2.0f };
    //circle_arr.erase(circle_arr.begin()); //Combats a current bug where the first ring is solid black because radius is undefined for the first construction

    Vector2 laser_start = { 0, screenHeight / 2 };
    Vector2 laser_direction = { 1, 0};
    Laser laser = Laser(0, laser_start, laser_direction, 1);



    std::cout << GetWorkingDirectory() << std::endl;
    Shader laserShader = LoadShader("laser_shader.vs", "laser_shader.fs");
    int laserPosLoc = GetShaderLocation(laserShader, "laserPositions");
    int laserCountLoc = GetShaderLocation(laserShader, "laserCount");


    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Your game logic updates go here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing(); // Start drawing
        ClearBackground(RAYWHITE); // Clear background with a color

        DrawText("Created with C++ and Raylib!", screenWidth/2-50, 10, 30, LIGHTGRAY); // Draw 
        //render_loop(laser, circle_arr);


        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            DrawText("Left mouse button is being held down!", 100, 100, 20, RED);
            circle_arr.push_back(Circle(circle_arr.size(), GetMousePosition(), ring_radius));
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            for (size_t i = 0; i < circle_arr.size();)
            {
                Vector2 mouse_pos = GetMousePosition();

                if (circle_arr[i].did_collide(mouse_pos, mouse_pos))
                {
                    circle_arr.erase(circle_arr.begin()+ i);
                }
                else
                {
                    ++i;
                }
            }
        }
        if (IsKeyDown(KEY_SPACE))
        {
            laser.laser_history.clear();
        }
        if (IsKeyPressed(KEY_R))
        {
            for (size_t i = 0; i < 40; i++)
            {
                float random_offset = 300;
                Vector2 ring_center = { screenWidth / 2.0f + getRandomFloat(-random_offset, random_offset), screenHeight / 2.0f + getRandomFloat(-random_offset, random_offset) };
                circle_arr.push_back(Circle(int(i), ring_center, ring_radius));
            }
        }
        if (IsKeyPressed(KEY_T))
        {
            circle_arr.clear();
        }

        if (IsKeyPressed(KEY_P))
        {
            simulation_running = !simulation_running;
        }

        if (IsKeyPressed(KEY_UP))
        {
            step_value += .01;
        }
        if (IsKeyPressed(KEY_DOWN))
        {
            step_value -= .01;
        }

        /*
        Currnet Controls

            Space = Change sim_mode to either rotational or static  -- In progress
            P = Pause the laser at it's current direction
            R = spawn in random circles
            T = Clear the array of circles
            CTRL + C = Close program
            ESC = Also closes
            Left Click = Spawn single circle
            Right Click(Over circle) = clear single "selected" circle
        
        */



        run_entire_loop(laser, circle_arr);  // Calculate laser path
        render_loop(laser, circle_arr);      // Draw rings (but not laser)

        size_t laser_count = std::min(laser.laser_history.size(), size_t(100));
        float laserPosArray[200];

        for (size_t i = 0; i < laser_count; i++) {
            laserPosArray[i * 2] = laser.laser_history[i].x;
            laserPosArray[i * 2 + 1] = laser.laser_history[i].y;
        }
        int laser_count_int = static_cast<int>(laser_count);

        // Set laser positions (array of vec2)
        SetShaderValue(laserShader, laserPosLoc, laserPosArray, SHADER_UNIFORM_VEC2);

        // Set laser count (single int)
        SetShaderValue(laserShader, laserCountLoc, &laser_count_int, SHADER_UNIFORM_INT);

        // Shader draws glow effect over screen based on laser trail
        BeginShaderMode(laserShader);
        DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);
        EndShaderMode();


        
        if (simulation_running)
        {
            sim_loop(laser, circle_arr);
        }
        else
        {
            // Oscillates from -90 to +90
            angle = angle + step_value;

            float angle_rad = (angle * PI) / 180.0f; // Convert to radians


            float cos_value = cosf(angle_rad);
            float sin_value = sinf(angle_rad);

            current_starting_direction = { abs(sin_value) , -cos_value };
            laser.change_angle(current_starting_direction);
        }
        
      

        DrawFPS(5, 0);

        EndDrawing(); // End drawing and swap buffers
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(laserShader);

    CloseWindow(); // Close window and unload OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}