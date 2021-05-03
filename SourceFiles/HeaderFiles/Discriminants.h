/*! 
 *  \file Discriminants.h 
 *  \brief Header file for some variables calculated with lambda function
 */

#ifndef DISCRIMINANTS_H
#define DISCRIMINANTS_H

#include <iostream>
#include <vector>
#include <TROOT.h>
#include <TLorentzVector.h>

/*These namespaces can be useful*/
using namespace std;

auto eta = [](Float_t vertex1x, Float_t vertex1y, Float_t vertex1z,
              Float_t vertex2x, Float_t vertex2y, Float_t vertex2z) {
  TVector3 vector1 = TVector3(vertex1x, vertex1y, vertex1z);
  TVector3 vector2 = TVector3(vertex2x, vertex2y, vertex2z);
  Float_t etav = (vector2 - vector1).Eta();

  return etav;
};

auto eta_3mumomentum = [](Float_t mu1pt, Float_t mu1eta, Float_t mu1phi,
                          Float_t mu2pt, Float_t mu2eta, Float_t mu2phi,
                          Float_t kpt, Float_t keta, Float_t kphi) {
  TVector3 vector1, vector2, vector3;
  vector1.SetPtEtaPhi(mu1pt, mu1eta, mu1phi);
  vector2.SetPtEtaPhi(mu2pt, mu2eta, mu2phi);
  vector3.SetPtEtaPhi(kpt, keta, kphi);
  TVector3 vecadd = vector1 + vector2 + vector3;
  Float_t eta3mumom = vecadd.Eta();

  return eta3mumom;
};

auto deltaeta = [](Float_t vertexrec1x, Float_t vertexrec1y, Float_t vertexrec1z,
                   Float_t vertexrec2x, Float_t vertexrec2y, Float_t vertexrec2z,
                   Float_t vertexgen1x, Float_t vertexgen1y, Float_t vertexgen1z,
                   Float_t vertexgen2x, Float_t vertexgen2y, Float_t vertexgen2z) {
  TVector3 deltarec = TVector3(vertexrec1x, vertexrec1y, vertexrec1z) - TVector3(vertexrec2x, vertexrec2y, vertexrec2z);
  Float_t etarec = deltarec.Eta();
  TVector3 deltagen = TVector3(vertexgen1x, vertexgen1y, vertexgen1z) - TVector3(vertexgen2x, vertexgen2y, vertexgen2z);
  Float_t etagen = deltagen.Eta();
  Float_t deltaeta = etarec - etagen;

  return deltaeta;
};

auto phi = [](Float_t vertex1x, Float_t vertex1y, Float_t vertex1z,
              Float_t vertex2x, Float_t vertex2y, Float_t vertex2z) {
  TVector3 vector1 = TVector3(vertex1x, vertex1y, vertex1z);
  TVector3 vector2 = TVector3(vertex2x, vertex2y, vertex2z);
  Float_t phi = (vector2 - vector1).Phi();

  return phi;
};

auto phi_3mumomentum = [](Float_t mu1pt, Float_t mu1eta, Float_t mu1phi,
                          Float_t mu2pt, Float_t mu2eta, Float_t mu2phi,
                          Float_t kpt, Float_t keta, Float_t kphi) {
  TVector3 vector1, vector2, vector3;
  vector1.SetPtEtaPhi(mu1pt, mu1eta, mu1phi);
  vector2.SetPtEtaPhi(mu2pt, mu2eta, mu2phi);
  vector3.SetPtEtaPhi(kpt, keta, kphi);
  TVector3 vecadd = vector1 + vector2 + vector3;
  Float_t phi3mumom = vecadd.Phi();

  return phi3mumom;
};

auto deltaphi = [](Float_t vertexrec1x, Float_t vertexrec1y, Float_t vertexrec1z,
                   Float_t vertexrec2x, Float_t vertexrec2y, Float_t vertexrec2z,
                   Float_t vertexgen1x, Float_t vertexgen1y, Float_t vertexgen1z,
                   Float_t vertexgen2x, Float_t vertexgen2y, Float_t vertexgen2z) {
  TVector3 deltarec = TVector3(vertexrec2x, vertexrec2y, vertexrec2z) - TVector3(vertexrec1x, vertexrec1y, vertexrec1z);
  TVector3 deltagen = TVector3(vertexgen2x, vertexgen2y, vertexgen2z) - TVector3(vertexgen1x, vertexgen1y, vertexgen1z);
  Float_t deltaphi = deltarec.DeltaPhi(deltagen);

  return deltaphi;
};

