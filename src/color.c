/*

Gravit - A gravity simulator
Copyright 2003-2005 Gerald Kaszuba

Gravit is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Gravit is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Gravit; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include "gravit.h"

const char *colourModes[] = { "mass", "velocity", "acceleration", "kinetic energy", "momentum" };

#ifndef NO_GUI

void setColoursByVel() {

    int i;
    particle_t *p;
    particleDetail_t *pd;
    float d;
    float velMax = 0;
    float velSpeed;

    VectorNew(zero);
    VectorZero(zero);

    // works out the highest velocity
    for (i = 0; i < state.particleCount; i++) {

        p = getParticleCurrentFrame(i);
        distance(zero, p->vel, velSpeed);
        velSpeed = fabs(velSpeed);

        if (i == 0) {
            velMax = velSpeed;
        } else {
            if (velSpeed > velMax)
                velMax = velSpeed;
        }

    }

    // applies velocity based on the highest
    for (i = 0; i < state.particleCount; i++) {

        p = getParticleFirstFrame(i);
        pd = getParticleDetail(i);

        distance(zero, p->vel, velSpeed);

        d = velSpeed / velMax;
        colourFromNormal(pd->col, (float)fabs((double)d));
        pd->particleTexture = colourSprite(pd->col, pd->mass);

    }

}

void setColoursByKinetic() {

    int i;
    particle_t *p;
    particleDetail_t *pd;
    float d;
    float kinMax = 0;
    float kinValue;
    float velocity;

    VectorNew(zero);
    VectorZero(zero);

    for (i = 0; i < state.particleCount; i++) {

        p = getParticleCurrentFrame(i);
        pd = getParticleDetail(i);

        distance(zero, p->vel, velocity);
        velocity = fabs(velocity);
        kinValue = velocity * velocity * pd->mass * 0.5;

        if (i == 0) {

            kinMax = kinValue;

        } else {

            if (kinValue > kinMax)
                kinMax = kinValue;

        }

    }

    for (i = 0; i < state.particleCount; i++) {

        p = getParticleFirstFrame(i);
        pd = getParticleDetail(i);

        distance(zero, p->vel, velocity);
        kinValue = velocity * velocity * pd->mass * 0.5;

        d = kinValue / kinMax;
        colourFromNormal(pd->col, (float)fabs((double)d));
        pd->particleTexture = colourSprite(pd->col, pd->mass);

    }

}

void setColoursByMomentum() {

    int i;
    particle_t *p;
    particleDetail_t *pd;
    float d;
    float kinMax = 0;
    float kinValue;
    float velocity;

    VectorNew(zero);
    VectorZero(zero);

    for (i = 0; i < state.particleCount; i++) {

        p = getParticleCurrentFrame(i);
        pd = getParticleDetail(i);

        distance(zero, p->vel, velocity);
        velocity = fabs(velocity);
        kinValue = velocity * pd->mass;

        if (i == 0) {

            kinMax = kinValue;

        } else {

            if (kinValue > kinMax)
                kinMax = kinValue;

        }

    }

    for (i = 0; i < state.particleCount; i++) {

        p = getParticleFirstFrame(i);
        pd = getParticleDetail(i);

        distance(zero, p->vel, velocity);
        kinValue = velocity * pd->mass;

        d = kinValue / kinMax;
        colourFromNormal(pd->col, (float)fabs((double)d));
        pd->particleTexture = colourSprite(pd->col, pd->mass);

    }

}

void setColoursByAcceleration() {

    int i;
    particle_t *p, *plast;
    particleDetail_t *pd;
    float d;
    float accMax = 0;
    float accCurrent;
    float velSpeed1;
    float velSpeed2;

    VectorNew(zero);
    VectorZero(zero);

    if (state.currentFrame == 0)
        return;

    for (i = 0; i < state.particleCount; i++) {

        p = getParticleCurrentFrame(i);
        plast = state.particleHistory + state.particleCount * (state.currentFrame-1) + i;
        distance(zero, p->vel, velSpeed1);
        distance(p->vel, plast->vel, velSpeed2);
        accCurrent = abs(velSpeed2 - velSpeed1);

        if (i == 0) {

            accMax = accCurrent;

        } else {

            if (accCurrent > accMax)
                accMax = accCurrent;

        }

    }

    for (i = 0; i < state.particleCount; i++) {

        p = getParticleCurrentFrame(i);
        plast = state.particleHistory + state.particleCount * (state.currentFrame-1) + i;
        distance(zero, p->vel, velSpeed1);
        distance(p->vel, plast->vel, velSpeed2);
        accCurrent = velSpeed2 - velSpeed1;
        pd = getParticleDetail(i);

        d = accCurrent / accMax;
        colourFromNormal(pd->col, (float)fabs((double)d));
        pd->particleTexture = colourSprite(pd->col, pd->mass);

    }

}


void setColoursByMass() {

    int i;
    particle_t *p;
    particleDetail_t *pd;
    float d;

    for (i = 0; i < state.particleCount; i++) {

        p = getParticleCurrentFrame(i);
        pd = getParticleDetail(i);

        if (i == 0) {

            state.massRange[0] = pd->mass;
            state.massRange[1] = pd->mass;

        } else {

            if (pd->mass < state.massRange[0])
                state.massRange[0] = pd->mass;

            if (pd->mass > state.massRange[1])
                state.massRange[1] = pd->mass;

        }

    }

    for (i = 0; i < state.particleCount; i++) {

        p = getParticleCurrentFrame(i);
        pd = getParticleDetail(i);

        d = pd->mass / state.massRange[1];
        colourFromNormal(pd->col, (float)fabs(d));

        if (d < 0) {

            pd->col[0] = 1 - pd->col[0];
            pd->col[1] = 1 - pd->col[1];
            pd->col[2] = 1 - pd->col[2];

        }
        pd->particleTexture = colourSprite(pd->col, pd->mass);
    }

}

void setColours() {

	if(state.particleHistory == NULL) return;

	switch (view.particleColourMode) {

    case CM_MASS:
    default:
        setColoursByMass();
        break;

    case CM_VEL:
        setColoursByVel();
        break;

    case CM_ACC:
        setColoursByAcceleration();
        break;

    case CM_KIN:
        setColoursByKinetic();
        break;

    case CM_MOM:
        setColoursByMomentum();
        break;
    }

}

void colourSpectrumClear() {

    if (view.colourSpectrum) {
        free(view.colourSpectrum);
        view.colourSpectrum = 0;
    }

    view.colourSpectrumSteps = 0;

}

void colourFromNormal(float *c, float n) {

    int i;
    int bits;

    if (n < 0)
        n = 0;
    if (n > 1)
        n = 1;

    if (view.colourSpectrumSteps == 0 || view.colourSpectrum == 0) {
        c[0] = c[1] = c[2] = c[3] = 1;
        return;
    }

    if (view.colourSpectrumSteps == 1) {
        c[0] = view.colourSpectrum[0];
        c[1] = view.colourSpectrum[1];
        c[2] = view.colourSpectrum[2];
        c[3] = view.colourSpectrum[3];
        return;
    }

    bits = view.colourSpectrumSteps - 1;

    for (i = 0; i < bits; i++) {

        if (n <= (float)(i+1) / bits) {

            float j;
            float l,h;
            int col;

            j = (n - (float)i / bits) * (float)bits;

            for (col = 0; col < 4; col++) {

                if (view.colourSpectrum[i*4+col] < view.colourSpectrum[(i+1)*4+col]) {
                    l = view.colourSpectrum[i*4+col];
                    h = view.colourSpectrum[(i+1)*4+col];
                    c[col] = (h - l) * j + l;
                } else {
                    l = view.colourSpectrum[(i+1)*4+col];
                    h = view.colourSpectrum[i*4+col];
                    c[col] = (h - l) * (1-j) + l;
                }

            }

            break;

        }

    }

    if (view.stereoMode == 2) {
	  // reduce to "half-color": red = 0,299*red + 0,587*green + 0,114*blue
	  c[0] = 0.299*c[0] + 0.587*c[1] + 0.144*c[2];
    }

    // more opacity
    if ((view.glow == 1) || (view.glow == 2) || (view.glow > 7))
      c[3] = sqrtf(c[3]);
}


GLuint colourSprite(float *c, float mass) {

  if (view.glow == 0) return(particleTextureID);

  if ((view.glow < 5) && (view.particleRenderMode == 1)) return (particleTextureID_gray2);

  // add starshine effect
  // color by mass --> above 90% of highest mass
  if((particleTextureID_glow != 0) && (view.particleColourMode == CM_MASS) && (fabs(mass) > fabs(state.massRange[1] * 0.9)))
      return(particleTextureID_glow);
  // not coloring by mass --> add effect for the brightest particles
  if((particleTextureID_glow != 0) && (view.particleColourMode != CM_MASS) && (fabs(c[3]) > 0.85))
      return(particleTextureID_glow);

  // stereo mode --> use grayscale particle
  if (view.stereoMode == 2)
    return(particleTextureID_gray);


  // select texture template based on dominant color
  //   blue
  if ((c[2] > c[1]) && (c[2] > c[0]) && (particleTextureID_blue != 0))
    return(particleTextureID_blue);

  //   green
  if ((c[1] > c[0]) && (particleTextureID_green != 0))
    return(particleTextureID_green);

  //   red
  if (particleTextureID_red != 0)
    return(particleTextureID_red);
  else
    return(particleTextureID);

}

#endif
