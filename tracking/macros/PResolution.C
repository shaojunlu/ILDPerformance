#include "TH1F.h"
#include "TF1.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TTree.h"
#include "TFile.h"
#include "iostream"
#include "vector"
#include "TLegend.h"
#include "TGraphErrors.h"
#include <math.h>  
#include "TLegend.h"
using namespace std;

#define SIZE_M 8
#define SIZE_PA 4

double sigma[SIZE_M][SIZE_PA], error_sigma[SIZE_M][SIZE_PA];

int Mom[SIZE_M] = {1, 3, 5, 10, 15, 25, 50, 100};
float Momentum[SIZE_M] = {1, 3, 5, 10, 15, 25, 50, 100};
float zeros[SIZE_M];
int PA[SIZE_PA] = {10, 20, 40, 85};

float LimAxis;
int color, marker;

void PResolution(){
  for (int ll = 0; ll < SIZE_M; ll++){  
    zeros[ll] = 0;
  }
  for(int ii = 0; ii < SIZE_PA; ii++){
    for (int i = 0; i < SIZE_M; i++){
      TFile *f = new TFile(Form("../Results/Analysis/analysis_MuonsAngle_%d_Mom_%d.root",PA[ii],Mom[i]), "read");
      //TTree *EvalTree = (TTree*)f->Get("EvalTree");
        TTree *EvalTree;
        f->GetObject("EvalTree", EvalTree);

	printf( " I am studying file analysis_MuonsAngle_%d_Mom_%d.root ",PA[ii],Mom[i]);

	vector<float> *PtReco = 0;
	vector<float> *PtMCP = 0; 

	EvalTree->SetBranchAddress("PtReco", &PtReco);
	EvalTree->SetBranchAddress("PtMCP", &PtMCP);
	
	EvalTree->SetBranchStatus("*",0); //disable all branches
	EvalTree->SetBranchStatus("PtReco",1);
	EvalTree->SetBranchStatus("PtMCP",1);

	
	//getting the correct size of the axis for a good fitting
        if (PA[ii] == 85){ //axis for polar angle 85
	  if ( (Mom[i]/1.0) > 100){
	    LimAxis = 1/(10*Mom[i]/1.0);
	  }
	  else{
	    LimAxis = 1/(100*Mom[i]/1.0);
	  }
	}
        else if (PA[ii] == 40){ //axis for polar angle 40
	  if ( (Mom[i]/1.0) > 100  && Mom[i] != 300 && Mom[i] != 500){ //I need alternative axis for 300 and 500 GeV.
	    LimAxis = 1/(10*Mom[i]/1.0);
	  }
          else if (Mom[i] == 300 || Mom[i] == 500){
	    LimAxis = 0.0015;
	  }
	  else{
	    LimAxis = 1/(100*Mom[i]/1.0);
	  }
	}      
        else if (PA[ii] == 20 ){ //axis for polar angle 20
	  if ( Mom[i] == 1 ){ LimAxis = 0.3; }
	  else{
	    LimAxis = 0.01;
	  }
	}
        else if (PA[ii] == 20 ){
	  LimAxis = 0.04;
	}

	TH1F* h1 = new TH1F("h1","Momentum Resolution",100, (-1)*LimAxis, LimAxis); 
	
	Long64_t nentries = EvalTree->GetEntriesFast(); 
	for (Long64_t jentry=0; jentry<nentries;jentry++) {
	  EvalTree->GetEntry(jentry);
	  for (int unsigned k = 0; k < PtReco->size(); k++){
	    float MomentumResolution = (PtReco->at(k) - PtMCP->at(k))/pow(PtMCP->at(k), 2);
	    h1->Fill( MomentumResolution);
	  }
	}
	
	h1->Fit("gaus");
	TF1 *fit1 = (TF1*)h1->GetFunction("gaus");
	sigma[i][ii] = fit1->GetParameter(2);
	error_sigma[i][ii] =  fit1->GetParError(2);

	//std::cout << " sigma " << sigma[i][ii] <<std::endl ;


    }//loop for each .root files, here I have collected sigma and its error
  }//polar angle loop
  
 float sigma20[SIZE_M];
 float error20[SIZE_M];
 float r_error20[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma20[kk] = sigma[kk][1];
    error20[kk] = error_sigma[kk][1];
    r_error20[kk] = 100*(error_sigma[kk][1]/sigma[kk][1]);
  }  

  float sigma10[SIZE_M];
  float error10[SIZE_M];
  float r_error10[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma10[kk] = sigma[kk][0];
    error10[kk] = error_sigma[kk][0];
  }
  
  float sigma40[SIZE_M];
  float error40[SIZE_M];
  float r_error40[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma40[kk] = sigma[kk][2];
    error40[kk] = error_sigma[kk][2];
    r_error40[kk] = 100*(error_sigma[kk][2]/sigma[kk][2]);
  }

  float sigma85[SIZE_M];
  float error85[SIZE_M];
  float r_error85[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma85[kk] = sigma[kk][3];
    error85[kk] = error_sigma[kk][3];
    r_error85[kk] = 100*(error_sigma[kk][3]/sigma[kk][3]);
    std::cout << " polar angle = 85, momentum " << Momentum[kk] << " kk " << kk << " sigma85 " << sigma85[kk] << std::endl ;

  }

 


  TCanvas *c_two = new TCanvas("MomentumResolutionBar","Momentum Resolution",200,10,700,500);
  //c_two->Divide(1,2);
  //c_two->cd(1);
  //TGraph *Muon_plot20 = new TGraph(SIZE_M, Momentum, sigma20);
  
  TGraphErrors* Muon_plot20  = new TGraphErrors(SIZE_M, Momentum, sigma20, zeros, error20);
  Muon_plot20 -> SetTitle("Momentum Resolution");
  Muon_plot20 -> SetMarkerColor(4);
  Muon_plot20 -> SetMarkerStyle(22);
  Muon_plot20 -> SetMarkerSize(1);
  Muon_plot20 -> GetXaxis() -> SetTitle("P (GeV)");
  Muon_plot20 -> GetYaxis() -> SetTitle("#sigma_{1/p_{T}}(GeV^{-1})");
  Muon_plot20 -> SetMinimum( pow(10,-5) );
  Muon_plot20 -> SetMaximum( 2*pow(10, -1) );
  Muon_plot20 -> Draw("AP");
  
  TGraphErrors *Muon_plot40 = new TGraphErrors(SIZE_M, Momentum, sigma40,  zeros, error40);
  Muon_plot40 -> SetTitle("Momentum Resolution");
  Muon_plot40 -> SetMarkerColor(2);
  Muon_plot40 -> SetMarkerStyle(21);
  Muon_plot40 -> SetMarkerSize(1);
  //Muon_plot40 -> GetXaxis() -> SetTitle("P (GeV)");
  //Muon_plot40 -> GetYaxis() -> SetTitle("#sigma_{1/p_{T}}(GeV^{-1})");
  //Muon_plot40 -> SetMinimum( pow(10,-5) );
  //Muon_plot40 -> SetMaximum( 2*pow(10, -1) );
  Muon_plot40 -> Draw("P");

  TGraphErrors *Muon_plot10 = new TGraphErrors(SIZE_M, Momentum, sigma10,  zeros, error10);
  Muon_plot10 -> SetTitle("Momentum Resolution");
  Muon_plot10 -> SetMarkerColor(6);
  Muon_plot10 -> SetMarkerStyle(20);
  Muon_plot10 -> SetMarkerSize(1);
  Muon_plot10 -> Draw("P");

  TGraphErrors *Muon_plot85 = new TGraphErrors(SIZE_M, Momentum, sigma85,  zeros, error85);
  Muon_plot85 -> SetTitle("Momentum Resolution");
  Muon_plot85 -> SetMarkerColor(1);
  Muon_plot85 -> SetMarkerStyle(20);
  Muon_plot85 -> SetMarkerSize(1);
  Muon_plot85 -> Draw("P");
  c_two->SetLogx();
  c_two->SetLogy();
  c_two->SetTickx(1);
  c_two->SetTicky(1);

  TLegend *leg = new TLegend(0.6,0.7,0.75,0.95);
  //leg->SetHeader("Polar Angles"); //name of the legend
  leg->AddEntry(Muon_plot10,"#theta = 10^{o}","p");
  leg->AddEntry(Muon_plot20,"#theta = 20^{o}","p");
  leg->AddEntry(Muon_plot40,"#theta = 40^{o}","p");
  leg->AddEntry(Muon_plot85,"#theta = 85^{o}","p");
  leg->Draw();
 
}
