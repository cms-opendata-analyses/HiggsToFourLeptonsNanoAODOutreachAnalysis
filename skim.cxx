/*
 * Implementation of the skimming step of the analysis
 *
 * The skimming step reduces the inital generic samples to a dataset optimized
 * for this specific analysis. Most important, the skimming removes all events
 * from the initial dataset, which are not of interest for our study and reconstructs
 * Z bosons from combinations of muons and electrons, which may originate from
 * the decay of a Higgs boson.
 */


#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector4D.h"
#include "TStopwatch.h"

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <exception>


/*
 * Base path to local filesystem or to EOS containing the datasets
 */
const std::string samplesBasePath = "root://eospublic.cern.ch//eos/opendata/cms/upload/stefan/HiggsToFourLeptonsNanoAODOutreachAnalysis/";


/*
 * Names of the datasets to be found in the base path and processed for the analysis and
 * the final states described by the dataset
 *
 * The analysis searches the Higgs in the four lepton final state, which can be built
 * from either four muons, four electrons, or two muon and two electrons. Not all simulations
 * cover all final states and therefore we have to distinguish between them in the processing.
 */
std::map<std::string, std::vector<std::string>> samples = {
    {"SMHiggsToZZTo4L", {"FourMuons", "FourElectrons", "TwoMuonsTwoElectrons"}},
    {"ZZTo4mu", {"FourMuons"}},
    {"ZZTo4e", {"FourElectrons"}},
    {"ZZTo2e2mu", {"TwoMuonsTwoElectrons"}},
    {"Run2012B_DoubleMuParked", {"FourMuons", "TwoMuonsTwoElectrons"}},
    {"Run2012C_DoubleMuParked", {"FourMuons", "TwoMuonsTwoElectrons"}},
    {"Run2012B_DoubleElectron", {"FourElectrons", "TwoMuonsTwoElectrons"}},
    {"Run2012C_DoubleElectron", {"FourElectrons", "TwoMuonsTwoElectrons"}}
};


/*
 * Compute event weights to be used for the respective datasets
 *
 * The event weight reweights the full dataset so that the sum of the weights
 * is equal to the expected number of events in data. The expectation is given by
 * multiplying the integrated luminosity of the data with the cross-section of
 * the process in the datasets divided by the number of simulated events.
 */
const float integratedLuminosity = 11.58 * 1000.0;
const float scaleFactorZZTo4l = 1.386; // Correction of the simulation
std::map<std::string, float> eventWeights = {
    {"SMHiggsToZZTo4L", 0.0065 / 299973.0 * integratedLuminosity},
    {"ZZTo4mu", 0.077 / 1499064.0 * scaleFactorZZTo4l * integratedLuminosity},
    {"ZZTo4e", 0.077 / 1499093.0 * scaleFactorZZTo4l * integratedLuminosity},
    {"ZZTo2e2mu", 0.18 / 1497445.0 * scaleFactorZZTo4l * integratedLuminosity},
    {"Run2012B_DoubleMuParked", 1.0},
    {"Run2012C_DoubleMuParked", 1.0},
    {"Run2012B_DoubleElectron", 1.0},
    {"Run2012C_DoubleElectron", 1.0}
};


/*
 * Perform a selection on the minimal requirements of an event
 */
