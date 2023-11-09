#pragma once

#include <random>

class Random
{
public:

   static double Generate()
   {
      std::random_device rd;
      std::mt19937 mt(rd());
      std::uniform_real_distribution<double> dist(0.0, 1.0);
   
      return dist(mt);
   }

};
