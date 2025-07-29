# Laser-Sim with C++ and Raylib  

<h>I saw a video on the internet of a program like this a couple years back and wanted to take a shot at it</h>

<p> I also felt that it would be a good step away from the python barnes-hut program. Additionally it functions as a nice step into C++</p>

    Currnet Controls

          Space = Change sim_mode to either rotational or static  -- In progress
          P = Pause the laser at it's current direction
          R = spawn in random circles
          T = Clear the array of circles
          CTRL + C = Close program
          ESC = Also closes
          Left Click = Spawn single circle
          Right Click(Over circle) = clear single "selected" circle
      
This is what the program looked like on the first day of its creation:
<p align="center">
  <img src="Laser-Sim2.gif" alt="Quadtree Demo" />
</p>
In a way I find the slower laser more interesting to look at, so I'm leaving it up on this page


# What I've learned

I managed to take a good chunk of valuable information out of what was designed to be a quick one and done project. Not only did I reinforce my working knowledge of traditional C++ syntax, I found myself experimenting with pointers, trying to understand shader code(I will come back and fix it).

This was also a really interesting trip into mathematics, while I initially thought this would involve some form of a rotational matrix... I wound up wrong because of this simulations dynamic "Incidence" angle, which represents the angle which the laser approaches from.

<img width="269" height="389" alt="image" src="https://github.com/user-attachments/assets/33cda1cc-ba8f-4bda-9a45-94a8714351b3"/>

The primary math behind this program is about drawing the "Normal" from the surface being reflected on. Luckily for me, I've already done that before in a former program: If you havn't seen my planetary simulation [https://github.com/Slashy-Slasher/Quadtree-Division-System]. In fact that experience with unit vectors and positioning things in 2-d actually made
this program a lot easier to write. 

__I was able to push around 80% of my desired features on day 1.__

As hinted at previously, this program isn't done:

    Before I can consider this project done:
      GLSL rendering needs to work
      I want to add buttons to the top of the game window
      Zoom and Pan
      Allow more variation on the direction of the laser(full 360) with circles anywhere