ROOT::RDF::RNode MinimalSelection(ROOT::RDF::RNode df, const std::string& finalState) {
    if (finalState.compare("FourMuons") == 0) {
        return df.Filter("nMuon>=4",
                         "At least four muons")
                 .Filter("All(abs(Muon_pfRelIso04_all)<0.40)",
                         "Require good isolation")
                 .Filter("All(Muon_pt>5) && All(abs(Muon_eta)<2.4)",
                         "Good muon kinematics")
                 .Define("Muon_ip3d",
                         "sqrt(Muon_dxy*Muon_dxy + Muon_dz*Muon_dz)")
                 .Define("Muon_sip3d",
                         "Muon_ip3d/sqrt(Muon_dxyErr*Muon_dxyErr + Muon_dzErr*Muon_dzErr)")
                 .Filter("All(Muon_sip3d<4) && All(abs(Muon_dxy)<0.5) && All(abs(Muon_dz)<1.0)",
                         "Track close to primary vertex with small uncertainty")
                 .Filter("nMuon==4 && Sum(Muon_charge==1)==2 && Sum(Muon_charge==-1)==2",
                         "Two positive and two negative muons");
    } else if (finalState.compare("FourElectrons") == 0) {
        return df.Filter("nElectron>=4",
                         "At least our electrons")
                 .Filter("All(abs(Electron_pfRelIso03_all)<0.40)",
                         "Require good isolation")
                 .Filter("All(Electron_pt>7) && All(abs(Electron_eta)<2.5)",
                         "Good Electron kinematics")
                 .Define("Electron_ip3d",
                         "sqrt(Electron_dxy*Electron_dxy + Electron_dz*Electron_dz)")
                 .Define("Electron_sip3d",
                         "Electron_ip3d/sqrt(Electron_dxyErr*Electron_dxyErr + Electron_dzErr*Electron_dzErr)")
                 .Filter("All(Electron_sip3d<4) && All(abs(Electron_dxy)<0.5) && All(abs(Electron_dz)<1.0)",
                         "Track close to primary vertex with small uncertainty")
                 .Filter("nElectron==4 && Sum(Electron_charge==1)==2 && Sum(Electron_charge==-1)==2",
                         "Two positive and two negative electrons");
    } else if (finalState.compare("TwoMuonsTwoElectrons") == 0) {
        using Vec = const ROOT::RVec<float>&;
        auto pt_cuts = [](Vec mu_pt, Vec el_pt)
        {
            auto mu_pt_sorted = Reverse(Sort(mu_pt));
            if (mu_pt_sorted[0] > 20 && mu_pt_sorted[1] > 10) return true;
            auto el_pt_sorted = Reverse(Sort(el_pt));
            if (el_pt_sorted[0] > 20 && el_pt_sorted[1] > 10) return true;
            return false;
        };
        auto dr_cuts = [](Vec mu_eta, Vec mu_phi, Vec el_eta, Vec el_phi)
        {
            auto mu_dr = ROOT::VecOps::DeltaR(mu_eta[0], mu_eta[1], mu_phi[0], mu_phi[1]);
            auto el_dr = ROOT::VecOps::DeltaR(el_eta[0], el_eta[1], el_phi[0], el_phi[1]);
            if (mu_dr < 0.02 || el_dr < 0.02) return false;
            return true;
        };
        return df.Filter("nElectron>=2 && nMuon>=2",
                         "At least two electrons and two muons")
                 .Filter("All(abs(Electron_eta)<2.5) && All(abs(Muon_eta)<2.4)",
                         "Eta cuts")
                 .Filter(pt_cuts, {"Muon_pt", "Electron_pt"},
                         "Pt cuts")
                 .Filter(dr_cuts, {"Muon_eta", "Muon_phi", "Electron_eta", "Electron_phi"},
                         "Delta R cuts")
                 .Filter("All(abs(Electron_pfRelIso03_all)<0.40) && All(abs(Muon_pfRelIso04_all)<0.40)",
                         "Require good isolation")
                 .Define("Electron_ip3d_el",
                         "sqrt(Electron_dxy*Electron_dxy + Electron_dz*Electron_dz)")
                 .Define("Electron_sip3d_el",
                         "Electron_ip3d_el/sqrt(Electron_dxyErr*Electron_dxyErr + Electron_dzErr*Electron_dzErr)")
                 .Filter("All(Electron_sip3d_el<4) && All(abs(Electron_dxy)<0.5) && All(abs(Electron_dz)<1.0)",
                         "Electron track close to primary vertex with small uncertainty")
                 .Define("Muon_ip3d_mu",
                         "sqrt(Muon_dxy*Muon_dxy + Muon_dz*Muon_dz)")
                 .Define("Muon_sip3d_mu",
                         "Muon_ip3d_mu/sqrt(Muon_dxyErr*Muon_dxyErr + Muon_dzErr*Muon_dzErr)")
                 .Filter("All(Muon_sip3d_mu<4) && All(abs(Muon_dxy)<0.5) && All(abs(Muon_dz)<1.0)",
                         "Muon track close to primary vertex with small uncertainty")
                 .Filter("Sum(Electron_charge)==0 && Sum(Muon_charge)==0",
                         "Two opposite charged electron and muon pairs");
    } else {
        throw std::runtime_error("Unknown final state " + finalState);
    }
}


/*
 * Reconstruct the Higgs system from the two Z boson decaying into combinations
 * of four leptons in the final state
 *
 * For the final states with four leptons of the same kind, the reconstruction
 * is non-trivial and selects the two pairs being closest to the known Z mass.
 * In the case of two muons and two electrons, the reconstruction of the Z is
 * trivial, each Z is built from two leptons of the same kind.
 */
