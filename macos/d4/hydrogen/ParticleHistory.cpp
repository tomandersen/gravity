//
//  ParticleHistory.cpp
//  d4
//
//  Created by Tom Andersen on 2016-11-21.
//  Copyright © 2016 Tom Andersen. All rights reserved.
//

#include "Utils.hpp"
#include "ParticleHistory.hpp"
#include "ScalarField.hpp"
#include "PhysicalConstants.h"

const double kPointsPerWaveLength = 20.0;
const double kTimeStep = 1.0/kPointsPerWaveLength;



ParticleHistory::ParticleHistory(long inMaxLength) {
    // for better cache coherence we should use particle positions in a struct.
    // In order to compress we need to use a seperate stream for x,y,z, but we only do that once.s
    states = (ParticleState*)malloc(inMaxLength*sizeof(ParticleState));
    
    //1 million histories is 8 doubles x 8 bytes x 1 million = 64 MB. So length is something like 1M - 100Million.
    maxLength = inMaxLength;        // max number of histories to store.
    stateCount = 0;        // number of states stored.
    
    //These need to be set
    mass = 0;        // electron or proton
    frequency = 0; // compton ish ? same for all particles?
}


void ParticleHistory::CalculateNextParticlePosition(ParticleHistory* other)
{
    // the particle movement depends on all mass coming into and leaving it.
    // We need to look back in time to find the contributions of the other particle(s)
    // Along the history there are points in this particle's history and the other particles's history where
    // waves can reach us from our own previous emissions.
    
    // Waves reach us from our previous emissions by being reflected back off of (perhaps distant) emission points.
    // How could such a regime work to make a stable hydrogen atom? Note that in a real atom, energy levels are altered by nearby particles.
    
    // Yet it is our own emissions that we must interact with in order to produce quantum effects.
    
    // distant sphere of reflectors - particles
    // Look to a paper like Trajectory eigenmodes of an orbiting wave source E. FORT1 AND Y. COUDER2 - to see the kind of wave system I am talking about
    // 'inertial waves' - like any relativisitic source, radiate in a sphere independent of the velocity of the particle (at c) and also have doppler frequency effects, etc.
    
    // Particle Model:
    // The paricle is a super radiant capable geometric object - a naked Kerr singularity.  The Kerr ring singuarity is unstable wrt to pertubations.
    // A pertubation will result in a quadrupole moment, which causes the non - perfect ring to re - radiate incoming energy that has deformed it.
    // Thus the electron is basically a mirror pump for these Compton frequency gravitational waves. It is thus a standing wave source, where energy
    // is balanced in vs out.
    // The Kerr ring for an electron is very large, and as such rather loosely circular in shape - its actual shape may be more of a fractal like ring.
    // The fractal ring could then perhaps super - radiate (or 'balanced radiate') over a large range of frequencies from the fundamental lowest Compton frequency
    // on up a few orders of magnitude.
    
    // Protons are assumed to be on the same frequency as electrons, having a more complex structure - but having the same outward behavoir of being a standing
    // wave source at the electron compton frequency.
    
    // All of the above comes to a simple model of an inertial walker in the Fort/Couder sense, with energy balanced.
    // The EM force will hopefully arise as a side effect of this energy exhange (shades of QED).
    
    // When we want to see what the effects of a particle's standing waves are when the particle was at t' = (labTime - dt), we
    // need to look at a wave source NOT centered on where the particle was a t' but rather where the particle would be at labTime if it continued
    // on at its current velocity. That point is then taken as the wave source, with wave amplitude and pahse given by the distance the virtual emssion point is from
    // this particle, in the usual 1/r for amplitude 1/r**2 for energy, and speed of light considerations for phase.
    // Summing over the histories of all particles involved gives one the net amplitude (really we need amplitudes and first derviatives of the amplitudes).
    // The force is then the sum of these slopes at the particle location.
    
    const long kMaxNumStepsToLookBack = 10000;
    long stepsToLookBack = stateCount;
    if (stepsToLookBack > kMaxNumStepsToLookBack)
        stepsToLookBack = kMaxNumStepsToLookBack;
    
    // We need to add up all the contributions from each particle (ie this and the other particle
    //
    
    double timePerStep = kTimeStep/frequency;
    ScalarField netField;
    netField.A = 0.0;
    netField.dAdx = 0.0;
    netField.dAdy = 0.0;
    netField.dAdz = 0.0;
    for (long step = 0; step < stepsToLookBack; step++)
    {
        double labT = step*timePerStep;
        ParticleState* us = &(states[step]);
        this->AddToScalarFieldAtPoint(&netField, us, labT);
        other->AddToScalarFieldAtPoint(&netField, us, labT);
    }
    
    // we have added up all the field contributions from the past by ourselves and the other particle.
    // So add a new point based on the reported field strength:
    double labT = stateCount*timePerStep;
    this->AddStateFromScalarField(&netField, labT);
}

void ParticleHistory::AddState(const ParticleState* intialState) {
    states[stateCount] = *intialState;
    stateCount += 1;
}

// Given the passed ScalarField, we add on to our current state by calculating new position and velocities
void ParticleHistory::AddStateFromScalarField(const ScalarField* inField, double labT) {
    ParticleState* last = &(states[stateCount - 1]);
    ParticleState* newOne = &(states[stateCount]);
    double deltaT = labT - last->t;
    
    
    // To Do - calc the velocities...really
    newOne->vx = last->vx + inField->dAdx*deltaT/mass;
    newOne->vy = last->vy + inField->dAdy*deltaT/mass;
    newOne->vz = last->vz + inField->dAdz*deltaT/mass;
    
    newOne->x = last->x + newOne->vx*deltaT;
    newOne->y = last->y + newOne->vy*deltaT;
    newOne->z = last->z + newOne->vz*deltaT;
    
    newOne->A = inField->A;
    newOne->t = labT;
    newOne->T = labT; // To Do - calculate the actual proper Time on the particle using the previous time T + relativity lorentxz on deltaT
    
    stateCount += 1;
}

// interpolates, filling in the passed state.
void ParticleHistory::StateAt(ParticleState* outState, double labTime)
{
    double timePerStep = kTimeStep/frequency;
    long step = labTime/timePerStep;
    if ((step + 1) >= stateCount)
    {
        // printf("error ? time was in the future");
        *outState = states[stateCount - 1];
        return;
    }
    
    ParticleState* stateBefore = &(states[step]);
    ParticleState* stateAfter = &(states[step + 1]);
    double fraction = (labTime - step*timePerStep)/timePerStep;
    double oneMinusF = 1.0 - fraction;
    
    outState->x = stateBefore->x*oneMinusF + stateAfter->x*fraction;
    outState->y = stateBefore->y*oneMinusF + stateAfter->y*fraction;
    outState->z = stateBefore->z*oneMinusF + stateAfter->z*fraction;
    outState->vx = stateBefore->vx*oneMinusF + stateAfter->vx*fraction;
    outState->vy = stateBefore->vy*oneMinusF + stateAfter->vy*fraction;
    outState->vz = stateBefore->vz*oneMinusF + stateAfter->vz*fraction;
    outState->t = stateBefore->t*oneMinusF + stateAfter->t*fraction; // labTime it should be check in debugs
    outState->T = stateBefore->T*oneMinusF + stateAfter->T*fraction;
}

/// idea is to look at the inertial memory model, use phase and timing to get phase, radius for amplitude, then
// also calculate the slopes of said fields.
void ParticleHistory::AddToScalarFieldAtPoint(ScalarField* ioField, const ParticleState* atPoint, double labT)
{
    // do that
    
}

