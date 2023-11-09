#pragma once

#include <iostream>
#include "Cell.h"

class Grid
{
public:

   Grid(std::size_t rows, std::size_t cols)
      : mRows (rows),
        mCols (cols)
   {
      PrepareGrid();
      InitializeCells();
   }

   ~Grid()
   {
      delete [] mCells;
   }

   void Clear()
   {
      for (std::size_t i = 0; i < Size(); i++)
         mCells[i].UnlinkAll();
   }

   Cell* GetCell(std::size_t index) const
   {
      if (index < 0) return nullptr;
      if (index >= mRows * mCols) return nullptr;

      return &mCells[index];
   }

   Cell* GetCell (std::size_t row, std::size_t col) const
   {
      if (row < 0) return nullptr;
      if (row >= mRows) return nullptr;
      if (col < 0) return nullptr;
      if (col >= mCols) return nullptr;

      std::size_t idx = row + col*mRows;

      return GetCell(idx);
   }

   std::size_t Size() const { return mRows * mCols; }
   std::size_t Rows() const { return mRows; }
   std::size_t Cols() const { return mCols; }

private:

   void InitializeCells()
   {
      for (std::size_t i = 0; i < mRows; i++)
      {
         for (std::size_t j = 0; j < mCols; j++)
         {
            std::size_t idx = i + j*mRows;

            Cell* north = GetCell(i-1, j);
            Cell* south = GetCell(i+1, j);
            Cell* east = GetCell(i, j+1);
            Cell* west = GetCell(i, j-1);

            mCells[idx].Prepare(i, j, north, south, east, west);
         }
      }
   }

   void PrepareGrid()
   {
      mCells = new Cell[mRows * mCols];
   }

   std::size_t mRows;
   std::size_t mCols;
   Cell* mCells;
};

std::ostream& operator<<(std::ostream& os, const Grid& grid)
{
   for (std::size_t i = 0; i < grid.Rows(); i++)
   {
      for (std::size_t j = 0; j < grid.Cols(); j++)
      {
         if (i == 0)
         {
            if (j == grid.Cols()-1)
               os << "+---+";
            else
               os << "+---";
         }
         else
         {
            if (grid.GetCell(i, j)->NorthLinked())
            {
               if (j == grid.Cols()-1)
                  os << "+   +";
               else
                  os << "+   ";
            }
            else
            {
               if (j == grid.Cols()-1)
                  os << "+---+";
               else
                  os << "+---";
            }
         }
      }

      os << "\n";

      for (std::size_t j = 0; j < grid.Cols(); j++)
      {
         if (j == 0)
         {
            os << "|   ";
         }

         if (grid.GetCell(i, j)->EastLinked())
         {
            os << "    ";
         }
         else
         {
            os << "|   ";
         }
      }

      os << "\n";

      if (i == grid.Rows()-1)
      {
         for (std::size_t j = 0; j < grid.Cols(); j++)
         {
            if (j == grid.Cols()-1)
               os << "+---+";
            else
               os << "+---";
         }
         os << "\n";
      }
   }

   return os;
}
