#include <iostream>
#include <raylib.h>
#include <vector>
#include <iomanip>
#include <cmath>


class MathUtil
{
public:
    static float distance_between_Vector2s(Vector2 pos1, Vector2 pos2)
    {
        float new_x = (pos2.x - pos1.x) * (pos2.x - pos1.x);
        float new_y = (pos2.y - pos1.y) * (pos2.y - pos1.y);
        float distance = std::sqrt(new_x + new_y);
        //std::cout << "Distance: " << distance;
        return distance;
    }
    static Vector2 get_vector2_direction(Vector2 pos1, Vector2 pos2)
    {
        float length = distance_between_Vector2s(pos1, pos2);
        Vector2 unit_vector = { pos1.x / length, pos1.y / length };
        std::cout << "Direction: {" << unit_vector.x << " , " << unit_vector.y << "}\n";
        return unit_vector;
    }
    static Vector2 mirror_direction(Vector2 direction)
    {
        float x_direction = direction.x;
        float y_direction = direction.y;
        Vector2 inversed_direction = { y_direction, x_direction };
        std::cout << "Inversed Direction: {" << inversed_direction.x << " , " << inversed_direction.y << "}\n";
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

};


class Circle
{
public:
    int id;                     //ID for the vector
    Vector2 position;           //Position of the circle in worldspace
    float radius;
    float thickness = 3.0f;
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
    //CheckCollisionCircleLine(ring_center, (ring_radius + ring_thickness) / 2.0f, line_start, line_end)
    bool did_collide(Vector2 current_position, Vector2 previous_position)
    {
        return CheckCollisionCircleLine(position, (radius + thickness), current_position, previous_position);
    }
};


class Laser
{
public:
    int id;
    Vector2 start_position;
    Vector2 current_position;
    Vector2 current_direction;
    float step_value = 1.0f;     //This should translate to the number of pixels moved per tick
    Vector2 step;
    float energy;           //This is going to be the value which interacts with the reflectiveness inside the circle object
    std::vector<Vector2> laser_history;         //Stores position  history
    std::vector<Vector2> direction_history;     //Stores direction history
    Laser(int laser_id, Vector2 laser_start_position, Vector2 laser_direction, float laser_energy)
    {
        id = laser_id;
        start_position = laser_start_position;
        current_position = start_position;
        current_direction = laser_direction;
        energy = laser_energy;
        step = MathUtil::multiplication(current_direction, step_value);
    }
    void reflect(Vector2 reflecting_object_position) //This method is going to take in the 
    {
        Vector2 object_direction = (MathUtil::get_vector2_direction(reflecting_object_position, current_position));
        direction_history.push_back(current_direction);
        current_direction = MathUtil::mirror_direction(object_direction);
    }
    void extend_laser()
    {
        laser_history.push_back(current_position);
        current_position = MathUtil::add(current_position, step);
    }
    void render_line()
    {
        for (size_t i = 0; i < laser_history.size(); i++)
        {
            //Will render all of the laser's lines together
        }
    }
};


void sim_loop(Laser laser, std::vector<Circle> circle_arr)
{
    laser.extend_laser();
    for (size_t i = 0; i < circle_arr.size(); i++)
    {
        if (circle_arr[i].did_collide(laser.current_position, laser.laser_history[laser.laser_history.size() - 1]))
        {
            std::cout << "Collided";
        }
    }


}


int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    std::vector<Circle> circle_arr;

    InitWindow(screenWidth, screenHeight, "raylib C++ Boilerplate"); // Initialize window with dimensions and title

    SetTargetFPS(240); // Set desired framerate (frames-per-second)

    

    for (size_t i = 0; i < 5; i++)
    {
        //circle_arr.push_back(Circle(0, ring_center, ring_radius));
    }

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

        DrawText("Congrats! You've set up raylib!", 190, 200, 20, LIGHTGRAY); // Draw 
        //void DrawRing(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color); // Draw ring
        Vector2 ring_center = { screenWidth / 2.0f, screenHeight / 2.0f };
        float ring_radius = 60.0f;
        float ring_thickness = 3.0f + ring_radius;
        Vector2 line_start{ 0, screenHeight / 2.0f };
        Vector2 line_end{ screenWidth / 2.0f + 100.0f, screenHeight / 2.0f };


        //std::cout << "Size: " << circle_arr.size() << "\n";
        //DrawRing(ring_center, ring_radius, ring_thickness, 0.0f, 360.0f, 100, BLACK);
        //bool CheckCollisionCircleLine(Vector2 center, float radius, Vector2 p1, Vector2 p2);               // Check if circle collides with a line created betweeen two points [p1] and [p2]
        //DrawLine(line_start.x, line_start.y, line_end.x, line_end.y, RED);
        //std::cout << "Collision Detected: " << CheckCollisionCircleLine(ring_center, (ring_radius + ring_thickness) / 2.0f, line_start, line_end) << "\n";
        /*
        Vector2 pos1 = { 2.0f, 1.0f };
        Vector2 pos2 = { 3.0f, 4.0f };
        MathUtil::distance_between_Vector2s(pos1, pos2);
        Vector2 direction = MathUtil::get_vector2_direction(pos1, pos2);
        MathUtil::mirror_direction(direction);




        Laser(int laser_id, Vector2 laser_start_position, Vector2 laser_direction, float circle_energy)

        */

        Vector2 laser_start = { 0, screenHeight / 2 };
        Vector2 laser_direction = { 0,-1 };
        Laser laser = Laser(0, laser_start, laser_direction, 1);

        sim_loop(laser, circle_arr);

        //DrawText(, 190, 230, 20, BLACK);
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