auto scalar = [](Float_t vertex1x, Float_t vertex1y, Float_t vertex1z,
                 Float_t vertex2x, Float_t vertex2y, Float_t vertex2z,
                 Float_t Bpt, Float_t Beta, Float_t Bphi) {
  TVector3 vector1 = TVector3(vertex1x, vertex1y, vertex1z);
  TVector3 vector2 = TVector3(vertex2x, vertex2y, vertex2z);
  TVector3 vector_bc;
  vector_bc.SetPtEtaPhi(Bpt, Beta, Bphi);
  Float_t scalarproduct = (vector2 - vector1).Dot(vector_bc);

  return scalarproduct;
};

auto angle = [](Float_t vertex1x, Float_t vertex1y, Float_t vertex1z,
                Float_t vertex2x, Float_t vertex2y, Float_t vertex2z,
                Float_t Bpt, Float_t Beta, Float_t Bphi) {
  TVector3 vector1 = TVector3(vertex1x, vertex1y, vertex1z);
  TVector3 vector2 = TVector3(vertex2x, vertex2y, vertex2z);
  TVector3 vector_bc;
  vector_bc.SetPtEtaPhi(Bpt, Beta, Bphi);
  Float_t alfa = vector_bc.Angle(vector2 - vector1);

  return alfa;
};

auto cosangle = [](Float_t vertex1x, Float_t vertex1y, Float_t vertex1z,
                   Float_t vertex2x, Float_t vertex2y, Float_t vertex2z,
                   Float_t Bpt, Float_t Beta, Float_t Bphi) {
  TVector3 vector1 = TVector3(vertex1x, vertex1y, vertex1z);
  TVector3 vector2 = TVector3(vertex2x, vertex2y, vertex2z);
  TVector3 vector_bc;
  vector_bc.SetPtEtaPhi(Bpt, Beta, Bphi);
  Float_t alfa = vector_bc.Angle(vector2 - vector1);
  Float_t cosalfa = TMath::Cos(alfa);

  return cosalfa;
};

auto angle3mu = [](Float_t Bpt, Float_t Beta, Float_t Bphi,
                   Float_t mu1pt, Float_t mu1eta, Float_t mu1phi,
                   Float_t mu2pt, Float_t mu2eta, Float_t mu2phi,
                   Float_t kpt, Float_t keta, Float_t kphi) {
  TVector3 p_mu1, p_mu2, p_k;
  p_mu1.SetPtEtaPhi(mu1pt, mu1eta, mu1phi);
  p_mu2.SetPtEtaPhi(mu2pt, mu2eta, mu2phi);
  p_k.SetPtEtaPhi(kpt, keta, kphi);
  TVector3 threemuons = p_mu1 + p_mu2 + p_k;

  TVector3 vector_bc;
  vector_bc.SetPtEtaPhi(Bpt, Beta, Bphi);

  Float_t alfa3mu = vector_bc.Angle(threemuons);

  return alfa3mu;
};

auto cosangle3mu = [](Float_t Bpt, Float_t Beta, Float_t Bphi,
                      Float_t mu1pt, Float_t mu1eta, Float_t mu1phi,
                      Float_t mu2pt, Float_t mu2eta, Float_t mu2phi,
                      Float_t kpt, Float_t keta, Float_t kphi) {
  TVector3 p_mu1, p_mu2, p_k;
  p_mu1.SetPtEtaPhi(mu1pt, mu1eta, mu1phi);
  p_mu2.SetPtEtaPhi(mu2pt, mu2eta, mu2phi);
  p_k.SetPtEtaPhi(kpt, keta, kphi);
  TVector3 threemuons = p_mu1 + p_mu2 + p_k;

  TVector3 vector_bc;
  vector_bc.SetPtEtaPhi(Bpt, Beta, Bphi);

  Float_t alfa3mu = vector_bc.Angle(threemuons);
  Float_t cosalfa3mu = TMath::Cos(alfa3mu);

  return cosalfa3mu;
};

