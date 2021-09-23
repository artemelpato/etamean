int FitEtaMeans() {

    const int nSec = 8;
    const int nCent = 4;

    const double maxMeanError = 0.2;
    const double maxMean = 1;

    const double maxSigmaError = 0.05;
    const double maxSigma = 0.2;

    const double fitMin = 2.1;
    const double fitMax = 15; 

    std::array <int, nCent> centClasses = { 0, 20, 40, 60 };

    TFile *outfile = new TFile("EtaMeansFit.root", "recreate");

    std::ifstream intxtfile;
    intxtfile.open("eta-means.txt");

    std::ofstream meansOutFile;
    meansOutFile.open("eta-means-functions.txt");

    std::ofstream sigmasOutFile;
    sigmasOutFile.open("eta-sigmas-functions.txt");

    std::array <std::array <TGraphErrors*, nCent>, nSec> meanGraphs;
    std::array <std::array <TGraphErrors*, nCent>, nSec> sigmaGraphs;

    std::array <std::array <TF1*, nCent>, nSec> meanFits;
    std::array <std::array <TF1*, nCent>, nSec> sigmaFits;

    std::array <std::array <int, nCent>, nSec> meanPoints;
    std::array <std::array <int, nCent>, nSec> sigmaPoints;

    char name[50];

    for (int sec = 0; sec < meanGraphs.size(); sec++) {
        for (int cent = 0; cent < meanGraphs[sec].size(); cent++) {

            meanGraphs[sec][cent] = new TGraphErrors();
            sigmaGraphs[sec][cent] = new TGraphErrors();

            meanFits  [sec][cent] = new TF1("temp", "pol1", fitMin, fitMax );
            sigmaFits [sec][cent] = new TF1("temp", "pol1", fitMin, fitMax );

            meanPoints [sec][cent] = 0;
            sigmaPoints[sec][cent] = 0;
        }
    }

    double pt, etaMean, etaMeanError, etaSigma, etaSigmaError;
    int cent, sec;

    while (!intxtfile.eof()) {
        
        intxtfile >> pt
                  >> cent 
                  >> sec 
                  >> etaMean 
                  >> etaMeanError
                  >> etaSigma
                  >> etaSigmaError;


        for (int iCent = 0; iCent < centClasses.size(); iCent++) {

            if (etaMeanError > maxMeanError) continue;
            if (etaMean > maxMean)           continue;

            if (cent == centClasses[iCent]) {

                sprintf(name, "etaMeansvsPt_cent%d_sec%d", cent, sec);
                meanGraphs[sec][iCent]->SetName(name);

                sprintf(name, "meanfit_cent%d_sec%d", cent, sec);
                meanFits[sec][iCent]->SetName(name);

                meanGraphs[sec][iCent]->GetYaxis()->SetRangeUser(0, 0.6);
                meanGraphs[sec][iCent]->SetPoint(meanPoints[sec][iCent], pt, etaMean);
                meanGraphs[sec][iCent]->SetPointError(meanPoints[sec][iCent], 0, etaMeanError);
                meanPoints[sec][iCent]++;
            }
        }

        for (int iCent = 0; iCent < centClasses.size(); iCent++) {

            if (etaSigmaError  > maxSigmaError) continue;
            if (fabs(etaSigma) > maxSigma)      continue;

            if (cent == centClasses[iCent]) {

                sprintf(name, "etaSigmasvsPt_cent%d_sec%d", cent, sec);
                sigmaGraphs[sec][iCent]->SetName(name);

                sprintf(name, "Sigmafit_cent%d_sec%d", cent, sec);
                sigmaFits[sec][iCent]->SetName(name);

                etaSigma = fabs(etaSigma);

                sigmaGraphs[sec][iCent]->GetYaxis()->SetRangeUser(0, 0.6);
                sigmaGraphs[sec][iCent]->SetPoint(sigmaPoints[sec][iCent], pt, etaSigma);
                sigmaGraphs[sec][iCent]->SetPointError(sigmaPoints[sec][iCent], 0, etaSigmaError);
                sigmaPoints[sec][iCent]++;
            }
        }
    }

    outfile->mkdir("meanFits");
    outfile->mkdir("sigmaFits");

    for (int sec = 0; sec < meanGraphs.size(); sec++) {
        for (int cent = 0; cent < meanGraphs[sec].size(); cent++) {
                
            meanGraphs [sec][cent]->Fit(meanFits [sec][cent], "RW");
            sigmaGraphs[sec][cent]->Fit(sigmaFits[sec][cent], "RW");

            meansOutFile << std::left << std::setw(15) << std::setprecision(7) << centClasses[cent] 
                         << std::left << std::setw(15) << std::setprecision(7) << sec
                         << std::left << std::setw(15) << std::setprecision(7) << meanFits[sec][cent]->GetParameter(0) 
                         << std::left << std::setw(15) << std::setprecision(7) << meanFits[sec][cent]->GetParameter(1) << std::endl;

            sigmasOutFile << std::left << std::setw(15) << std::setprecision(7) << centClasses[cent] 
                          << std::left << std::setw(15) << std::setprecision(7) << sec
                          << std::left << std::setw(15) << std::setprecision(7) << sigmaFits[sec][cent]->GetParameter(0) 
                          << std::left << std::setw(15) << std::setprecision(7) << sigmaFits[sec][cent]->GetParameter(1) << std::endl;

            outfile->cd("meanFits");
            meanGraphs[sec][cent]->Write();
            outfile->cd();
            
            outfile->cd("sigmaFits");
            sigmaGraphs[sec][cent]->Write();
            outfile->cd();

        }
    }

    sigmasOutFile.close();
    meansOutFile.close();
    outfile->Close();

    return 0;
}
