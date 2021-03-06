////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  https://BrutPitt.com
//
//  This software is distributed under the terms of the BSD 2-Clause license:
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//   
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <deque>
#include <vector>
#include <string>
#include <complex>

#include <thread>
#include <mutex>
#include <condition_variable>
//#include <omp.h>

//#include "nv/nvMath.h"
//#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <configuru/configuru.hpp>

#include <fastRandom.h>
#include "attractorsStartVals.h"


//void resetVBOindexes();


using namespace glm;
using namespace std;
using namespace configuru;


#define BUFFER_DIM 100

//#define RANDOM(MIN, MAX) ((MIN)+((float)rand()/(float)RAND_MAX)*((MAX)-(MIN)))


class attractorDlgClass;
class AttractorsClass;
class emitterBaseClass;

//  Attractor base class
////////////////////////////////////////////////////////////////////////////
class AttractorBase
{
public:

    typedef void (AttractorBase::*stepPtrFn)(vec3 &v, vec3 &vp);

    enum { attLoadPtVal, attLoadKtVal };
    enum { attHaveKVect, attHaveKScalar }; 
    enum { attPt3D = 3, attPt4D };

    AttractorBase() 
    {
        resetQueue();
        //srand((unsigned)time(NULL));
    }

    virtual ~AttractorBase() {}


    void Insert(const vec3 &vect)
    {
        stepQueue.push_front(vect);
        //deque <vec3>::size_type sz = stepQueue.size();
        stepQueue.pop_back();
    }

    virtual void resetQueue();

    virtual void initStep() = 0;
    virtual void newRandomValues() = 0;

    virtual void searchAttractor() {};
    void searchLyapunov();


    virtual void saveVals(const char *name) {}
    virtual void loadVals(const char *name) {}

    virtual void saveVals(Config &cfg);
    virtual void loadVals(Config &cfg);

    virtual void saveAdditionalData(Config &cfg) {}
    virtual void loadAdditionalData(Config &cfg) {}

    virtual int additionalDataDlg();
    virtual void additionalDataCtrls();

    virtual void saveKVals(Config &cfg) = 0;
    virtual void loadKVals(Config &cfg) = 0;

    virtual void startData() = 0;

    virtual float getValue(int i, int type) = 0;
    virtual float getValue(int x, int y, int type) = 0; 
    virtual void setValue(int i, int type, float val) = 0; 
    virtual void setValue(int x, int y, int type, float val) = 0; 
    virtual int getNumElements(int type) = 0; //return # rows

    virtual int getKType() = 0;

    //thread Step with shared GPU memory
    void Step(float *&ptr, vec3 &v, vec3 &vp);
    //single step
    virtual void Step();
    //buffered Step
    virtual void Step(float *ptr, int numElements);
    //attractor step algorithm
    virtual void Step(vec3 &v, vec3 &vp) = 0;


    vec3& getCurrent()  { return stepQueue.front(); }
    vec3& getPrevious() { return stepQueue[1]; }
    vec3& getAt(int i)  { return stepQueue[i]; }

    int getMagnetSize() { return vVal.size(); }

    vec3& getPOV() { return m_POV; }
    vec3& getTGT() { return m_TGT; }

    void setKMax(float v) { kMax = v; }
    void setKMin(float v) { kMin = v; }
    void setVMax(float v) { vMax = v; }
    void setVMin(float v) { vMin = v; }

    float getKMax() { return kMax; }
    float getKMin() { return kMin; }
    float getVMax() { return vMax; }
    float getVMin() { return vMin; }

    string& getDisplayName() { return displayName; }
    void setDisplayName(string &name) { displayName = name; }

    string& getNameID() { return nameID; }

//    void breakLoopOn() { breakLoop = true; }

    void setBufferRendered() {  bufferRendered = true; }

    int getEmittedParticles() { return emittedPoints; }
    uint &getRefEmittedParticles() { return emittedPoints; }
    void resetEmittedParticles() { emittedPoints = 0; }
    void incEmittedParticles() { emittedPoints++; }

    bool dlgAdditionalDataVisible() { return bDlgAdditionalDataVisible; }
    void dlgAdditionalDataVisible(bool b) { bDlgAdditionalDataVisible=b; }

    bool dtType() { return isDTtype; }
    bool fractalType() { return isFractal; }

    virtual int getPtSize() { return attPt3D; }
    float getDim4D() { return dim4D; }
    void setDim4D(float f) { dim4D = f; }

    vector<vec3> vVal;
protected:
    float dim4D = 0;

    //innerThreadStepPtrFn innerThreadStepFn;
    stepPtrFn stepFn;

    deque<vec3> stepQueue;

    // limit random generators
    float kMax, kMin, vMax, vMin;

    bool bDlgAdditionalDataVisible = false;
    

    vec3 m_POV, m_TGT = vec3(0.f, 0.f, 0.f);

    string displayName, fileName, nameID;

    friend class attractorDlgClass;
    friend class AttractorsClass;
    bool bufferRendered = false;
    uint emittedPoints = 0;

    bool flagFileData = false;
    bool isDTtype = false;
    bool isFractal = false;
private:

};

//  Attractors class with scalar K coeff
////////////////////////////////////////////////////////////////////////////
class attractorScalarK : public AttractorBase
{
public:

    //  Virtual funcs... overridable for exceptions
    ///////////////////////////////////////
    virtual void initStep() {
        resetQueue();
        Insert(vVal[0]);
    }

    virtual void newRandomValues() 
    {
            for (int i=0; i<kVal.size(); i++) kVal[i] = RANDOM(kMin,kMax);
            vVal[0] = vec3(RANDOM(vMin,vMax),RANDOM(vMin,vMax),RANDOM(vMin,vMax));
    }



    void clear() { kVal.clear(); vVal.clear(); }

    float getKParam(int i) { return kVal[i]; }


    void saveKVals(Config &cfg);
    void loadKVals(Config &cfg);

    float getValue(int i, int type)                  { return type ? kVal[i] : vVal[0][i]; }
    float getValue(int x, int y, int type)           { return type ? kVal[x] : vVal[x][y]; }
    void setValue(int i, int type, float val)        { type ? kVal[i]=val : vVal[0][i]=val; }
    void setValue(int x, int y, int type, float val) { type ? kVal[x]=val : vVal[x][y]=val; }

