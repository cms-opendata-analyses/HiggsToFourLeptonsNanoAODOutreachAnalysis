# Implementation of the histogramming step of the analysis
#
# The histogramming step produces histograms for each variable in the dataset
# and for each physics process resulting into the final state with a combination
# of muons and a electrons. Then, the resulting histograms are passed to the plotting
# step, which combines the histograms so that we can study the physics of the decay.


import ROOT
ROOT.gROOT.SetBatch(True)


# Declare the range of the histogram for each variable
#
# Each entry in the dictionary contains of the variable name as key and a tuple
# specifying the histogram layout as value. The tuple sets the number of bins,
# the lower edge and the upper edge of the histogram.
default_nbins = 36
ranges = {
        "Higgs_mass": (default_nbins, 70, 180),
        "Z1_mass": (default_nbins, 40, 120),
        "Z2_mass": (default_nbins, 12, 120)
        }


# Book a histogram for a specific variable
def bookHistogram(df, variable, range_):
    return df.Histo1D(ROOT.ROOT.RDF.TH1DModel(variable, variable, range_[0], range_[1], range_[2]),\
                      variable, "weight")


# Write a histogram with a given name to the output ROOT file
def writeHistogram(h, name):
    h.SetName(name)
    h.Write()


# Main function of the histogramming step
#
# The function loops over the outputs from the skimming step and produces the
# required histograms for the final plotting.
def main():
    # Set up multi-threading capability of ROOT
    ROOT.ROOT.EnableImplicitMT()
    poolSize = ROOT.ROOT.GetThreadPoolSize()
    print(">>> Thread pool size for parallel processing: {}".format(poolSize))

    # Create output file
    tfile = ROOT.TFile("histograms.root", "RECREATE")
    variables = ranges.keys()

    # Loop through skimmed datasets and final states to produce histograms of all variables
    for sample, final_states in {
                    "SMHiggsToZZTo4L": ["FourMuons", "FourElectrons", "TwoMuonsTwoElectrons"],
                    "ZZTo4mu": ["FourMuons"],
                    "ZZTo4e": ["FourElectrons"],
                    "ZZTo2e2mu": ["TwoMuonsTwoElectrons"],
                    "Run2012B_DoubleMuParked": ["FourMuons", "TwoMuonsTwoElectrons"],
                    "Run2012C_DoubleMuParked": ["FourMuons", "TwoMuonsTwoElectrons"],
                    "Run2012B_DoubleElectron": ["FourElectrons", "TwoMuonsTwoElectrons"],
                    "Run2012C_DoubleElectron": ["FourElectrons", "TwoMuonsTwoElectrons"]
                }.items():
        for final_state in final_states:
            print(">>> Process skimmed sample {} and final state {}".format(sample, final_state))

            # Make dataframe of the skimmed dataset
            df = ROOT.ROOT.RDataFrame("Events", sample + final_state + "Skim.root")

            # Book histograms
            hists = {}
            for variable in variables:
                hists[variable] = bookHistogram(df, variable, ranges[variable])

            # Write histograms to output file
            for variable in variables:
                writeHistogram(hists[variable], "{}_{}_{}".format(sample, final_state, variable))

    tfile.Close()


if __name__ == "__main__":
    main()
