//
//  ParticleHistory.hpp
//  d4
//
//  Created by Tom Andersen on 2016-11-21.
//  Copyright © 2016 Tom Andersen. All rights reserved.
//

#ifndef ParticleHistory_hpp
#define ParticleHistory_hpp

#include <stdio.h>
#include "ScalarField.hpp"

/// The idea is that there is one history object for each particle.
/// x,y,z,t,vx, vy, vz
///
/// If we use lab coordinates, then time will not reflect the time on the particle.
/// It seems to get the emission time we would have to integrate back over the entire path,
/// so that the phase at emission would be correct.
///
///

/// The idea is we need to keep a history of the particles.
/// With a EmQm system, we need to let the particle interact with waves from its past - the memory effect. (only makes sense)
/// So I need a history.

/// When a particle is at a position, it can see waves from its past and the past of all the other particles around it.
/// So for a particle at a point we need to integrate over all positions in the history of the particle(s).
/// We need the sum over the history taking into account radius and phase at emission.
/// having x,y,z,t,vx,vy,vz over determines the particle's state. Perhaps only need xyzt? Or x,y,z,t,vx,vy,vz and T (proper time on the particle).

/// Velocity affects the observed frequency (through lorentz)
/// So with proper time at the particle and velocity, we can get phase and distance from the lead.

/// We will want long particle trails.
/// Use 1D zfp compression. Perhaps compression not needed?

typedef struct {
    double x; // lab coords
    double y;
    double z;
    double vx; // lab speed
    double vy;
    double vz;
    double t; // lab time -- or is this a deltat time step - no - not a constant delta t, may want to vary time steps
    double T; // particle proper time.
    double A; // amplitude changes - amplitude is simply equal to the mearsured amplitude from the previous step, so the particle conserves energy.
} ParticleState;



/// Computational time - we need to add up from 2 particle histories, say a million steps back in history (given suitable steps)
class ParticleHistory {
    public:
    ParticleState* states;
        long maxLength;        // max number of histories to store. 1 million histories is 8 doubles x 8 bytes x 1 million = 64 MB. So length is something like 1M - 100Million.
        
        long stateCount;    // how many states we have stored.
        double mass;        // electron or proton
        double frequency;   // compton ish
        double amplitude;   // 0 --> 1 how much mass the particle exhanges each cycle? - fractional mass loss, etc?
        double initialPhase;
    public:
        ParticleHistory(long length); // malloc the arrays. calloc even
        void CalculateNextParticlePosition(ParticleHistory* other);
    
        void AddState(const ParticleState* intialState);
        void AddStateFromScalarField(const ScalarField* inField, double labT);
    
        void StateAt(ParticleState* outState, double labTime); // interpolates, filling in the passed state.
        void AddToScalarFieldAtPoint(ScalarField* ioField, const ParticleState* atPoint, double labT);

};


#endif /* ParticleHistory_hpp */
