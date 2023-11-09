#pragma once

#include "Instrumentor.h"

const std::size_t max_links = 4;

class Cell
{
public:

   Cell()
      : mRow (0),
        mCol (0),
        mNorth (nullptr),
        mSouth (nullptr),
        mEast (nullptr),
        mWest (nullptr),
        mTextureIndex (-1),
        mSelected (false)
   {
   }

   void Prepare(std::size_t row, std::size_t col, Cell* north, Cell* south, Cell* east, Cell* west)
   {
      mRow = row;
      mCol = col;
      mNorth = north;
      mSouth = south;
      mEast = east;
      mWest = west;

      for (std::size_t i = 0; i < max_links; i++)
         mLinks[i] = nullptr;

   }

   void Link(Cell* cell, bool bidirectional = false)
   {
      MAZE_PROFILE_FUNCTION();
      bool found = false;

      // add if not currently in link list
      for (std::size_t i = 0; i < max_links; i++)
      {
         if (mLinks[i] == cell)
         {
            found = true;
            break;
         }
      }

      if (!found)
      {
         for (std::size_t i = 0; i < max_links; i++)
         {
            if (mLinks[i] == nullptr)
            {
               mLinks[i] = cell;
               break;
            }
         }
      }

      // link back to cell
      if (bidirectional)
         cell->Link(this);
   }

   void Unlink(Cell* cell, bool bidirectional = false)
   {
      MAZE_PROFILE_FUNCTION();

      // remove cell if found in link list
      for (std::size_t i = 0; i < max_links; i++)
      {
         if (mLinks[i] == cell)
         {
            mLinks[i] = nullptr;
            break;
         }
      }

      // unlink back to cell
      if (bidirectional)
         cell->Unlink(this);
   }

   void UnlinkAll()
   {
      for (std::size_t i = 0; i < max_links; i++)
         mLinks[i] = nullptr;
   }

   Cell* GetNorth() { return mNorth; }
   Cell* GetSouth() { return mSouth; }
   Cell* GetEast() { return mEast; }
   Cell* GetWest() { return mWest; }

   bool NorthLinked()
   {
      bool found = false;

      for (std::size_t i = 0; i < max_links; i++)
      {
         if (mLinks[i] == mNorth && mNorth)
         {
            found = true;
            break;
         }
      }

      return found;
   }

   bool EastLinked()
   {
      bool found = false;

      for (std::size_t i = 0; i < max_links; i++)
      {
         if (mLinks[i] == mEast && mEast)
         {
            found = true;
            break;
         }
      }

      return found;
   }

   bool SouthLinked()
   {
      bool found = false;

      for (std::size_t i = 0; i < max_links; i++)
      {
         if (mLinks[i] == mSouth && mSouth)
         {
            found = true;
            break;
         }
      }

      return found;
   }

   bool WestLinked()
   {
      bool found = false;

      for (std::size_t i = 0; i < max_links; i++)
      {
         if (mLinks[i] == mWest && mWest)
         {
            found = true;
            break;
         }
      }

      return found;
   }

   std::size_t GetTextureIndex() const { return mTextureIndex; }

   void SetTextureIndex(std::size_t Index)
   {
      mTextureIndex = Index;
   }

   bool GetSelected() const { return mSelected; }

   void SetSelected(bool Selected)
   {
      mSelected = Selected;
   }

private:

   std::size_t mRow;
   std::size_t mCol;
   Cell*       mLinks[max_links];
   Cell*       mNorth;
   Cell*       mSouth;
   Cell*       mEast;
   Cell*       mWest;
   std::size_t mTextureIndex;
   bool        mSelected;
};
