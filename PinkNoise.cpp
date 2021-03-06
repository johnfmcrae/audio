/*
  ==============================================================================

    PinkNoise.cpp
    Created: 23 Jun 2020 1:31:08pm
    Last updated: 27 June 2020 5:51 pm
    Author:  John F. McRae

    Implements the McCartney-Voss algorithm based on Burk's implementation using the JUCE framework
    http://www.firstpr.com.au/dsp/pink-noise/phil_burk_19990905_patest_pink.c

    Generate Pink Noise using Gardner method (describes work by Voss).
    Optimization suggested by James McCartney uses a tree
    to select which random value to replace.

    x x x x x x x x x x x x x x x x
     x   x   x   x   x   x   x   x
       x       x       x       x
           x               x
                   x

  ==============================================================================
*/

#include "PinkNoise.h"

// constructor
PinkNoise::PinkNoise(int numRows)
{
    pinkIndex = 0;
    // some low level magic to effectively get you a binary string of 1's of equal length
    // to the max integer size
    pinkIndexMask = (1 << numRows) - 1;
    int pmax = numRows + 1;
    pinkNorm = 1.0 / pmax;
    // in testing, I found it was better to initialize the rows with noise
    // this avoids a climb up to some max value during the first run through the rows
    for (int i = 0; i < numRows; i++)
        pinkRows.push_back(noiseSrc.nextFloat());
    pinkRunSum = noiseSrc.nextFloat();
}

float PinkNoise::generate()
{
    float newRandom, sum, output;

    // increment and mask index
    pinkIndex = (pinkIndex + 1) & pinkIndexMask;

    // ensure pink index is not zero, if it is, do not update any of the random vals
    if (pinkIndex != 0)
    {
        // determine the number of trailing zeros in pinkIndex
        int numZeros = 0;
        int n = pinkIndex;
        while ((n & 1) == 0)
        {
            // bit shift until you run out of trailing zeros
            n = n >> 1;
            numZeros++;
        }

        // McCARTNEY-VOSS ALGORITHM
        // subtract prev. value from running sum
        pinkRunSum -= pinkRows[numZeros];
        // generate a new random number
        newRandom = noiseSrc.nextFloat();
        // add the new random number
        pinkRunSum += newRandom;
        // reaplce the row value at index numZeros with the new random value
        pinkRows[numZeros] = newRandom;
    }

    // add extra white noise value
    sum = pinkRunSum + noiseSrc.nextFloat();

    // scale and return value
    return (sum * pinkNorm);
}

// Changes the number of noise generating rows
// Note that this overrides the initialization found in the constructor
// AS WELL AS the pinkRows vector. Therefore, it is advised that this
// function only be called on initialization
void PinkNoise::setRows(int newRows)
{
    // reset pinkIndex
    pinkIndex = 0;
    // some low level magic to effectively get you a binary string of 1's of equal length
    // to the max integer size
    pinkIndexMask = (1 << newRows) - 1;
    int pmax = newRows + 1;
    pinkNorm = 1.0 / pmax;
    // clear the pinkRows vector
    pinkRows.clear();
    // reinitialize the pinkRows vector
    for (int i = 0; i < newRows; i++)
        pinkRows.push_back(noiseSrc.nextFloat());
    pinkRunSum = noiseSrc.nextFloat();
}