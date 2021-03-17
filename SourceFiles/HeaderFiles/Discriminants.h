/*! 
 *  \file Discriminants.h 
 *  \brief Header file for various discriminants return function
 */

#ifndef DISCRIMINANTS_H
#define DISCRIMINANTS_H

#include <iostream>
#include <vector>
#include <TROOT.h>
#include <TLorentzVector.h>

/*These namespaces can be useful*/
using namespace std;

auto squaredtransferredmomentum = [](Float_t Bpt, Float_t Beta, Float_t Bphi, Float_t Bmass,
                                     Float_t mu1pt, Float_t mu1eta, Float_t mu1phi, Float_t mu1mass,
                                     Float_t mu2pt, Float_t mu2eta, Float_t mu2phi, Float_t mu2mass) 
{
    Float_t  q2e = Bpt*cosh(Beta)-mu1pt*cosh(mu1eta)-mu2pt*cosh(mu2eta);
    Float_t q2px = Bpt*cos(Bphi)-mu1pt*cos(mu1phi)-mu2pt*cos(mu2phi);
    Float_t q2py = Bpt*sin(Bphi)-mu1pt*sin(mu1phi)-mu2pt*sin(mu2phi);
    Float_t q2pz = Bpt*sinh(Beta)-mu1pt*sinh(mu1eta)-mu2pt*sinh(mu2eta);

    return (Float_t)(q2e*q2e-q2px*q2px-q2py*q2py-q2pz*q2pz);
};

auto missingmasssquared = [](Float_t Bpt, Float_t Beta, Float_t Bphi, Float_t Bmass,
                             Float_t mu1pt, Float_t mu1eta, Float_t mu1phi, Float_t mu1mass,
                             Float_t mu2pt, Float_t mu2eta, Float_t mu2phi, Float_t mu2mass,
                             Float_t kpt, Float_t keta, Float_t kphi, Float_t kmass) 
{
    Float_t  mm2e = Bpt*cosh(Beta)-mu1pt*cosh(mu1eta)-mu2pt*cosh(mu2eta)-kpt*cosh(keta);
    Float_t mm2px = Bpt*cos(Bphi)-mu1pt*cos(mu1phi)-mu2pt*cos(mu2phi)-kpt*cos(kphi);
    Float_t mm2py = Bpt*sin(Bphi)-mu1pt*sin(mu1phi)-mu2pt*sin(mu2phi)-kpt*sin(kphi);
    Float_t mm2pz = Bpt*sinh(Beta)-mu1pt*sinh(mu1eta)-mu2pt*sinh(mu2eta)-kpt*sinh(keta);

    return (Float_t)(mm2e*mm2e-mm2px*mm2px-mm2py*mm2py-mm2pz*mm2pz);
};

auto missingpt = [](Float_t Bpt, Float_t mu1pt, Float_t mu2pt, Float_t kpt)
{
    return (Float_t)(Bpt-mu1pt-mu2pt-kpt);
};

#endif