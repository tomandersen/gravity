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
#include "PhysicalConstants.h"

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

#define kNumSteps 100000

int main(int argc, const char * argv[]) {
    // Use SI units
    const double kTimeStep = 1e-26;// 50 steps per wavelength - keep errors small, then open things up.
    
    // Eventually, use an array of particles.
    ParticleHistory* electron = new ParticleHistory(kNumSteps);
    ParticleHistory* proton = new ParticleHistory(kNumSteps);
    
    // compton frequency
    electron->frequency = k__ComptonFrequency;
    proton->frequency = k__ComptonFrequency;
    
    electron->mass = k__m_e_kg;
    proton->mass = k__m_P_kg;
    
    // Set up proton at origin at rest
    ParticleState protonI;
    protonI.x = 0.0;
    protonI.y = 0.0;
    protonI.z = 0.0;
    protonI.vx = 0.0;
    protonI.vy = 0.0;
    protonI.vz = 0.0;
    protonI.t = 0.0;
    protonI.T = 0.0;
    protonI.A = 1.0;
    proton->ParticleHistory::AddState(&protonI);
    
    // Set up electron at origin at rest
    ParticleState electronI;
    electronI.x = k__HydrogenRadius;
    electronI.y = 0.0;
    electronI.z = 0.0;
    electronI.vx = 0.0;
    electronI.vy = 0.0;
    electronI.vz = 0.0;
    electronI.t = 0.0;
    electronI.T = 0.0;
    electronI.A = 1.0;
    electron->ParticleHistory::AddState(&electronI);
    
    // CalculateNextParticlePosition - can use GCD to dispatch out the jobs of determining the field. Say we need 50,000 steps, make 5 dispatch_async of 10,000 each
    // CalculateNextParticlePosition - output is proportional to input, waves in == waves out, but it scatters them.
    for (long count = 0; count < kNumSteps; count++)
    {
        //Future -  These dispatch groups run the job on two cpus.
        electron->CalculateNextParticlePosition(proton); // Adds up contributions from all fields then advances position, velocity, proper time, etc
        proton->CalculateNextParticlePosition(electron);
    }
    return 0;
}
