# Tests for the expected workflow log messages

Feature: Log messages

    As a researcher,
    I want to be able to see the log messages of my workflow execution,
    So that I can verify that the workflow ran correctly.

    Scenario: The compiling step has produced the expected messages
        When the workflow is finished
        Then the engine logs should contain
            """
            Publishing step:0, cmd: g++ -g -O3 -Wall -Wextra -Wpedantic -o skim skim.cxx `root-config --cflags --libs`, total steps 4 to MQ
            """

    Scenario: The skimming step has produced the expected messages
        When the workflow is finished
        Then the engine logs should contain "Publishing step:1, cmd: ./skim"
        And the job logs for the "skimming" step should contain
            """
            >>> Process final state FourElectrons for sample Run2012B_DoubleElectron:
            At least our electrons: pass=258449     all=21474287   -- eff=1.20 % cumulative eff=1.20 %
            Require good isolation: pass=113        all=258449     -- eff=0.04 % cumulative eff=0.00 %
            Good Electron kinematics: pass=67         all=113        -- eff=59.29 % cumulative eff=0.00 %
            """

    Scenario: The histogramming step has produced the expected messages
        When the workflow is finished
        Then the engine logs should contain "Publishing step:2, cmd: python histograms.py"
        And the job logs for the "histogramming" step should contain
            """
            >>> Process skimmed sample SMHiggsToZZTo4L and final state FourMuons
            >>> Process skimmed sample SMHiggsToZZTo4L and final state FourElectrons
            >>> Process skimmed sample SMHiggsToZZTo4L and final state TwoMuonsTwoElectrons
            >>> Process skimmed sample ZZTo4mu and final state FourMuons
            >>> Process skimmed sample ZZTo4e and final state FourElectrons
            >>> Process skimmed sample ZZTo2e2mu and final state TwoMuonsTwoElectrons
            >>> Process skimmed sample Run2012B_DoubleMuParked and final state FourMuons
            >>> Process skimmed sample Run2012B_DoubleMuParked and final state TwoMuonsTwoElectrons
            >>> Process skimmed sample Run2012C_DoubleMuParked and final state FourMuons
            >>> Process skimmed sample Run2012C_DoubleMuParked and final state TwoMuonsTwoElectrons
            >>> Process skimmed sample Run2012B_DoubleElectron and final state FourElectrons
            >>> Process skimmed sample Run2012B_DoubleElectron and final state TwoMuonsTwoElectrons
            >>> Process skimmed sample Run2012C_DoubleElectron and final state FourElectrons
            >>> Process skimmed sample Run2012C_DoubleElectron and final state TwoMuonsTwoElectrons
            """

    Scenario: The plotting step has produced the expected messages
        When the workflow is finished
        Then the engine logs should contain "Publishing step:3, cmd: python plot.py"
        And the job logs for the "plotting" step should contain
            """
            Info in <TCanvas::Print>: pdf file FourMuons_Higgs_mass.pdf has been created
            Info in <TCanvas::Print>: png file FourMuons_Higgs_mass.png has been created
            Info in <TCanvas::Print>: pdf file FourElectrons_Higgs_mass.pdf has been created
            Info in <TCanvas::Print>: png file FourElectrons_Higgs_mass.png has been created
            Info in <TCanvas::Print>: pdf file TwoMuonsTwoElectrons_Higgs_mass.pdf has been created
            Info in <TCanvas::Print>: png file TwoMuonsTwoElectrons_Higgs_mass.png has been created
            Info in <TCanvas::Print>: pdf file combined_Higgs_mass.pdf has been created
            Info in <TCanvas::Print>: png file combined_Higgs_mass.png has been created
            Info in <TCanvas::Print>: pdf file FourMuons_Z1_mass.pdf has been created
            Info in <TCanvas::Print>: png file FourMuons_Z1_mass.png has been created
            Info in <TCanvas::Print>: pdf file FourElectrons_Z1_mass.pdf has been created
            Info in <TCanvas::Print>: png file FourElectrons_Z1_mass.png has been created
            Info in <TCanvas::Print>: pdf file TwoMuonsTwoElectrons_Z1_mass.pdf has been created
            Info in <TCanvas::Print>: png file TwoMuonsTwoElectrons_Z1_mass.png has been created
            Info in <TCanvas::Print>: pdf file combined_Z1_mass.pdf has been created
            Info in <TCanvas::Print>: png file combined_Z1_mass.png has been created
            Info in <TCanvas::Print>: pdf file FourMuons_Z2_mass.pdf has been created
            Info in <TCanvas::Print>: png file FourMuons_Z2_mass.png has been created
            Info in <TCanvas::Print>: pdf file FourElectrons_Z2_mass.pdf has been created
            Info in <TCanvas::Print>: png file FourElectrons_Z2_mass.png has been created
            Info in <TCanvas::Print>: pdf file TwoMuonsTwoElectrons_Z2_mass.pdf has been created
            Info in <TCanvas::Print>: png file TwoMuonsTwoElectrons_Z2_mass.png has been created
            Info in <TCanvas::Print>: pdf file combined_Z2_mass.pdf has been created
            Info in <TCanvas::Print>: png file combined_Z2_mass.png has been created
            """