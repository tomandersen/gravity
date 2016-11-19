//
//  main.cpp
//  field-energy
//
//  Created by Tom Andersen on 2016-11-13.
//  Copyright © 2016 Tom Andersen. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <dispatch/dispatch.h>

// big enough to hold all the answers, makes things easier.
#define kMaxEnergyPoints 1000

// Calculate Energy of field of two wave sources each the same, phase adjustable
// phase
// power
// speed of light
// number of points.
// distance between. d
// setup is xyz coordinates
// one source at -d/2, the other at d/2 on x axis.
int main(int argc, const char * argv[]) {
    // insert code here...
    
    
    const double phaseA = 0;
    const double phaseB = 0.0; // M_PI/4;
    const double amplitudeA = 1; //sqrt(2);
    const double amplitudeB = 1;
    const double speedOfLight = 1.0;
    const double freq = 3.0;
    const double waveK = freq/speedOfLight;
    
    
    // Symmetry - only need positive Y
    // only need positive Z
    // More symmetry happens if phase matches, etc, but we want to vary phase
    // We carefully put the sources in the middle of the line of cubes along the x axis.
    // So the cubes are setup not to hit x = 0, y = 0 or z = 0 with offsets
    
    const double cubeSize = 0.1347;
    const double halfCube = cubeSize/2.0;
    
    long numToDo = 100;
    long gridSize = 2*numToDo;
    
    double minX = -gridSize*cubeSize;
    double maxX = gridSize*cubeSize;
    double minY = halfCube;
    double maxY = gridSize*cubeSize;
    double minZ = halfCube;
    double maxZ = gridSize*cubeSize;
    
    //double timeStep = M_PI/17.8/freq;
    //    double timeStep = M_PI/73.88765/freq;
    double timeStep = M_PI/9.1876565/freq; // speed up - use fewer time steps
    double theEnergy[numToDo];
    double* energy = theEnergy;
    
    dispatch_group_t grp = dispatch_group_create();
    dispatch_queue_t q = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    for (long counter = 0; counter < numToDo; counter ++)
    {
        dispatch_group_async(grp, q, ^{
            
            double xA = -counter*cubeSize - halfCube;
            double xB = counter*cubeSize + halfCube;
            double dist = xB - xA;
            double energyAtThisDist = 0.0;
            long totalPoints = 0;
            for (double time = - M_PI/freq; time < M_PI/freq; time+= timeStep)
            {
                // symmetry implies only need positive Z
                for (double x = minX; x < maxX; x+= cubeSize)
                {
                    for (double y = minY; y < maxY; y+= cubeSize)
                    {
                        for (double z = minZ; z < maxZ; z+= cubeSize)
                        {
                            double netXA = xA - x;
                            double netXB = xB - x;
                            double distA = sqrt(netXA*netXA + y*y + z*z);
                            double distB = sqrt(netXB*netXB + y*y + z*z);
                            double ampFromA = amplitudeA/distA*sin(phaseA + freq*time - distA*waveK);
                            double ampFromB = amplitudeB/distB*sin(phaseB + freq*time - distB*waveK);
                            double totalAmp = ampFromA + ampFromB;
                            double energyHere = totalAmp*totalAmp;
                            energyAtThisDist += energyHere;
                            totalPoints++;
                        }
                    }
                }
            }
            
            std::cout << "Total Field energy " << energyAtThisDist/totalPoints <<  ", for Dist = " << dist <<  ", points used = " << totalPoints << "\n";
            energy[counter] = energyAtThisDist/totalPoints;
        });
    }
    
    dispatch_group_wait(grp, DISPATCH_TIME_FOREVER);
    if (phaseB != 0.0)
        printf("freq: %d c: %d phase PI/%d calcSize %d gridSize %d, energy\n", (int) freq, (int)speedOfLight, (int)(M_PI/phaseB), (int)numToDo, (int)gridSize*2);
    else
        printf("freq: %d c: %d calcSize %d gridSize %d, energy\n", (int) freq, (int)speedOfLight, (int)numToDo, (int)gridSize*2);

    for (long count = 0; count < numToDo; count++)
    {
        double xA = -count*cubeSize - halfCube;
        double xB = count*cubeSize + halfCube;
        double dist = xB - xA;
        std::cout << dist << "," << energy[count] << "\n";
    }
    
    dispatch_release(grp);
    return 0;
}