    int getNumElements(int type) {  return (type) ? kVal.size () : vVal.size(); } //return # rows


    int getKSize() { return kVal.size(); }
    int getKType() { return attHaveKScalar; }

protected:
    vector<float> kVal;

};

class attractorDtType : public attractorScalarK
{
protected:

    attractorDtType() {
        kMin = -5.0; kMax = 5.0; vMin = 0.0; vMax = 0.0;
        isDTtype = true;
    }

    virtual void additionalDataCtrls();

    virtual void saveAdditionalData(Config &cfg);
    virtual void loadAdditionalData(Config &cfg);
    // dTime step 
    float dtStepInc = 0.001f;


};



//  Attractors class with vector K coeff
////////////////////////////////////////////////////////////////////////////
class attractorVectorK : public AttractorBase
{
public:

    //  Virtual funcs... overridable for exceptions
    ///////////////////////////////////////
    virtual void initStep() {
        resetQueue();
        Insert(vVal[0]);
    }
    virtual void newRandomValues() 
    {
            for (auto &i : kVal) i = vec3(RANDOM(kMin,kMax),RANDOM(kMin,kMax),RANDOM(kMin,kMax));
            vVal[0] = vec3(RANDOM(vMin,vMax),RANDOM(vMin,vMax),RANDOM(vMin,vMax));
    }

    void clear() { kVal.clear(); vVal.clear(); }

    vec3& getKParam(int i) { return kVal[i]; }

    void saveKVals(Config &cfg);
    void loadKVals(Config &cfg);

    virtual float getValue(int i, int type) { return type ? kVal[0][i] : vVal[0][i]; }
    virtual float getValue(int row, int col, int type) { return type ? kVal[row][col] : vVal[row][col]; }
    virtual void setValue(int i, int type, float val) { 
        if (type) kVal[0][i]=val;
        else      vVal[0][i]=val;
    }
    virtual void setValue(int row, int col, int type, float val) { 
        if (type) kVal[row][col]=val;
        else      vVal[row][col]=val;
    }

    int getNumElements(int type) {  return (type) ? kVal.size () : vVal.size(); } //return # rows


    int getKSize() { return kVal.size(); }
    int getKType() { return attHaveKVect; }

protected:
    vector<vec3> kVal;

};

class fractalIIMBase : public attractorScalarK
{
public:

protected:
    fractalIIMBase() {
        vMin = 0.f; vMax = 0.f; kMin = 0.f; kMax = 0.f;
        m_POV = vec3( 0.f, 0, 7.f);
        isFractal = true; 
    }

    void maxDepthReached() {

    }

    virtual void preStep(vec3 &v) {
        if(depth++>maxDepth) {
            depth = 0;
/*
            last4D = dim4D +   FLOAT_RANDOM(vMin, vMax, fastRandom.SWB());
            v = vVal[0] + vec3(FLOAT_RANDOM(vMin, vMax, fastRandom.SWB()),
                               FLOAT_RANDOM(vMin, vMax, fastRandom.SWB()),
                               FLOAT_RANDOM(vMin, vMax, fastRandom.SWB()));
            
            kRnd = vec4(FLOAT_RANDOM(kMin, kMax, fastRandom.SWB()),
                        FLOAT_RANDOM(kMin, kMax, fastRandom.SWB()),
                        FLOAT_RANDOM(kMin, kMax, fastRandom.SWB()),
                        FLOAT_RANDOM(kMin, kMax, fastRandom.SWB()));
*/
            last4D = dim4D +   fastRandom.floatRnd(vMin, vMax);
            v = vVal[0] + vec3(fastRandom.floatRnd(vMin, vMax),
                               fastRandom.floatRnd(vMin, vMax),
                               fastRandom.floatRnd(vMin, vMax));
            
            kRnd = vec4(fastRandom.floatRnd(kMin, kMax),
                        fastRandom.floatRnd(kMin, kMax),
                        fastRandom.floatRnd(kMin, kMax),
                        fastRandom.floatRnd(kMin, kMax));
        } 
    }

    //  Personal vals
    ///////////////////////////////////////
    void saveAdditionalData(Config &cfg);
    void loadAdditionalData(Config &cfg);

    virtual void additionalDataCtrls();

    vec4 kRnd = vec4(0.f);
    vec4 vIter;
    std::vector<vec3> eqRoots;
    
    int maxDepth = 50;
    int degreeN = 2;
    float last4D = 1;

    int depth = 0;
private:
};


class fractalIIM_Nth : public fractalIIMBase
{
public:
    //  Personal vals
    ///////////////////////////////////////
    virtual void additionalDataCtrls();
protected:

};


class juliaBulb_IIM : public fractalIIMBase
{    
public:
    juliaBulb_IIM() { stepFn = (stepPtrFn) &juliaBulb_IIM::Step; }

protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};

class juliaBulb4th_IIM : public fractalIIM_Nth
{    
public:
    juliaBulb4th_IIM() { stepFn = (stepPtrFn) &juliaBulb4th_IIM::Step; }

protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};

class fractalIIM_4D : public fractalIIMBase
{
public:
    //void preStep(vec3 &v, vec3 &vp) { last4D = RANDOM(vMin, vMax); fractalIIMBase::preStep(v,vp); }
    int getPtSize() { return attPt4D; }

    virtual void initStep() {
        last4D = dim4D;
        attractorScalarK::initStep();
    }

    //  Personal vals
    ///////////////////////////////////////
    //void additionalDataCtrls();
protected:

};


class BicomplexBase : public fractalIIM_4D
{
public:
    typedef void (BicomplexBase::*magneticPtrFn)(const vec3 &, int);
    void startData();

    void radiciBicomplex(const vec4 &pt, vec3 &vp)
    {           
        const int rnd = xorshift64();
        const float sign1 = (rnd&1) ? 1.f : -1.f, sign2 = (rnd&2) ? 1.f : -1.f;
        const vec4 p(pt - ((vec4 &)*kVal.data()+kRnd));

        const std::complex<float> z1(p.x, p.y), z2(-p.w, p.z);
        const std::complex<float> w1 = sign1 * sqrt(z1 - z2), w2 = sign2 *sqrt(z1 + z2);
        vp = .5f * vec3(w1.real()+w2.real(), w1.imag()+w2.imag(), w2.imag()-w1.imag()/*, w1.real()-w2.real()*/);
        last4D =  .5f * (w1.real()-w2.real());
    };

};

