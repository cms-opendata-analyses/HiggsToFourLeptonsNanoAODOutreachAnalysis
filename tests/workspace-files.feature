# Tests for the presence of files in the workspace

Feature: Workspace files

    As a researcher,
    I want to make sure that my workflow produces the expected files,
    and that the relevant input files are present in the workspace,
    so that I can be sure that the workflow outputs are correct.

    Scenario: The workspace contains all the relevant input files
        When the workflow execution completes
        Then the workspace should contain "skim.cxx"
        And the workspace should include "histograms.py"
        And the workspace should include "plot.py"

    Rule: The output files and reports are generated correctly

        Scenario: The expected plots are present in the workspace
            When the workflow is finished
            Then the workspace should include "combined_Higgs_mass.pdf"
            And the workspace should include "combined_Higgs_mass.png"
            And the workspace should include "combined_Z1_mass.pdf"
            And the workspace should include "combined_Z1_mass.png"
            And the workspace should include "combined_Z2_mass.pdf"
            And the workspace should include "combined_Z2_mass.png"
            And the workspace should include "FourElectrons_Higgs_mass.pdf"
            And the workspace should include "FourElectrons_Higgs_mass.png"
            And the workspace should include "FourElectrons_Z1_mass.pdf"
            And the workspace should include "FourElectrons_Z1_mass.png"
            And the workspace should include "FourElectrons_Z2_mass.pdf"
            And the workspace should include "FourElectrons_Z2_mass.png"
            And the workspace should include "FourMuons_Higgs_mass.pdf"
            And the workspace should include "FourMuons_Higgs_mass.png"
            And the workspace should include "FourMuons_Z1_mass.pdf"
            And the workspace should include "FourMuons_Z1_mass.png"
            And the workspace should include "FourMuons_Z2_mass.pdf"
            And the workspace should include "FourMuons_Z2_mass.png"
            And the workspace should include "TwoMuonsTwoElectrons_Higgs_mass.pdf"
            And the workspace should include "TwoMuonsTwoElectrons_Higgs_mass.png"
            And the workspace should include "TwoMuonsTwoElectrons_Z1_mass.pdf"
            And the workspace should include "TwoMuonsTwoElectrons_Z1_mass.png"
            And the workspace should include "TwoMuonsTwoElectrons_Z2_mass.pdf"
            And the workspace should include "TwoMuonsTwoElectrons_Z2_mass.png"

        Scenario: The expected plots represent the expected data
            When the workflow is finished
            Then the size of the file "FourMuons_Z2_mass.pdf" should be between "16 KiB" and "17 KiB"
            And the size of the file "TwoMuonsTwoElectrons_Higgs_mass.png" should be between "13 KiB" and "14 KiB"
            And the size of the file "FourMuons_Z1_mass.png" should be between "14 KiB" and "15 KiB"
            And the size of the file "plot.py" should be between "6 KiB" and "7 KiB"
            And the size of the file "FourMuons_Higgs_mass.png" should be between "13 KiB" and "14 KiB"
            And the size of the file "histograms.py" should be between "3 KiB" and "4 KiB"
            And the size of the file "combined_Z2_mass.png" should be between "15 KiB" and "16 KiB"
            And the size of the file "TwoMuonsTwoElectrons_Higgs_mass.pdf" should be between "16 KiB" and "17 KiB"
            And the size of the file "TwoMuonsTwoElectrons_Z2_mass.png" should be between "14 KiB" and "15 KiB"
            And the size of the file "FourElectrons_Z1_mass.pdf" should be between "15 KiB" and "16 KiB"
            And the size of the file "combined_Z1_mass.png" should be between "14 KiB" and "15 KiB"
            And the size of the file "FourElectrons_Z1_mass.png" should be between "13 KiB" and "14 KiB"
            And the size of the file "combined_Higgs_mass.pdf" should be between "17 KiB" and "18 KiB"
            And the size of the file "FourElectrons_Z2_mass.png" should be between "14 KiB" and "15 KiB"
            And the size of the file "FourElectrons_Higgs_mass.pdf" should be between "15 KiB" and "16 KiB"
            And the size of the file "combined_Z1_mass.pdf" should be between "16 KiB" and "17 KiB"
            And the size of the file "FourMuons_Z1_mass.pdf" should be between "16 KiB" and "17 KiB"
            And the size of the file "TwoMuonsTwoElectrons_Z2_mass.pdf" should be between "17 KiB" and "18 KiB"
            And the size of the file "TwoMuonsTwoElectrons_Z1_mass.pdf" should be between "15.5 KiB" and "16.5 KiB"
            And the size of the file "combined_Higgs_mass.png" should be between "15 KiB" and "16 KiB"
            And the size of the file "FourElectrons_Z2_mass.pdf" should be between "16 KiB" and "17 KiB"
            And the size of the file "FourElectrons_Higgs_mass.png" should be between "12 KiB" and "13 KiB"
            And the size of the file "combined_Z2_mass.pdf" should be between "17 KiB" and "18 KiB"
            And the size of the file "TwoMuonsTwoElectrons_Z1_mass.png" should be between "13 KiB" and "14 KiB"
            And the size of the file "FourMuons_Z2_mass.png" should be between "14 KiB" and "15 KiB"
            And the size of the file "FourMuons_Higgs_mass.pdf" should be between "16 KiB" and "17 KiB"