ROOT::RDF::RNode ReconstructHiggs(ROOT::RDF::RNode df, const std::string& finalState) {
    using VecF = const ROOT::RVec<float>&;
    using VecI = const ROOT::RVec<int>&;
    using Idx = const ROOT::RVec<ROOT::RVec<int>>&;
    const auto z_mass = 91.2;

    // Reconstruct two Z candidates from four leptons of the same kind
    auto reconstruct_samekind = [z_mass](VecF pt, VecF eta, VecF phi, VecF mass, VecI charge)
    {
        ROOT::RVec<ROOT::RVec<int>> idx(2);
        idx[0].reserve(2); idx[1].reserve(2);

        // Find first lepton pair with invariant mass closest to Z mass
        auto idx_cmb = Combinations(pt, 2);
        auto best_mass = -1;
        size_t best_i1 = 0; size_t best_i2 = 0;
        for (size_t i = 0; i < idx_cmb[0].size(); i++) {
            const auto i1 = idx_cmb[0][i];
            const auto i2 = idx_cmb[1][i];
            if (charge[i1] != charge[i2]) {
                ROOT::Math::PtEtaPhiMVector p1(pt[i1], eta[i1], phi[i1], mass[i1]);
                ROOT::Math::PtEtaPhiMVector p2(pt[i2], eta[i2], phi[i2], mass[i2]);
                const auto this_mass = (p1 + p2).M();
                if (std::abs(z_mass - this_mass) < std::abs(z_mass - best_mass)) {
                    best_mass = this_mass;
                    best_i1 = i1;
                    best_i2 = i2;
                }
            }
        }
        idx[0].emplace_back(best_i1);
        idx[0].emplace_back(best_i2);

        // Reconstruct second Z from remaining lepton pair
        for (size_t i = 0; i < 4; i++) {
            if (i != best_i1 && i != best_i2) {
                idx[1].emplace_back(i);
            }
        }

        // Return indices of the pairs building two Z bosons
        return idx;
    };

    // Compute Z masses from four leptons of the same kind and sort ascending in distance to Z mass
    auto z_fourvectors_samekind = [z_mass](Idx idx, VecF pt, VecF eta, VecF phi, VecF mass) {
        ROOT::RVec<ROOT::Math::PtEtaPhiMVector> z_fourvecs(2);
        for (size_t i = 0; i < 2; i++) {
            const auto i1 = idx[i][0];
            const auto i2 = idx[i][1];
            ROOT::Math::PtEtaPhiMVector p1(pt[i1], eta[i1], phi[i1], mass[i1]);
            ROOT::Math::PtEtaPhiMVector p2(pt[i2], eta[i2], phi[i2], mass[i2]);
            z_fourvecs[i] = p1 + p2;
        }
        if (std::abs(z_fourvecs[0].mass() - z_mass) < std::abs(z_fourvecs[1].mass() - z_mass)) {
            return z_fourvecs;
        } else {
            return ROOT::VecOps::Reverse(z_fourvecs);
        }
    };

    // Compute mass of two Z candidates from two electrons and two muons and sort ascending in distance to Z mass
    auto z_fourvectors_2el2mu = [z_mass](VecF el_pt, VecF el_eta, VecF el_phi, VecF el_mass,
                                      VecF mu_pt, VecF mu_eta, VecF mu_phi, VecF mu_mass) {
        ROOT::Math::PtEtaPhiMVector p1(mu_pt[0], mu_eta[0], mu_phi[0], mu_mass[0]);
        ROOT::Math::PtEtaPhiMVector p2(mu_pt[1], mu_eta[1], mu_phi[1], mu_mass[1]);
        ROOT::Math::PtEtaPhiMVector p3(el_pt[0], el_eta[0], el_phi[0], el_mass[0]);
        ROOT::Math::PtEtaPhiMVector p4(el_pt[1], el_eta[1], el_phi[1], el_mass[1]);
        ROOT::RVec<ROOT::Math::PtEtaPhiMVector> z_fourvecs = {p1 + p2, p3 + p4};
        if (std::abs(z_fourvecs[0].mass() - z_mass) < std::abs(z_fourvecs[1].mass() - z_mass)) {
            return z_fourvecs;
        } else {
            return ROOT::VecOps::Reverse(z_fourvecs);
        }
    };

    // Cut on distance between leptons building Z systems
    auto filter_deltar = [](Idx idx, VecF eta, VecF phi) {
        for (size_t i = 0; i < 2; i++) {
            const auto i1 = idx[i][0];
            const auto i2 = idx[i][1];
            const auto dr = ROOT::VecOps::DeltaR(eta[i1], eta[i2], phi[i1], phi[i2]);
            if (dr < 0.02) return false;
        }
        return true;
    };

    // Reconstruct the ZZ system for all final states
    ROOT::RDF::RNode df_reco = df;
    if (finalState.compare("FourMuons") == 0) {
        df_reco = df.Define("Z_idx", reconstruct_samekind,
                            {"Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass", "Muon_charge"})
                    .Filter(filter_deltar, {"Z_idx", "Muon_eta", "Muon_phi"},
                            "Delta R separation of particles building the Z systems")
                    .Define("Z_fourvecs", z_fourvectors_samekind,
                            {"Z_idx", "Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass"});
    } else if (finalState.compare("FourElectrons") == 0) {
        df_reco = df.Define("Z_idx", reconstruct_samekind,
                            {"Electron_pt", "Electron_eta", "Electron_phi", "Electron_mass", "Electron_charge"})
                    .Filter(filter_deltar, {"Z_idx", "Electron_eta", "Electron_phi"},
                            "Delta R separation of particles building the Z systems")
                    .Define("Z_fourvecs", z_fourvectors_samekind,
                            {"Z_idx", "Electron_pt", "Electron_eta", "Electron_phi", "Electron_mass"});
    } else if (finalState.compare("TwoMuonsTwoElectrons") == 0) {
        // With exactly two muons and two electrons, the reconstruction is trivial (each Z is built from two of the same kind).
        df_reco = df.Define("Z_fourvecs", z_fourvectors_2el2mu,
                            {"Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass", "Electron_pt", "Electron_eta", "Electron_phi", "Electron_mass"});
    } else {
        throw std::runtime_error("Unknown final state " + finalState);
    }

    // Apply cut on the reconstructed Z masses
    auto df_cut = df_reco.Filter("Z_fourvecs[0].mass() > 40 && Z_fourvecs[0].mass() < 120",
                                 "Mass of first Z candidate in [40, 120]")
                         .Filter("Z_fourvecs[1].mass() > 12 && Z_fourvecs[1].mass() < 120",
                                 "Mass of second Z candidate in [12, 120]");

    // Combine the fourvectors of the two Z bosons to the fourvector of the reconstructed Higgs boson
    return df_cut.Define("Higgs_fourvec", "Z_fourvecs[0] + Z_fourvecs[1]");
}