auto b_pisa_threemomentum = [](Float_t mu1_pt, Float_t mu1_eta, Float_t mu1_phi,
                               Float_t mu2_pt, Float_t mu2_eta, Float_t mu2_phi,
                               Float_t k_pt, Float_t k_eta, Float_t k_phi,
                               Float_t beamspot_x, Float_t beamspot_y, Float_t pv_z,
                               Float_t bvtx_vtx_x, Float_t bvtx_vtx_y, Float_t bvtx_vtx_z) {
  TVector3 mu1_threemomentum, mu2_threemomentum, k_threemomentum, threemuons;
  mu1_threemomentum.SetPtEtaPhi(mu1_pt, mu1_eta, mu1_phi);
  mu2_threemomentum.SetPtEtaPhi(mu2_pt, mu2_eta, mu2_phi);
  k_threemomentum.SetPtEtaPhi(k_pt, k_eta, k_phi);
  threemuons = mu1_threemomentum + mu2_threemomentum + k_threemomentum;

  TVector3 vector1 = TVector3(beamspot_x, beamspot_y, pv_z);
  TVector3 vector2 = TVector3(bvtx_vtx_x, bvtx_vtx_y, bvtx_vtx_z);
  Float_t phi = (vector2 - vector1).Phi();

  TVector3 b_pisa;
  b_pisa.SetPtEtaPhi(threemuons.Pt(), threemuons.Eta(), phi);

  return b_pisa;
};

auto squaredtransferredmomentumgen = [](Float_t Bpt, Float_t Beta, Float_t Bphi,
                                     Float_t mu1pt, Float_t mu1eta, Float_t mu1phi,
                                     Float_t mu2pt, Float_t mu2eta, Float_t mu2phi) {
  Float_t Bc_MASS = 6.2756;
  Float_t MUON_MASS = 0.1056583745;

  TLorentzVector pmu_B, pmu_mu1, pmu_mu2;

  pmu_B.SetPtEtaPhiM(Bpt, Beta, Bphi, Bc_MASS);
  pmu_mu1.SetPtEtaPhiM(mu1pt, mu1eta, mu1phi, MUON_MASS);
  pmu_mu2.SetPtEtaPhiM(mu2pt, mu2eta, mu2phi, MUON_MASS);

  return (Float_t)(pmu_B - pmu_mu1 - pmu_mu2).Mag2();
};

auto squaredtransferredmomentumjona = [](Float_t Bpt, Float_t Beta, Float_t Bphi, Float_t Bmass,
                                     Float_t mu1pt, Float_t mu1eta, Float_t mu1phi, Float_t mu1mass,
                                     Float_t mu2pt, Float_t mu2eta, Float_t mu2phi, Float_t mu2mass) {
  Float_t Bc_MASS = 6.2756;

  TLorentzVector pmu_B, pmu_mu1, pmu_mu2;
  Float_t BPt = Bpt*(Bc_MASS/Bmass);

  pmu_B.SetPtEtaPhiM(BPt, Beta, Bphi, Bc_MASS);
  pmu_mu1.SetPtEtaPhiM(mu1pt, mu1eta, mu1phi, mu1mass);
  pmu_mu2.SetPtEtaPhiM(mu2pt, mu2eta, mu2phi, mu2mass);

  return (Float_t)(pmu_B - pmu_mu1 - pmu_mu2).Mag2();
};

auto squaredtransferredmomentumpisa = [](TVector3 b_pisa, Float_t Bmass,
                                         Float_t mu1pt, Float_t mu1eta, Float_t mu1phi, Float_t mu1mass,
                                         Float_t mu2pt, Float_t mu2eta, Float_t mu2phi, Float_t mu2mass) {
  Float_t Bc_MASS = 6.2756;

  TLorentzVector pmu_b_pisa, pmu_mu1, pmu_mu2;
  Float_t BPt = b_pisa.Pt()*(Bc_MASS/Bmass);
  
  pmu_b_pisa.SetPtEtaPhiM(BPt, b_pisa.Eta(), b_pisa.Phi(), Bc_MASS);
  pmu_mu1.SetPtEtaPhiM(mu1pt, mu1eta, mu1phi, mu1mass);
  pmu_mu2.SetPtEtaPhiM(mu2pt, mu2eta, mu2phi, mu2mass);

  return (Float_t)(pmu_b_pisa - pmu_mu1 - pmu_mu2).Mag2();
};

