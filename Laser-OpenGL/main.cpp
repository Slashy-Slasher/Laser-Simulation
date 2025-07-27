#include <iostream>
#include <raylib.h>
#include <vector>
#include <iomanip>
#include <cmath>
#include <random>


class MathUtil
{
public:
    static float distance_between_Vector2s(Vector2 pos1, Vector2 pos2)
    {
        float new_x = (pos2.x - pos1.x) * (pos2.x - pos1.x);
        float new_y = (pos2.y - pos1.y) * (pos2.y - pos1.y);
        float distance = std::sqrt(new_x + new_y);
        return distance;
    }
    
    static Vector2 get_vector2_direction(Vector2 from, Vector2 to)
    {
        Vector2 delta = { to.x - from.x, to.y - from.y };
        float length = sqrtf(delta.x * delta.x + delta.y * delta.y);
        if (length == 0.0f) return { 0.0f, 0.0f };
        Vector2 unit_vector = { delta.x / length, delta.y / length };
        return unit_vector;
    }

    static Vector2 mirror_direction(Vector2 direction)
    {
        float x_direction = direction.x;
        float y_direction = direction.y;
        Vector2 inversed_direction = { y_direction, x_direction };
        return inversed_direction;
    }
    static Vector2 multiplication(Vector2 vec, float mult)
    {
        vec.x = vec.x * mult;
        vec.y = vec.y * mult;
        return vec;
    }
    static Vector2 add(Vector2 vec1, Vector2 vec2)
    {
        Vector2 new_vec = { vec1.x + vec2.x, vec1.y + vec2.y };
        return new_vec;
    }
    static Vector2 sub(Vector2 vec1, Vector2 vec2)
    {
        Vector2 new_vec = { vec1.x - vec2.x, vec1.y - vec2.y };
        return new_vec;
    }
    static float dot_product(Vector2 vec1, Vector2 vec2)
    {
        return (vec1.x * vec2.x + vec1.y * vec2.y);
    }

};


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
        DrawRing(position, radius, thickness, 0.0f, 360.0f, 10000, BLACK);
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


void render_entire_loop(Laser& laser, std::vector<Circle>& circle_arr)
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
    float angle = 0;
    Vector2 current_starting_direction = { 0, cosf(0) };
    std::cout << current_starting_direction.y;

    float ring_thickness = 3.0f;
    float ring_radius = 20.0f + ring_thickness;
    
    /*
    for (size_t i = 0; i < 40; i++)
    {
        float random_offset = 300;
        Vector2 ring_center = { screenWidth / 2.0f + getRandomFloat(-random_offset, random_offset), screenHeight / 2.0f + getRandomFloat(-random_offset, random_offset)};
        circle_arr.push_back(Circle(int(i), ring_center, ring_radius));
    }
    */
    float random_offset = 300;
    Vector2 ring_center = { screenWidth / 2.0f , screenHeight / 2.0f };
    //circle_arr.erase(circle_arr.begin()); //Combats a current bug where the first ring is solid black because radius is undefined for the first construction

    Vector2 laser_start = { 0, screenHeight / 2 };
    Vector2 laser_direction = { 1, 0};

    Laser laser = Laser(0, laser_start, laser_direction, 1);



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
        render_loop(laser, circle_arr);


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
            simulation_running = !simulation_running;
        }

        render_entire_loop(laser, circle_arr);

        /*
        if (simulation_running)
        {
            //sim_loop(laser, circle_arr);
        }
        */

        // Oscillates from -90 to +90
        angle = angle + .01f;
      
        float angle_rad = (angle * PI) / 180.0f; // Convert to radians


        float cos_value = cosf(angle_rad);
        float sin_value = sinf(angle_rad);

        current_starting_direction = { abs(sin_value) , -cos_value };
        laser.change_angle(current_starting_direction);
      

        DrawFPS(5, 0);

        EndDrawing(); // End drawing and swap buffers
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and unload OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}