/*
 * Declare all variables which we want to study in the analysis
 */
ROOT::RDF::RNode DeclareVariables(ROOT::RDF::RNode df) {
    return df.Define("Higgs_mass", "Higgs_fourvec.mass()")
             .Define("Z1_mass", "Z_fourvecs[0].mass()")
             .Define("Z2_mass", "Z_fourvecs[1].mass()");
}


/*
 * Add the event weight to the dataset as the column "weight"
 */
ROOT::RDF::RNode AddEventWeight(ROOT::RDF::RNode df, const std::string& sample) {
    const auto weight = eventWeights[sample];
    return df.Define("weight", [weight](){ return weight; });
}


/*
 * Declare all variables which shall end up in the final reduced dataset
 */
const std::vector<std::string> finalVariables = {
    "run", "weight", "Higgs_mass", "Z1_mass", "Z2_mass"
};


int main() {
    ROOT::EnableImplicitMT();
    const auto poolSize = ROOT::GetThreadPoolSize();
    std::cout << ">>> Thread pool size for parallel processing: " << poolSize << std::endl;

    for (const auto &sample : samples) {
        const auto name = sample.first;
        const auto finalStates = sample.second;
        std::cout << ">>> Process sample " << name << ":" << std::endl;
        ROOT::RDataFrame df("Events", samplesBasePath + name + ".root");

        for (const auto &finalState : finalStates) {
            std::cout << ">>> Process final state " << finalState << " for sample " << name << ":" << std::endl;
            TStopwatch time;
            time.Start();

            auto df2 = MinimalSelection(df, finalState);
            auto df3 = ReconstructHiggs(df2, finalState);
            auto df4 = DeclareVariables(df3);
            auto df5 = AddEventWeight(df4, name);

            auto dfFinal = df5;
            auto report = dfFinal.Report();
            dfFinal.Snapshot("Events", name + finalState + "Skim.root", finalVariables);
            report->Print();
            time.Stop();
            time.Print();
        }
    }
}