/////////////////////////////////////////////////
class BicomplexJ_IIM : public BicomplexBase 
{    
public:
    BicomplexJ_IIM() { stepFn = (stepPtrFn) &BicomplexJ_IIM::Step; }

protected:
    void Step(vec3 &v, vec3 &vp) { preStep(v); radiciBicomplex(vec4(v, last4D), vp); }
};

/////////////////////////////////////////////////
class BicomplexJMod0_IIM : public BicomplexBase
{    
public:
    BicomplexJMod0_IIM() { stepFn = (stepPtrFn) &BicomplexJMod0_IIM::Step; }

protected:
    void Step(vec3 &v, vec3 &vp) { preStep(v); radiciBicomplex(vec4(v, dim4D), vp); }
};

/////////////////////////////////////////////////
class BicomplexJMod1_IIM : public BicomplexBase
{    
public:
    BicomplexJMod1_IIM() { stepFn = (stepPtrFn) &BicomplexJMod1_IIM::Step; }

protected:
    void Step(vec3 &v, vec3 &vp) { preStep(v); radiciBicomplex(vec4(v.x, v.y, vVal[0].z, last4D), vp); }
};
/////////////////////////////////////////////////
class BicomplexJMod2_IIM : public BicomplexBase
{    
public:
    BicomplexJMod2_IIM() { stepFn = (stepPtrFn) &BicomplexJMod2_IIM::Step; }

protected:
    //void Step(vec3 &v, vec3 &vp) { preStep(v,vp); radiciBicomplex(vec4(last4D, v.x, v.z, v.x), vp); }
    //void Step(vec3 &v, vec3 &vp) { preStep(v,vp); radiciBicomplex(vec4(v.y, v.z, last4D, v.x), vp); }
    void Step(vec3 &v, vec3 &vp) { preStep(v); radiciBicomplex(vec4(v.x, v.y, v.z, v.y), vp); }
    //void Step(vec3 &v, vec3 &vp) { preStep(v,vp); radiciBicomplex(vec4(v.x, v.y, v.y, v.y), vp); }
};

/////////////////////////////////////////////////
class BicomplexJMod3_IIM : public BicomplexBase
{    
public:
    BicomplexJMod3_IIM() { stepFn = (stepPtrFn) &BicomplexJMod3_IIM::Step; }

protected:
    void Step(vec3 &v, vec3 &vp) { preStep(v); radiciBicomplex(vec4(vVal[0].x, v.y, v.z, last4D), vp); }
};
/////////////////////////////////////////////////
class BicomplexJMod4_IIM : public BicomplexBase
{    
public:
    BicomplexJMod4_IIM() { stepFn = (stepPtrFn) &BicomplexJMod4_IIM::Step; }

protected:
    void Step(vec3 &v, vec3 &vp) { preStep(v); radiciBicomplex(vec4(v.x, v.y, v.x, last4D), vp); }
};
/////////////////////////////////////////////////
class BicomplexJMod5_IIM : public BicomplexBase
{    
public:
    BicomplexJMod5_IIM() { stepFn = (stepPtrFn) &BicomplexJMod5_IIM::Step; }

protected:
    void Step(vec3 &v, vec3 &vp) { preStep(v); radiciBicomplex(vec4( v.y, v.x, last4D, v.z), vp); }
};
/////////////////////////////////////////////////
class BicomplexJMod6_IIM : public BicomplexBase
{    
public:
    BicomplexJMod6_IIM() { stepFn = (stepPtrFn) &BicomplexJMod6_IIM::Step; }

protected:
    void Step(vec3 &v, vec3 &vp) { preStep(v); radiciBicomplex(vec4(vVal[0].x, vVal[0].y, v.z, last4D), vp); }
};
/////////////////////////////////////////////////
class BicomplexJMod7_IIM : public BicomplexBase
{    
public:
    BicomplexJMod7_IIM() { stepFn = (stepPtrFn) &BicomplexJMod7_IIM::Step; }

protected:
    //void Step(vec3 &v, vec3 &vp) { radiciBicomplex(vec4( v.x, v.x, v.z, v.x), vp); }
    void Step(vec3 &v, vec3 &vp) { preStep(v); radiciBicomplex(vec4( v.x, v.x, v.z, dim4D), vp); }
    //void Step(vec3 &v, vec3 &vp) { radiciBicomplex(vec4( v.x, v.x, v.z, last4D), vp); }
};

/////////////////////////////////////////////////
class BicomplexJExplorer : public BicomplexBase
{    
public:
    BicomplexJExplorer() { 
        stepFn = (stepPtrFn) &BicomplexJExplorer::Step; 
    }

    void additionalDataCtrls();
    virtual void initStep() {
        fractalIIMBase::initStep();
        a1[0] = a2[0] = a3[0] = a4[0] = &vVal[0].x;
        a1[1] = a2[1] = a3[1] = a4[1] = &vVal[0].y;
        a1[2] = a2[2] = a3[2] = a4[2] = &vVal[0].z;
        a1[3] = a2[3] = a3[3] = a4[3] = &dim4D;
        a1[4] = a2[4] = a3[4] = a4[4] = &vt.x;
        a1[5] = a2[5] = a3[5] = a4[5] = &vt.y;
        a1[6] = a2[6] = a3[6] = a4[6] = &vt.z;
        a1[7] = a2[7] = a3[7] = a4[7] = &last4D;        
    }

protected:
    void Step(vec3 &v, vec3 &vp) { 
        preStep(v); 
        vt = v; 
        radiciBicomplex(vec4( *a1[idx0], *a2[idx1], *a3[idx2], *a4[idx3]), vp); 
    }
private:
    float *a1[8], *a2[8], *a3[8], *a4[8];
    int idx0 = 4, idx1 = 5, idx2 = 6, idx3 = 7 ;
    const char str[8][4] { "s.X", "s.Y", "s.Z", "s.W", "i.X", "i.Y", "i.Z", "i.W" }; 
    vec3 vt;

};

