#pragma once

#include "Grid.h"
#include "Random.h"
#include "Instrumentor.h"

class BinaryTree
{
public:

   static void Generate(Grid& grid)
   {
      for (std::size_t i = 0; i < grid.Size(); i++)
      {
         MAZE_PROFILE_SCOPE("Generate");
         Cell* cell = grid.GetCell(i);

         // if north and east neighbors, randomly pick one
         if (cell->GetNorth() && cell->GetEast())
         {
            double random_value = Random::Generate();

            if (random_value > 0.5)
               cell->Link(cell->GetNorth(), true);
            else
               cell->Link(cell->GetEast(), true);
         }
         // if no north neighbor, link to east neighbor
         else if (cell->GetEast())
            cell->Link(cell->GetEast(), true);
         // if no east neighbor, link to north neighbor
         else if (cell->GetNorth())
            cell->Link(cell->GetNorth(), true);
      }
   }

};
