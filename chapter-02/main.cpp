#include <iostream>
#include <unistd.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "Grid.h"
#include "BinaryTree.h"
#include "Sidewinder.h"
#include "Instrumentor.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

enum T_MazeType
{
   MAZE_BINARY,
   MAZE_SIDEWINDER,
   MAZE_NUM_TYPES
};

struct T_TexCoords
{
   float min_x;
   float max_x;
   float min_y;
   float max_y;
};

constexpr int tiles = 16;

// global variables
SDL_Window* window = nullptr;
std::size_t window_width = 0;
std::size_t window_height = 0;
SDL_Surface* screen_surface = nullptr;
SDL_GLContext context;
GLuint texture_id = 0;
bool quit = false;
Cell* selected_cell = nullptr;

constexpr T_TexCoords tex_coords[16] = 
{
   { 0.0307, 0.2275, 0.0367, 0.2332 }, // index 0 - all directions
   { 0.2770, 0.4738, 0.0367, 0.2332 }, // index 1 - west, north, east
   { 0.5232, 0.7201, 0.0367, 0.2332 }, // index 2 - north, south, west
   { 0.7696, 0.9662, 0.0367, 0.2332 }, // index 3 - north, west
   { 0.0307, 0.2275, 0.2821, 0.4785 }, // index 4 - east, west, south
   { 0.2770, 0.4738, 0.2821, 0.4785 }, // index 5 - east, west
   { 0.5232, 0.7201, 0.2821, 0.4785 }, // index 6 - west, south
   { 0.7696, 0.9662, 0.2821, 0.4785 }, // index 7 - west
   { 0.0307, 0.2275, 0.5275, 0.7238 }, // index 8 - north, south, east
   { 0.2770, 0.4738, 0.5275, 0.7238 }, // index 9 - north, east
   { 0.5232, 0.7201, 0.5275, 0.7238 }, // index 10 - north, south
   { 0.7696, 0.9662, 0.5275, 0.7238 }, // index 11 - north
   { 0.0307, 0.2275, 0.7729, 0.9691 }, // index 12 - east, south
   { 0.2770, 0.4738, 0.7729, 0.9691 }, // index 13 - east
   { 0.5232, 0.7201, 0.7729, 0.9691 }, // index 14 - south
   { 0.7696, 0.9662, 0.7729, 0.9691 }, // index 15 - none
};

void handle_events(const Grid& Grid)
{
   SDL_Event event;

   while (SDL_PollEvent(&event))
   {
      switch (event.type)
      {
         case SDL_MOUSEBUTTONDOWN:
         {
            int x, y, col, row;
            double sel_x;
            double sel_y;
            double w = 1.0 / Grid.Cols();
            double h = 1.0 / Grid.Rows();

            SDL_GetMouseState(&x, &y);

            sel_x = (double)x / (double)window_width;
            sel_y = (double)y / (double)window_height;

            col = sel_x / w;
            row = sel_y / h;

            Cell* cell = Grid.GetCell(row, col);

            if (cell)
            {
               if (selected_cell)
               {
                  if (selected_cell != cell)
                  {
                     cell->SetSelected(true);
                     selected_cell->SetSelected(false);
                     selected_cell = cell;
                  }
                  else
                  {
                     if (cell->GetSelected())
                     {
                        cell->SetSelected(false);
                        selected_cell = nullptr;
                     }
                     else
                     {
                        cell->SetSelected(true);
                        selected_cell = cell;
                     }
                  }
               }
               else
               {
                  cell->SetSelected(true);
                  selected_cell = cell;
               }
            }

            break;
         }
         case SDL_QUIT:
            quit = true;
            break;
         default:
            break;
      }
   }
}

void render(const Grid& Grid)
{
   bool selected = false;
   std::size_t sel_col = 0;
   std::size_t sel_row = 0;
   double x = -1.0;
   double y = 1.0;
   double w = 2.0 / Grid.Cols();
   double h = 2.0 / Grid.Rows();

   // clear color buffer
   glClear(GL_COLOR_BUFFER_BIT);
 
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, texture_id);
   glColor4f(1.0, 1.0, 1.0, 1.0);
   glBegin(GL_QUADS);
       glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);
       glTexCoord2f(1.0, 0.0); glVertex2f( 1.0, -1.0);
       glTexCoord2f(1.0, 1.0); glVertex2f( 1.0,  1.0);
       glTexCoord2f(0.0, 1.0); glVertex2f(-1.0,  1.0);

       for (std::size_t i = 0; i < Grid.Rows(); i++, y -= h)
       {
          x = -1.0;
          for (std::size_t j = 0; j < Grid.Cols(); j++, x += w)
          {
             Cell* cell = Grid.GetCell(i, j);
             int idx = cell->GetTextureIndex();

             glTexCoord2f(tex_coords[idx].min_x, tex_coords[idx].max_y); glVertex2f(x,   y);
             glTexCoord2f(tex_coords[idx].max_x, tex_coords[idx].max_y); glVertex2f(x+w, y);
             glTexCoord2f(tex_coords[idx].max_x, tex_coords[idx].min_y); glVertex2f(x+w, y-h);
             glTexCoord2f(tex_coords[idx].min_x, tex_coords[idx].min_y); glVertex2f(x,   y-h);

             if (cell->GetSelected())
             {
                selected = true;
                sel_row = i;
                sel_col = j;
             }
          }
       }

   glEnd();

   // show selected cell
   if (selected)
   {
      x = -1.0 + (w*sel_col);
      y = 1.0 - (h*sel_row);
      glDisable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, 0);
      glColor4f(0.8, 0.8, 0.8, 0.75);
      glBegin(GL_QUADS);
          glVertex2f(x,   y);
          glVertex2f(x+w, y);
          glVertex2f(x+w, y-h);
          glVertex2f(x,   y-h);
      glEnd();
   }
}