/////////////////////////////////////////////////
class quatJulia_IIM : public fractalIIM_4D
{    
public:
    quatJulia_IIM() { stepFn = (stepPtrFn) &quatJulia_IIM::Step; }

protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};


class glynnJB_IIM : public fractalIIMBase
{    
public:
    glynnJB_IIM() { stepFn = (stepPtrFn) &glynnJB_IIM::Step;  }

protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};


//  Hopalong base class
////////////////////////////////////////////////////////////////////////////
class Hopalong : public attractorScalarK
{
public:

    Hopalong()  {
        stepFn = (stepPtrFn) &Hopalong::Step;    

        kMin = -10; kMax = 10; vMin = 0; vMax = 0;

        Init();        

    }

    void Init()
    {

        step = 1.05;

        _zy = _x = _y = oldZ = 0.f;
        _r=.01f;

        m_POV = vec3(0.f, .0, 7.f);

    }

    virtual void resetQueue() {
        AttractorBase::resetQueue();
        step = 1.05;
        _zy = _x = _y = oldZ = 0.f;
        _r=.01f;
    }

    //void Step();
    //void Step(float *ptr, int numElements);
    //inline void Step(float *&ptr, vec3 &v, vec3 &vp);
    void Step(vec3 &v, vec3 &vp);

    void startData();


    void newRandomValues() {
        resetQueue();

        float temp = kVal[6];
        for (int i = 0; i<getKSize(); i++) kVal[i] = RANDOM(kMin,kMax);
        
        kVal[0] = (float)rand() / (float)RAND_MAX;

        kVal[6] = temp;

    }

private:
    float step;
    float _zy, _x, _y, oldZ, _r;

    float getRnadomK() { return 10. * (float)rand() / (float)RAND_MAX; }

};

//--------------------------------------------------------------------------
//  Vector K Coeff Attractors
//--------------------------------------------------------------------------

//  Polinomial base class
////////////////////////////////////////////////////////////////////////////
class PowerN3D : public attractorVectorK
{
public:

    PowerN3D() {
        
        stepFn = (stepPtrFn) &PowerN3D::Step;    

        tmpOrder = order = 2;
        vMin = 0.0; vMax = 0.0; kMin = -1.25; kMax = 1.25;
        m_POV = vec3( 0.f, 0, 7.f);
        
        resetData();
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();

    void searchAttractor()  { searchLyapunov(); }

    void initStep() {
        resetQueue();
        resetData();

        if(nCoeff!=kVal.size()) assert("mismatch loaded size!!");

        Insert(vVal[0]);

    }

    //  Personal vals
    ///////////////////////////////////////
    void saveAdditionalData(Config &cfg);
    void loadAdditionalData(Config &cfg);

    void additionalDataCtrls();

    

    //  SetOrder
    ///////////////////////////////////////
    void setOrder(const int n)
    { 
        tmpOrder = order = n;
        resetData();

        kVal.resize(nCoeff);

        newRandomValues();

        resetQueue();
        Insert(vVal[0]);

        searchAttractor();
    }


    void newRandomValues()
    {
        for(int i=0; i<nCoeff; i++)
            kVal[i] = vec3(RANDOM(kMin,kMax),RANDOM(kMin,kMax),RANDOM(kMin,kMax));

        vVal[0] = vec3(RANDOM(vMin,vMax),RANDOM(vMin,vMax),RANDOM(vMin,vMax));
    }

private:

    int getNumCoeff(int o) { return (o+1) * (o+2) * (o+3) / 6; }
    int getNumCoeff() { return getNumCoeff(order); }

    void resetData() {
        nCoeff = getNumCoeff();

        //elv.clear();
        elv.resize(order+1);

        //cf.clear();
        cf.resize(nCoeff);

    }

    vector<vec3> elv;
    vector<float> cf;
    int nCoeff;
    int order, tmpOrder;
};

//  Polinomial base class
////////////////////////////////////////////////////////////////////////////
class PolynomialBase : public attractorVectorK
{
public:
    PolynomialBase() {
        vMin = 0.0; vMax = 0.0; kMin = -1.0; kMax = 1.0;
        m_POV = vec3( 0.f, 0, 7.f);
    }
protected:
    virtual void searchAttractor()  { searchLyapunov(); }
};

/////////////////////////////////////////////////
class PolynomialA : public PolynomialBase
{
public:
    PolynomialA() { stepFn = (stepPtrFn) &PolynomialA::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
/*
    void newRandomValues ()
    {
            float val=.23+(float)rand()/(float)RAND_MAX*.1f;
            //float val=1.55f+(float)rand()/(float)RAND_MAX*.2f;

            int j=rand()%3;

            kVal[j]=val;
            kVal[(j+1)%3]=(float)rand()/(float)RAND_MAX*2.f;
            kVal[(j+2)%3]=3.f-kVal[j]-kVal[(j+1)%3];
    }*/
};

/////////////////////////////////////////////////
class PolynomialB : public PolynomialBase
{
public:
    PolynomialB() { stepFn = (stepPtrFn) &PolynomialB::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};

/////////////////////////////////////////////////
class PolynomialC : public PolynomialBase
{
public:
    PolynomialC() { stepFn = (stepPtrFn) &PolynomialC::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};

/////////////////////////////////////////////////
class PolynomialABS : public PolynomialBase
{
public:
    PolynomialABS() { stepFn = (stepPtrFn) &PolynomialABS::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};

/////////////////////////////////////////////////
class PolynomialPow : public PolynomialBase
{
public:
    PolynomialPow() { stepFn = (stepPtrFn) &PolynomialPow::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};

/////////////////////////////////////////////////
class PolynomialSin : public PolynomialBase
{
public:
    PolynomialSin() { stepFn = (stepPtrFn) &PolynomialSin::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  Rampe base class
////////////////////////////////////////////////////////////////////////////
class RampeBase : public attractorVectorK
{
public:
    RampeBase() {
        vMin = 0.0; vMax = 0.0; kMin = -1.0; kMax = 1.0;
        m_POV = vec3( 0.f, 0, 10.f);
    }
protected:
    void searchAttractor()  { searchLyapunov(); }
};
/////////////////////////////////////////////////
class Rampe01 : public RampeBase
{
public:
    Rampe01() { stepFn = (stepPtrFn) &Rampe01::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};
/////////////////////////////////////////////////
class Rampe02 : public RampeBase
{
public:
    Rampe02() { stepFn = (stepPtrFn) &Rampe02::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};
/////////////////////////////////////////////////
class Rampe03 : public RampeBase
{
public:
    Rampe03() { stepFn = (stepPtrFn) &Rampe03::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};
/////////////////////////////////////////////////
class Rampe03A : public RampeBase
{
public:
    Rampe03A() { stepFn = (stepPtrFn) &Rampe03A::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};
/////////////////////////////////////////////////
class Rampe04 : public RampeBase
{
public:
    Rampe04() { stepFn = (stepPtrFn) &Rampe04::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};
/////////////////////////////////////////////////
class Rampe05 : public RampeBase
{
public:
    Rampe05() { stepFn = (stepPtrFn) &Rampe05::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};
/////////////////////////////////////////////////
class Rampe06 : public RampeBase
{
public:
    Rampe06() { stepFn = (stepPtrFn) &Rampe06::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};
/////////////////////////////////////////////////
class Rampe07 : public RampeBase
{
public:
    Rampe07() { stepFn = (stepPtrFn) &Rampe07::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};
/////////////////////////////////////////////////
class Rampe08 : public RampeBase
{
public:
    Rampe08() { stepFn = (stepPtrFn) &Rampe08::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};
/////////////////////////////////////////////////
class Rampe09 : public RampeBase
{
public:
    Rampe09() { stepFn = (stepPtrFn) &Rampe09::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};
/////////////////////////////////////////////////
class Rampe10 : public RampeBase
{
public:
    Rampe10() { stepFn = (stepPtrFn) &Rampe10::Step; }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//--------------------------------------------------------------------------
//  Scalar K Coeff Attractors
//--------------------------------------------------------------------------

//  KingsDream base class
////////////////////////////////////////////////////////////////////////////
class KingsDream : public attractorScalarK
{
public:
    KingsDream() { 
        stepFn = (stepPtrFn) &KingsDream::Step; 
        vMin = -0.5; vMax = 0.5; kMin = -2.0; kMax = 2.0;
        m_POV = vec3( 0.f, 0, 10.f);
    }
protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
    void searchAttractor()  { searchLyapunov(); }
};

//  Pickover base class
////////////////////////////////////////////////////////////////////////////
class Pickover : public attractorScalarK
{
public:

    Pickover() {
        stepFn = (stepPtrFn) &Pickover::Step;    

        vMin = -1.0; vMax = 1.0; kMin = -3.0; kMax = 3.0;

        m_POV = vec3( 0.f, 0, 7.f);
    }

protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
    void searchAttractor()  { searchLyapunov(); }
};

//  SinCos base class
////////////////////////////////////////////////////////////////////////////
class SinCos : public attractorScalarK
{
public:

    SinCos() {
        stepFn = (stepPtrFn) &SinCos::Step;    

        kMin = -glm::pi<float>();
        kMax =  glm::pi<float>();
        vMin = -1.0; vMax = 1.0;

        m_POV = vec3( 0.f, 0, 12.f);
    }

protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
    void searchAttractor()  { searchLyapunov(); }
};

//  SinCos base class
////////////////////////////////////////////////////////////////////////////
class Mira3D : public attractorScalarK
{
public:

    Mira3D() {
        stepFn = (stepPtrFn) &Mira3D::Step;    

        vMin = -10.0; vMax = 10.0; kMin = -1.0; kMax = 1.0;

        m_POV = vec3( 0.f, 0, 12.f);
    }

protected:
    void Step(vec3 &v, vec3 &vp);
    void startData();
    void searchAttractor()  { searchLyapunov(); }
};


//--------------------------------------------------------------------------
//  d(x,y,z)/dt Attractors
//--------------------------------------------------------------------------

//  Lorenz base class
////////////////////////////////////////////////////////////////////////////
class Lorenz : public attractorDtType
{
public:

    Lorenz() {
        stepFn = (stepPtrFn) &Lorenz::Step;    

        kMin = -10; kMax = 10; vMin = 0; vMax = 0;
        m_POV = vec3(0.f, .0, 40.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
/*
    void Step()
    {
    //static GLfloat h=0.135f;
    vec3 vect=getCurrent();

//Lorenz 84
//    Insert(vec3(vect.x+dtStepInc*(-kVal[0]*vect.x-vect.y*vect.y-vect.x*vect.x+kVal[0]*kVal[2]),
//                 vect.y+dtStepInc*(-vect.y+vect.x*vect.y-kVal[1]*vect.x*vect.z+kVal[3]),
//                 vect.z+dtStepInc*(-vect.z+kVal[1]*vect.x*vect.y+vect.x*vect.z)));
    }
*/
    void newRandomValues()
    {
        kVal[0] = (float)rand()/(float)RAND_MAX * 20.f;
        kVal[1] = (float)rand()/(float)RAND_MAX * 56.f;
        kVal[2] = (float)rand()/(float)RAND_MAX * (16/3.f);

    }

    float getDistance(int i=0, int j=1) { return  glm::distance(stepQueue[i], stepQueue[j]); }

};

//  ChenLee base class
////////////////////////////////////////////////////////////////////////////
class ChenLee : public attractorDtType
{
public:

    ChenLee() {
        stepFn = (stepPtrFn) &ChenLee::Step;    

        m_POV = vec3( 0.f, 0, 50.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  TSUCS1 base class
////////////////////////////////////////////////////////////////////////////
class TSUCS : public attractorDtType
{
public:

    TSUCS() {
        stepFn = (stepPtrFn) &TSUCS::Step;    

        m_POV = vec3( 0.f, 0, 240.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  Aizawa
////////////////////////////////////////////////////////////////////////////
class Aizawa : public attractorDtType
{
public:

    Aizawa() {
        stepFn = (stepPtrFn) &Aizawa::Step;    

        m_POV = vec3( 0.f, 0, 10.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  YuWang
////////////////////////////////////////////////////////////////////////////
class YuWang : public attractorDtType
{
public:

    YuWang() {
        stepFn = (stepPtrFn) &YuWang::Step;    

        m_POV = vec3( 0.f, 0, 10.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  FourWing
////////////////////////////////////////////////////////////////////////////
class FourWing : public attractorDtType
{
public:

    FourWing() {
        stepFn = (stepPtrFn) &FourWing::Step;

        m_POV = vec3( 0.f, 0, 30.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  FourWing2
////////////////////////////////////////////////////////////////////////////
class FourWing2 : public attractorDtType
{
public:

    FourWing2() {
        stepFn = (stepPtrFn) &FourWing2::Step;

        m_POV = vec3( 0.f, 0, 100.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  FourWing3
////////////////////////////////////////////////////////////////////////////
class FourWing3 : public attractorDtType
{
public:

    FourWing3() {
        stepFn = (stepPtrFn) &FourWing3::Step;

        m_POV = vec3( 0.f, 0, 70.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  Thomas
////////////////////////////////////////////////////////////////////////////
class Thomas : public attractorDtType
{
public:

    Thomas() {
        stepFn = (stepPtrFn) &Thomas::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  Halvorsen
////////////////////////////////////////////////////////////////////////////
class Halvorsen : public attractorDtType
{
public:

    Halvorsen() {
        stepFn = (stepPtrFn) &Halvorsen::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  Arneodo 
////////////////////////////////////////////////////////////////////////////
class Arneodo : public attractorDtType
{
public:

    Arneodo() {
        stepFn = (stepPtrFn) &Arneodo::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  Bouali 
////////////////////////////////////////////////////////////////////////////
class Bouali : public attractorDtType
{
public:

    Bouali() {
        stepFn = (stepPtrFn) &Bouali::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  Hadley
////////////////////////////////////////////////////////////////////////////
class Hadley : public attractorDtType
{
public:

    Hadley() {
        stepFn = (stepPtrFn) &Hadley::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  LiuChen
////////////////////////////////////////////////////////////////////////////
class LiuChen : public attractorDtType
{
public:

    LiuChen() {
        stepFn = (stepPtrFn) &LiuChen::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  GenesioTesi
////////////////////////////////////////////////////////////////////////////
class GenesioTesi : public attractorDtType
{
public:

    GenesioTesi() {
        stepFn = (stepPtrFn) &GenesioTesi::Step;    

        m_POV = vec3( 0.f, 0, 10.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  NewtonLeipnik
////////////////////////////////////////////////////////////////////////////
class NewtonLeipnik : public attractorDtType
{
public:

    NewtonLeipnik() {
        stepFn = (stepPtrFn) &NewtonLeipnik::Step;    

        m_POV = vec3( 0.f, 0, 10.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  NoseHoover
////////////////////////////////////////////////////////////////////////////
class NoseHoover : public attractorDtType
{
public:

    NoseHoover() {
        stepFn = (stepPtrFn) &NoseHoover::Step;    

        m_POV = vec3( 0.f, 0, 10.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  RayleighBenard
////////////////////////////////////////////////////////////////////////////
class RayleighBenard : public attractorDtType
{
public:

    RayleighBenard() {
        stepFn = (stepPtrFn) &RayleighBenard::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  Sakarya  
////////////////////////////////////////////////////////////////////////////
class Sakarya : public attractorDtType
{
public:

    Sakarya() {
        stepFn = (stepPtrFn) &Sakarya::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  Robinson
////////////////////////////////////////////////////////////////////////////
class Robinson : public attractorDtType
{
public:

    Robinson() {
        stepFn = (stepPtrFn) &Robinson::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  Rossler
////////////////////////////////////////////////////////////////////////////
class Rossler : public attractorDtType
{
public:

    Rossler() {
        stepFn = (stepPtrFn) &Rossler::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  Rucklidge
////////////////////////////////////////////////////////////////////////////
class Rucklidge : public attractorDtType
{
public:

    Rucklidge() {
        stepFn = (stepPtrFn) &Rucklidge::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};

//  Dequan Li 
////////////////////////////////////////////////////////////////////////////
class DequanLi : public attractorDtType
{
public:

    DequanLi() {
        stepFn = (stepPtrFn) &DequanLi::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};
//  Multi Chua II
////////////////////////////////////////////////////////////////////////////
class MultiChuaII : public attractorDtType
{
public:

    MultiChuaII() {
        stepFn = (stepPtrFn) &MultiChuaII::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};
//  Zhou-Chen
////////////////////////////////////////////////////////////////////////////
class ZhouChen : public attractorDtType
{
public:

    ZhouChen() {
        stepFn = (stepPtrFn) &ZhouChen::Step;    

        m_POV = vec3( 0.f, 0, 20.f);
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();
};


//  Magnetic base class
////////////////////////////////////////////////////////////////////////////
class Magnetic : public attractorVectorK
{
public:

    typedef  const vec3 (Magnetic::*magneticPtrFn)(const vec3 &, int);
    Magnetic() 
    {
        stepFn = (stepPtrFn) &ChenLee::Step;    
        kMax = 5.0; kMin = -5.0; vMax = 1.0; vMin = -1.0;

        m_POV = vec3( 0.f, 0.f, 3.f);
    }

    void initStep() {
        resetQueue();
        Insert(vec3(0.f));
        tmpElements = vVal.size();
    }

    void Step(vec3 &v, vec3 &vp);
    void startData();

    //  innerSteps functions
    ///////////////////////////////////////
    const vec3 straight(const vec3 &vx, int i);
    const vec3 rightShift(const vec3 &vx, int i);
    const vec3 leftShift(const vec3 &vx, int i);
    const vec3 fullPermutated(const vec3 &vx, int i);
    const vec3 tryed(const vec3 &vx, int i);

    
    //  Additional save vals
    ///////////////////////////////////////
    void saveAdditionalData(Config &cfg);
    void loadAdditionalData(Config &cfg);

    //  Additional Controls
    ///////////////////////////////////////
    void additionalDataCtrls();
    int additionalDataDlg();

    void setElements(const int n)
    { 
        newItemsEnd = true;
        //clear();
        vVal.resize(n); kVal.resize(n); 

        newRandomValues();

        newItemsEnd = false;
    }

    void setElementsA(const int n)
    { 
        newItemsEnd = true;
        const int nMagnets = vVal.size();

        const float _kUP =  kMax/nMagnets;
        const float _kDW =  kMin/nMagnets;

        const float _dUP =  vMax;
        const float _dDW =  vMin;

        int elements=nMagnets;
        while(n > elements) {
            vVal.push_back(vec3(RANDOM(_dDW,_dUP), RANDOM(_dDW,_dUP), RANDOM(_dDW,_dUP)));
            kVal.push_back(vec3(RANDOM(_kDW,_kUP), RANDOM(_kDW,_kUP), RANDOM(_kDW,_kUP))); 
            elements++;
        }
        while(n < elements) {
            vVal.pop_back();
            kVal.pop_back(); 
            elements--;
        }
        //ResizeVectors(); 
        resetQueue(); 
        newItemsEnd = false;
    }

    int getElements() { return vVal.size(); }


    void newRandomValues() 
    {
        for(int j =0; j<100; j++) {
            initParams();

            for(int i=0; i<100; i++) AttractorBase::Step();

            vec3 v0 = getCurrent();
            vec3 v1 = getPrevious();

            //if(fabs(v0.x-v1.x)<.01 && fabs(v0.y-v1.y)<.01 && fabs(v0.z-v1.z)<.01) 
            if(fabs(v0.x-v1.x)>.01 || fabs(v0.y-v1.y)>.01 || fabs(v0.z-v1.z)>.01) break;
        }
    }
    
    void searchAttractor()  { searchLyapunov(); }



protected:
    thread *th[4];
    vec3 vth[4],vcurr;

    magneticPtrFn increment;
    bool newItemsEnd = false;

    int tmpElements, nElements;


    friend void fillMagneticData();
/*
    void ResizeVectors() {
        kVal.resize(m_nMagnets);
        vVal.resize(m_nMagnets);       
    }

*/
    void initParams() {

        const int nMagnets = vVal.size();

        const float _kUP =  kMax/nMagnets;
        const float _kDW =  kMin/nMagnets;

        const float _dUP =  vMax;
        const float _dDW =  vMin;

        for(auto &i : vVal) i = vec3(RANDOM(_dDW,_dUP), RANDOM(_dDW,_dUP), RANDOM(_dDW,_dUP));
        for(auto &i : kVal) i = vec3(RANDOM(_kDW,_kUP), RANDOM(_kDW,_kUP), RANDOM(_kDW,_kUP));

        initStep();
    }
};
//  Magnetic LeftShift
///////////////////////////////////////
class MagneticLeft : public Magnetic {
public:
    MagneticLeft() { increment = &Magnetic::leftShift; }
};
//  Magnetic RightShift
///////////////////////////////////////
class MagneticRight : public Magnetic {
public:
    MagneticRight() { increment = &Magnetic::rightShift; }
};
//  Magnetic Full permutated
///////////////////////////////////////
class MagneticFull : public Magnetic {
public:
    MagneticFull() { increment = &Magnetic::fullPermutated; }
};
//  Magnetic Full permutated
///////////////////////////////////////
class MagneticStraight : public Magnetic {
public:
    MagneticStraight() { increment = &Magnetic::straight; }
};

//  Attractors Thread helper class
////////////////////////////////////////////////////////////////////////////
class threadStepClass
{
public:
    threadStepClass(emitterBaseClass *e) : emitter(e) { newThread(); }
    ~threadStepClass() { deleteThread(); }

    void newThread();
    void deleteThread();
    void startThread(bool startOn=true);

    thread *getThread() { return attractorLoop; }
    emitterBaseClass *getEmitter() { return emitter; }

    void stopThread();
    void restartEmitter();

    void notify();

    bool canStart();

private:
    emitterBaseClass *emitter = nullptr;
    thread *attractorLoop = nullptr;

};

//  Attractor Class container
////////////////////////////////////////////////////////////////////////////
#if !defined(GLCHAOSP_LIGHTVER)
#define ATT_PATH "startData/"
#else
#define ATT_PATH "startData/"
#endif
#define ATT_EXT ".sca"

#define PB(ATT,DISPLAY_NAME) ptr.push_back(new ATT());\
                             ptr.back()->fileName = (ATT_PATH #ATT ATT_EXT);\
                             ptr.back()->nameID = (#ATT);\
                             ptr.back()->displayName = (DISPLAY_NAME);



class AttractorsClass 
{
public:
    AttractorsClass() {
        PB(MagneticRight      , u8"\uf006" " MagneticRight"      )
        PB(MagneticLeft       , u8"\uf006" " MagneticLeft"       )
        PB(MagneticFull       , u8"\uf006" " MagneticFull"       )
        PB(MagneticStraight   , u8"\uf006" " MagneticStraight"   )
        PB(PolynomialA        , u8"\uf006" " Polynom A"          )
        PB(PolynomialB        , u8"\uf006" " Polynom B"          )
        PB(PolynomialC        , u8"\uf006" " Polynom C"          )
        PB(PolynomialABS      , u8"\uf006" " Polynom Abs"        )
        PB(PolynomialPow      , u8"\uf006" " Polynom Pow"        )
        PB(PolynomialSin      , u8"\uf006" " Polynom Sin"        )
        PB(PowerN3D           , u8"\uf006" " Polynom N-order"    )
        PB(Rampe01            , u8"\uf006" " Rampe01"            )
        PB(Rampe02            , u8"\uf006" " Rampe02"            )
        PB(Rampe03            , u8"\uf006" " Rampe03"            )
        PB(Rampe03A           , u8"\uf006" " Rampe03 mod."       )
        PB(Rampe04            , u8"\uf006" " Rampe04"            )
        PB(Rampe05            , u8"\uf006" " Rampe05"            )
        PB(Rampe06            , u8"\uf006" " Rampe06"            )
        PB(Rampe07            , u8"\uf006" " Rampe07"            )
        PB(Rampe08            , u8"\uf006" " Rampe08"            )
        PB(Rampe09            , u8"\uf006" " Rampe09"            )
        PB(Rampe10            , u8"\uf006" " Rampe10"            )
        PB(KingsDream         , u8"\uf006" " King's Dream"       )
        PB(Pickover           , u8"\uf006" " Pickover"           )
        PB(SinCos             , u8"\uf006" " Sin Cos"            )
//        PB(Mira3D             , u8"\uf006" " Mira3D"             )
        PB(Lorenz             , u8"\uf192" " Lorenz"             )
        PB(ChenLee            , u8"\uf192" " Chen-Lee"           )
        PB(TSUCS              , u8"\uf192" " TSUCS 1&2"          )
        PB(Aizawa             , u8"\uf192" " Aizawa"             )
        PB(YuWang             , u8"\uf192" " Yu-Wang"            )
        PB(FourWing           , u8"\uf192" " FourWing"           )
        PB(FourWing2          , u8"\uf192" " FourWing 2"         )
        PB(FourWing3          , u8"\uf192" " FourWing 3"         )
        PB(Thomas             , u8"\uf192" " Thomas"             )
        PB(Halvorsen          , u8"\uf192" " Halvorsen"          )
        PB(Arneodo            , u8"\uf192" " Arneodo"            )
        PB(Bouali             , u8"\uf192" " Bouali"             )
        PB(Hadley             , u8"\uf192" " Hadley"             )
        PB(LiuChen            , u8"\uf192" " Liu-Chen"           )
        PB(GenesioTesi        , u8"\uf192" " Genesio-Tesi"       )
        PB(NewtonLeipnik      , u8"\uf192" " Newton-Leipnik"     )
        PB(NoseHoover         , u8"\uf192" " Nose-Hoover"        )
        PB(RayleighBenard     , u8"\uf192" " Rayleigh-Benard"    )
        PB(Sakarya            , u8"\uf192" " Sakarya"            )
        PB(Rossler            , u8"\uf192" " Rossler"            )
        PB(Rucklidge          , u8"\uf192" " Rucklidge"          )
        PB(DequanLi           , u8"\uf192" " Dequan-Li"          )
        PB(MultiChuaII        , u8"\uf192" " Multi-Chua II"      )
        PB(ZhouChen           , u8"\uf192" " Zhou-Chen"          )
//        PB(Robinson           , u8"\uf192" " Robinson"           )
        PB(juliaBulb_IIM      , u8"\uf2dc" " JuliaBulb"          )
        PB(juliaBulb4th_IIM   , u8"\uf2dc" " JuliaBulb Nth"      )
        PB(BicomplexJ_IIM     , u8"\uf2dc" " biComplex Julia"    )
        PB(BicomplexJMod0_IIM , u8"\uf2dc" " biCplxJ m.0"        )
        PB(BicomplexJMod1_IIM , u8"\uf2dc" " biCplxJ m.1"        )
        PB(BicomplexJMod2_IIM , u8"\uf2dc" " biCplxJ m.2"        )
        PB(BicomplexJMod3_IIM , u8"\uf2dc" " biCplxJ m.3"        )
        PB(BicomplexJMod4_IIM , u8"\uf2dc" " biCplxJ m.4"        )
        PB(BicomplexJMod5_IIM , u8"\uf2dc" " biCplxJ m.5"        )
        PB(BicomplexJMod6_IIM , u8"\uf2dc" " biCplxJ m.6"        )
        PB(BicomplexJMod7_IIM , u8"\uf2dc" " biCplxJ m.7"        )
        PB(quatJulia_IIM      , u8"\uf2dc" " quatJulia"          )
//        PB(BicomplexJExplorer , u8"\uf2dc" " biComplexJExplorer" )
            
//        PB(glynnJB_IIM        , u8"\uf2dc" " Glynn JuliaBulb"    )
//        PB(Hopalong        , "Hopalong"         )

        selected = -1;

        //std::sort(ptr.begin(), ptr.end());


        loadStartData();
    }
#undef PB
#undef ATT_PATH
#undef ATT_EXT 

    void loadStartData() {
        for(auto i : ptr) i->startData();
    }

    AttractorBase *get()      { return ptr.at(selected); }
    AttractorBase *get(int i) { return ptr.at(i); }
    vector<AttractorBase *>& getList()  { return ptr; }

    string& getDisplayName(int i)    { return ptr.at(i)->getDisplayName(); }
    string& getDisplayName() { return ptr.at(selected)->getDisplayName(); }

    string& getNameID(int i)    { return ptr.at(i)->getNameID(); }
    string& getNameID() { return ptr.at(selected)->getNameID(); }

    string& getFileName(int i)    { return ptr.at(i)->fileName; }
    string& getFileName() { return ptr.at(selected)->fileName; }
    void setFileName(const std::string &s) { ptr.at(selected)->fileName = s; }
    //

    void saveFile(const char *name);
    bool loadFile(const char *name);

    void saveVals(Config &cfg);
    bool loadVals(Config &cfg);

    int  getSelection()      { return selected; }
    void setSelection(int i) { newSelection(i);  }

    void setSelectionByName(const string &s) {        
        for (int i = 0; i < ptr.size(); i++) 
            if(getNameID(i) == s) {
                newSelection(i);
                break;
            }
    }

    int getSelectionByName(const string &s) {        
        for (int i = 0; i < ptr.size(); i++) 
            if(getNameID(i) == s) return i;

        return -1;
    }

    void restart();

    ~AttractorsClass() {        

        while(!ptr.empty()) {
            delete ptr.back();
            ptr.pop_back();
        }

    }

    threadStepClass *getThreadStep() { return threadStep; }
    thread *getThread() { return threadStep->getThread(); }

    //thread func
    void endlessStep(emitterBaseClass *emitter);

    void queryStopThread() { endlessLoop=false; }
    void queryStartThread() { endlessLoop=true; }
    bool isStopThread() { return endlessLoop; }

    void newStepThread(emitterBaseClass *e);
    void deleteStepThread();
    void generateNewRandom();

    std::mutex &getStepMutex() { return stepMutex; }

    bool getEndlessLoop() { return endlessLoop; }

    //void lockStep() { mtxStep.lock(); }
    //void unlockStep() { mtxStep.unlock(); }
private:

    void newSelection(int i);
    void selection(int i);

    //thread helper
    //bool breakLoop = false;

    bool endlessLoop = true;

    std::mutex stepMutex;
    std::condition_variable stepCondVar;

    std::mutex mtxStep;

//vector<string> name;

    threadStepClass *threadStep = nullptr;

    vector<AttractorBase *> ptr;
    int selected;

    friend AttractorBase;
    friend threadStepClass;

};

extern AttractorsClass attractorsList;

