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
#include "ParticleHistory.hpp"

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

#define kNumSteps 100000;

int main(int argc, const char * argv[]) {
    // Use SI units
    const double kTimeStep = 1e-26;// 50 steps per wavelength - keep errors small, then open things up.
    
    
    ParticleHistory electron(kNumSteps);
    electron.mass = ;
    electron.frequency = ;
    ParticleHistory proton(kNumSteps);
    proton.mass = ;
    
    // initial conditions, velocity, position, etc.
    proton.setState(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
    electron.setState(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
    
    
    
    // CalculateNextParticlePosition - can use GCD to dispatch out the jobs of determining the field. Say we need 50,000 steps, make 5 dispatch_async of 10,000 each
    for (long count = 0; count < kNumSteps; count++)
    {
        double labTime = count*kTimeStep;
        
        // These dispatch groups run the job on two cpus.
        dispatch_group_t grp = dispatch_group_create();
        dispatch_queue_t q = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
        dispatch_group_async(grp, q, ^{
            electron.CalculateNextParticlePosition(proton, labTime); // Adds up contributions from all fields then advances position, velocity, proper time, etc
        });
        dispatch_group_async(grp, q, ^{
            proton.CalculateNextParticlePosition(electron, labTime);
        });
        dispatch_group_wait(grp, DISPATCH_TIME_FOREVER);
        dispatch_release(grp);
        
    }
    
    
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
