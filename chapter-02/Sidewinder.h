#pragma once

#include "Grid.h"
#include "Random.h"
#include <vector>

class Sidewinder
{
public:

   static void Generate(Grid& grid)
   {
      for (std::size_t i = 0; i < grid.Rows(); i++)
      {
         std::vector<Cell*> run;

         for (std::size_t j = 0; j < grid.Cols(); j++)
         {
            MAZE_PROFILE_SCOPE("Generate");
            double random_value = Random::Generate();
            Cell* cell = grid.GetCell(i, j);

            run.push_back(cell);

            bool at_eastern_boundary = !cell->GetEast();
            bool at_northern_boundary = !cell->GetNorth();
            bool should_close_out =  at_eastern_boundary || random_value > 0.5;

            if (should_close_out && !at_northern_boundary)
            {
               // randomly pick a cell in the run to close out
               int idx = (int)(Random::Generate() * ((float)run.size()));

               run[idx]->Link(run[idx]->GetNorth(), true);

               run.clear();
            }
            else if (!at_eastern_boundary)
            {
               cell->Link(cell->GetEast(), true);
            }
         }
      }
   }

};
