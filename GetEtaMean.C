int GetEtaMean() {

    const double minPt = 4;
    const double maxPt = 7;

    const double bgIntegrationLowEdge = 0.3;
    const double bgIntegrationUpEdge = 0.4;

    const double massLowEdge = 0.3;
    const double massUpEdge = 0.9;
    
    TFile *infile  = new TFile("temp/CombinedCabanaBoy.root", "read");
    TFile *outfile = new TFile("temp/EtaPeakOut.root", "recreate");

    std::ofstream outtxtfile;
    outtxtfile.open("eta-means.txt");

    std::array <std::string, 4> dirs = { "c0-1_z0-2_r00","c2-3_z0-2_r00","c4-5_z0-2_r00", "c6-9_z0-2_r00" };

    std::string histname;

    char name[50];

    TH1D *massSpectrumFG = nullptr;
    TH1D *massSpectrumBG = nullptr;
    
    TH1D *etaPeak = nullptr;

    TF1 *fit = nullptr;

    const std::string temp1 = "/MassSectorPt_FG11";
    const std::string temp2 = "/MassSectorPt_BG11";

    int cent = 0;

    for ( auto &dir : dirs ) {

        histname = dir + temp1;
        TH3D *massHistFG = infile->Get<TH3D>( histname.c_str() );

        histname = dir + temp2;
        TH3D *massHistBG = infile->Get<TH3D>( histname.c_str() );
        
        massHistFG->GetXaxis()->SetRangeUser(massLowEdge, massUpEdge);
        massHistBG->GetXaxis()->SetRangeUser(massLowEdge, massUpEdge);

        const int pTMinBin = massHistFG->GetZaxis()->FindBin(minPt);
        const int pTMaxBin = massHistBG->GetZaxis()->FindBin(maxPt);

        const double bgIntegrationMinBin = massHistFG->GetXaxis()->FindBin(bgIntegrationLowEdge);
        const double bgIntegrationMaxBin = massHistFG->GetXaxis()->FindBin(bgIntegrationUpEdge);

        for (int sec = 0; sec < 8; sec++) {

            for (int ptBin = pTMinBin; ptBin < pTMaxBin; ptBin++) {

                sprintf(name, "massHistFG_pt%f_cent%d_sec%d", massHistFG->GetZaxis()->GetBinLowEdge(ptBin), cent, sec);
                massSpectrumFG = massHistFG->ProjectionX(name, sec, sec, ptBin, ptBin);

                sprintf(name, "massHistBG_pt%f_cent%d_sec%d", massHistBG->GetZaxis()->GetBinLowEdge(ptBin), cent, sec);
                massSpectrumBG = massHistBG->ProjectionX(name, sec, sec, ptBin, ptBin);

                const double scalingFactor = massSpectrumFG->Integral(bgIntegrationMinBin, bgIntegrationMaxBin) /
                                             massSpectrumBG->Integral(bgIntegrationMinBin, bgIntegrationMaxBin) ;


                cout << "INTEGRAL 1: " << massSpectrumFG->Integral(bgIntegrationMinBin, bgIntegrationMaxBin) << endl;
                cout << "INTEGRAL 2: " << massSpectrumBG->Integral(bgIntegrationMinBin, bgIntegrationMaxBin) << endl;

                cout << scalingFactor << endl;

                massSpectrumBG->Scale( scalingFactor );
                
                sprintf(name, "etaPeak_pt%f_cent%d_sec%d", massSpectrumFG->GetZaxis()->GetBinLowEdge(ptBin), cent, sec);
                etaPeak = (TH1D*) massSpectrumFG->Clone(name);

                etaPeak->Add(massSpectrumBG, -1);

                sprintf(name, "fit__pt%f_cent%d_sec%d", massSpectrumFG->GetZaxis()->GetBinLowEdge(ptBin), cent, sec);
                fit = new TF1(name, "gaus(0) + pol2(3)", massLowEdge, massUpEdge); 
                fit->SetParameters(2000, 0.547, 0.1, 0, 0, 0);
                etaPeak->Fit(fit, "RQ");

                outfile->mkdir(dir.c_str());
                outfile->cd(dir.c_str());

                massSpectrumFG->Write();
                massSpectrumBG->Write();
                etaPeak   ->Write();

                outfile->cd();

                /*
                delete massSpectrumFG;
                delete massSpectrumBG;
                delete massHistFG;
                delete massHistBG;
                delete etaPeak;
                delete fit;
                */
            }
        }

        cent = cent + 20;
    }

    infile ->Close();
    outfile->Close();

    return 0;
}