auto missingmasssquaredgen = [](Float_t Bpt, Float_t Beta, Float_t Bphi,
                             Float_t mu1pt, Float_t mu1eta, Float_t mu1phi,
                             Float_t mu2pt, Float_t mu2eta, Float_t mu2phi,
                             Float_t kpt, Float_t keta, Float_t kphi) {
  Float_t Bc_MASS = 6.2756;
  Float_t MUON_MASS = 0.1056583745;

  TLorentzVector pmu_B, pmu_mu1, pmu_mu2, pmu_k;

  pmu_B.SetPtEtaPhiM(Bpt, Beta, Bphi, Bc_MASS);
  pmu_mu1.SetPtEtaPhiM(mu1pt, mu1eta, mu1phi, MUON_MASS);
  pmu_mu2.SetPtEtaPhiM(mu2pt, mu2eta, mu2phi, MUON_MASS);
  pmu_k.SetPtEtaPhiM(kpt, keta, kphi, MUON_MASS);

  return (Float_t)(pmu_B - pmu_mu1 - pmu_mu2 - pmu_k).Mag2();
};

auto missingmasssquaredjona = [](Float_t Bpt, Float_t Beta, Float_t Bphi, Float_t Bmass,
                             Float_t mu1pt, Float_t mu1eta, Float_t mu1phi, Float_t mu1mass,
                             Float_t mu2pt, Float_t mu2eta, Float_t mu2phi, Float_t mu2mass,
                             Float_t kpt, Float_t keta, Float_t kphi, Float_t kmass) {
  Float_t Bc_MASS = 6.2756;

  TLorentzVector pmu_B, pmu_mu1, pmu_mu2, pmu_k;
  Float_t BPt = Bpt*(Bc_MASS/Bmass);

  pmu_B.SetPtEtaPhiM(BPt, Beta, Bphi, Bc_MASS);
  pmu_mu1.SetPtEtaPhiM(mu1pt, mu1eta, mu1phi, mu1mass);
  pmu_mu2.SetPtEtaPhiM(mu2pt, mu2eta, mu2phi, mu2mass);
  pmu_k.SetPtEtaPhiM(kpt, keta, kphi, kmass);

  return (Float_t)(pmu_B - pmu_mu1 - pmu_mu2 - pmu_k).Mag2();
};

auto missingmasssquaredpisa = [](TVector3 b_pisa, Float_t Bmass,
                             Float_t mu1pt, Float_t mu1eta, Float_t mu1phi, Float_t mu1mass,
                             Float_t mu2pt, Float_t mu2eta, Float_t mu2phi, Float_t mu2mass,
                             Float_t kpt, Float_t keta, Float_t kphi, Float_t kmass) {
  Float_t Bc_MASS = 6.2756;

  TLorentzVector pmu_b_pisa, pmu_mu1, pmu_mu2, pmu_k;
  Float_t BPt = b_pisa.Pt()*(Bc_MASS/Bmass);

  pmu_b_pisa.SetPtEtaPhiM(BPt, b_pisa.Eta(), b_pisa.Phi(), Bc_MASS);
  pmu_mu1.SetPtEtaPhiM(mu1pt, mu1eta, mu1phi, mu1mass);
  pmu_mu2.SetPtEtaPhiM(mu2pt, mu2eta, mu2phi, mu2mass);
  pmu_k.SetPtEtaPhiM(kpt, keta, kphi, kmass);

  return (Float_t)(pmu_b_pisa - pmu_mu1 - pmu_mu2 - pmu_k).Mag2();
};

auto missingptgen = [](Float_t Bpt, Float_t mu1pt, Float_t mu2pt, Float_t kpt) {
  return (Float_t)(Bpt - mu1pt - mu2pt - kpt);
};

auto missingptjona = [](Float_t Bpt, Float_t Bmass, Float_t mu1pt, Float_t mu2pt, Float_t kpt) {
  Float_t Bc_MASS = 6.2756;

  Float_t BPt = Bpt * (Bc_MASS / Bmass);

  return (Float_t)(BPt - mu1pt - mu2pt - kpt);
};

auto missingptpisa = [](TVector3 b_pisa, Float_t Bmass, Float_t mu1pt, Float_t mu2pt, Float_t kpt) {
  Float_t Bc_MASS = 6.2756;

  Float_t BPt = b_pisa.Pt() * (Bc_MASS / Bmass);

  return (Float_t)(BPt - mu1pt - mu2pt - kpt);
};

#endif