void show_usage()
{
   std::cout << "Usage:" << std::endl;
   std::cout << "  ./main -r 40 -c 30 -t 1" << std::endl;
   std::cout << "    r: Number of rows (4-100)" << std::endl;
   std::cout << "    c: Number of columns (4-100)" << std::endl;
   std::cout << "    t: Maze type" << std::endl;
}

int main(int argc, char** argv)
{
   const float dt = 1.0 / 60.0;
   int option;
   int type = MAZE_BINARY;
   int rows = 4;
   int cols = 4;

   while ((option = getopt(argc, argv, "ht:r:c:")) != -1)
   {
      switch (option)
      { 
         case 't':
            type = atoi(optarg);
            if (type < 0 || type >= MAZE_NUM_TYPES)
               type = MAZE_BINARY;
            break;
         case 'r':
            rows = atoi(optarg);
            if (rows < 4 || rows >= 100)
               rows = 4;
            break;
         case 'c':
            cols = atoi(optarg);
            if (cols < 4 || cols >= 100)
               cols = 4;
            break;
         case 'h':
            show_usage();
            return 0;
      }
   }

   std::cout << "Generating " << rows << "x" << cols << " maze of type " << type << std::endl << std::endl;

   Grid grid(rows, cols);

   MAZE_PROFILE_BEGIN_SESSION("Maze Generation", "maze_results.json");

   if (type == MAZE_BINARY)
   {
      MAZE_PROFILE_SCOPE("BinaryTree");
      BinaryTree::Generate(grid);
   }
   else if (type == MAZE_SIDEWINDER)
   {
      MAZE_PROFILE_SCOPE("Sidewinder");
      Sidewinder::Generate(grid);
   }

   //std::cout << grid << std::endl;

   MAZE_PROFILE_END_SESSION();

   // determine texture index for each cell
   for (std::size_t i = 0; i < grid.Size(); i++)
   {
      Cell* cell = grid.GetCell(i);

      bool north_linked = cell->NorthLinked();
      bool south_linked = cell->SouthLinked();
      bool east_linked = cell->EastLinked();
      bool west_linked = cell->WestLinked();

      if (north_linked && east_linked && south_linked && west_linked)
         cell->SetTextureIndex(0);
      else if (north_linked && east_linked && !south_linked && west_linked)
         cell->SetTextureIndex(1);
      else if (north_linked && !east_linked && south_linked && west_linked)
         cell->SetTextureIndex(2);
      else if (north_linked && !east_linked && !south_linked && west_linked)
         cell->SetTextureIndex(3);
      else if (!north_linked && east_linked && south_linked && west_linked)
         cell->SetTextureIndex(4);
      else if (!north_linked && east_linked && !south_linked && west_linked)
         cell->SetTextureIndex(5);
      else if (!north_linked && !east_linked && south_linked && west_linked)
         cell->SetTextureIndex(6);
      else if (!north_linked && !east_linked && !south_linked && west_linked)
         cell->SetTextureIndex(7);
      else if (north_linked && east_linked && south_linked && !west_linked)
         cell->SetTextureIndex(8);
      else if (north_linked && east_linked && !south_linked && !west_linked)
         cell->SetTextureIndex(9);
      else if (north_linked && !east_linked && south_linked && !west_linked)
         cell->SetTextureIndex(10);
      else if (north_linked && !east_linked && !south_linked && !west_linked)
         cell->SetTextureIndex(11);
      else if (!north_linked && east_linked && south_linked && !west_linked)
         cell->SetTextureIndex(12);
      else if (!north_linked && east_linked && !south_linked && !west_linked)
         cell->SetTextureIndex(13);
      else if (!north_linked && !east_linked && south_linked && !west_linked)
         cell->SetTextureIndex(14);
      else if (!north_linked && !east_linked && !south_linked && !west_linked)
         cell->SetTextureIndex(14);
   }

   // initialize SDL
   if (SDL_Init(SDL_INIT_VIDEO) < 0)
   {
       std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
       quit = true;
   }
   else
   {
      // use OpenGL 2.1
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

      // create window
      window_width = cols*20;
      window_height = rows*20;
      std::cout << "Creating SDL window width " << window_width << " height " << window_height << std::endl;
      window = SDL_CreateWindow("MAZE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

      if (window == nullptr)
      {
         std::cout << "SDL Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
         quit = true;
      }
      else
      {
         screen_surface = SDL_GetWindowSurface(window);

         // create OpenGL context
         context = SDL_GL_CreateContext(window);

         // initialize OpenGL
         glMatrixMode(GL_PROJECTION);
         glLoadIdentity();

         glMatrixMode(GL_MODELVIEW);
         glLoadIdentity();

         glClearColor(0.0, 0.0, 0.0, 1.0);

         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
   }

   // load texture image
   int width;
   int height;
   int bpp;
   unsigned char* image = stbi_load("./4bit_road_tiles.png", &width, &height, &bpp, 4);

   glGenTextures(1, &texture_id);
   glBindTexture(GL_TEXTURE_2D, texture_id);
 
   int mode = GL_RGB;
 
   if (bpp == 4)
      mode = GL_RGBA;
 
   glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode, GL_UNSIGNED_BYTE, image);
 
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   if (image)
      stbi_image_free(image);

   while (!quit)
   {
      SDL_Delay(dt * 1000);

      handle_events(grid);

      render(grid);

      SDL_GL_SwapWindow(window);
   }

   SDL_DestroyWindow(window);
   SDL_Quit();

   return 0;
}
