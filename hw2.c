// Based off ex 1
#include "CSCIx239.h"

int th=0,ph=0; //  View angles
int zh=0;      //  Light position
int move=1;    //  Moving light
int fov=55;    //  Field of view (for perspective)
int obj=0;     //  Object
int shader=0;  //  Shader
float asp=1;   //  Aspect ratio
float dim=3;   //  Size of world

// Shader variables
float originX = 0, originY = 0, originZ = 1;
float startTime = 0;

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Set view
   View(th,ph,fov,dim);
   //  Enable lighting
   Lighting(3*Cos(zh),1.5,3*Sin(zh) , 0.3,0.5,0.8);

   //  Enable shader and add uniform variables
   glUseProgram(shader);
   int id = glGetUniformLocation(shader, "time");
   float time = startTime == -1 ? -1 : glfwGetTime() - startTime;
   time -= 1;
   time /= 5;
   glUniform1f(id, time);
   id = glGetUniformLocation(shader, "coeffs");
   float coeff1 = 0.06 * time * (sin(3 * time) + 2);
   float coeff2 = 0.05 * time * (sin(5 * time) + 2);
   float coeff3 = 0.1 * time;
   glUniform3f(id, coeff1, coeff2, coeff3);
   id = glGetUniformLocation(shader, "origin");
   glUniform3f(id, originX, originY, originZ);

   //  Draw scene
   switch (obj)
   {
      case 0: SolidCube();        break;
      case 1: SolidIcosahedron(); break;
      case 2: SolidTorus(0.7, 20);       break;
      default: SolidTeapot(8);    break;
   }

   //  Revert to fixed pipeline
   glUseProgram(0);
   glDisable(GL_LIGHTING);

   //  Display parameters
   glColor3f(1,1,1);
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f Projection=%s",th,ph,dim,fov>0?"Perpective":"Orthogonal");
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glfwSwapBuffers(window);
}

//
//  Key pressed callback
//
void key(GLFWwindow* window,int key,int scancode,int action,int mods)
{
   //  Discard key releases (keeps PRESS and REPEAT)
   if (action==GLFW_RELEASE) return;

   //  Exit on ESC
   if (key == GLFW_KEY_ESCAPE)
     glfwSetWindowShouldClose(window,1);
   //  Reset view angle
   else if (key==GLFW_KEY_0)
      th = ph = 0;
   //  Switch objects
   else if (key==GLFW_KEY_O){
      obj = (obj + 1)%4;
      startTime = -1;
   }
   //  Toggle moving light 
   else if (key == GLFW_KEY_M)
      move = !move;
   //  Switch between perspective/orthogonal
   else if (key==GLFW_KEY_P)
      fov = fov ? 0 : 57;
   //  Increase/decrease asimuth
   else if (key==GLFW_KEY_RIGHT)
      th -= 5;
   else if (key==GLFW_KEY_LEFT)
      th += 5;
   //  Increase/decrease elevation
   else if (key==GLFW_KEY_UP)
      ph += 5;
   else if (key==GLFW_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key==GLFW_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key==GLFW_KEY_PAGE_UP && dim>1)
      dim -= 0.1;

   else if(key==GLFW_KEY_R) startTime = -1;
   else if(key==GLFW_KEY_SPACE) startTime = glfwGetTime();

   //  Wrap angles
   th %= 360;
   ph %= 360;
   //  Update projection
   Projection(fov,asp,dim);
}

//
//  Window resized callback
//
void reshape(GLFWwindow* window,int width,int height)
{
   //  Get framebuffer dimensions (makes Apple work right)
   glfwGetFramebufferSize(window,&width,&height);
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Projection(fov,asp,dim);
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("HW2: Nathan Howard",1,600,600,&reshape,&key);

   //  Load shader
   shader = CreateShaderProg("fire.vert", "fire.frag");

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      if (move) zh = fmod(90*glfwGetTime(),360);
      //  Display
      display(window);
      //  Process any events
      glfwPollEvents();
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
