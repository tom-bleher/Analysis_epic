#ifndef CAL_CC
#define CAL_CC

#include "cal.h"

// Constructor
//-------------------------------------------------------------------------
CALAnalysis::CALAnalysis() {

  LoadCalibration();

}

// The data structures from the simulation have to be passed for each event
//-------------------------------------------------------------------------
void CALAnalysis::Prepare(std::vector<const edm4hep::SimCalorimeterHit*> &CalHits, 
      std::vector<const edm4hep::RawCalorimeterHit*> &CALadc, 
      std::vector<const edm4eic::CalorimeterHit*> &CALrecHits,
      std::vector<const edm4eic::ProtoCluster*> &CALprotoClusters,
      std::vector<const edm4eic::Cluster*> &CALClusters,
      std::vector<const edm4hep::SimTrackerHit*> &tracker_hits,
      std::shared_ptr<DD4hep_service> geoSvc ) {

  m_CALhits = CalHits;
  m_CALadc = CALadc;
  m_CALrecHits = CALrecHits;
  m_CALprotoClusters = CALprotoClusters;
  m_CALclusters = CALClusters;
  m_edm4hepTrackerHits = tracker_hits;
  m_geoSvc = geoSvc;

  m_GoodClusters.clear();

  m_EtopTotal = 0.0;
  m_EbotTotal = 0.0;
  for( auto cluster : m_CALclusters ) {
    edm4hep::Vector3f vec = cluster->getPosition();// mm

    if( cluster->getNhits() < variables::Nhits_min ) { continue; }
    
    if( vec.y > variables::LumiSpecCAL_FiveSigma ) {
      m_EtopTotal = cluster->getEnergy();
    }
    if( vec.y < -variables::LumiSpecCAL_FiveSigma ) {
      m_EbotTotal = cluster->getEnergy();
    }
  }

  m_E_CALhits_total = 0.0;
  m_E_CALtophits_total = 0.0;
  m_E_CALbothits_total = 0.0;
  m_CALtophits_total = 0.0;
  m_CALbothits_total = 0.0;

}

//-------------------------------------------------------------------------
void CALAnalysis::LoadCalibration() {

  // Load the calibration matrix from the utilities directory
  // Check that it exists.  If not there, issue a warning or abort the code
  // m_calibration = ...

}

// //-------------------------------------------------------------------------
// void CALAnalysis::FillTrees() {

//   ///////////////////////////////////////////////////////////////////////
//   // CAL G4 Hits Tree
//   map<string, int> CALfield_idx_Map{ {"sector", 0}, {"module", 0}, {"fiber_x", 0}, {"fiber_y", 0}};

//   for( auto hit : m_CALhits ) {

//     const auto id = hit->getCellID();
//     auto id_dec   = m_geoSvc->detector()->readout( "LumiSpecCALHits" ).idSpec().decoder();

//     // field of readout fields
//     vector<dd4hep::BitFieldElement> hitFields = id_dec->fields();

//     // try to find the expected fields and store field index
//     for( auto field : hitFields ) {
//       if( CALfield_idx_Map.find( field.name() ) != CALfield_idx_Map.end() ) {
//         CALfield_idx_Map[ field.name() ] = id_dec->index( field.name() );
//       }
//     }

//     // look up sector,module,fiber... id of this hit 
//     variables::sec_id 	= (int) id_dec->get( id, CALfield_idx_Map["sector"] ); // Top (0) and Bottom (1)
//     variables::mod_id 	= (int) id_dec->get( id, CALfield_idx_Map["module"] ); // 10x10 Matrix of bars
//     variables::fiber_x_id  = (int) id_dec->get( id, CALfield_idx_Map["fiber_x"] );
//     variables::fiber_y_id  = (int) id_dec->get( id, CALfield_idx_Map["fiber_y"] );

//     edm4hep::Vector3f vec = hit->getPosition();// mm
//     variables::E_hit = hit->getEnergy();
//     variables::x_hit = vec.x;
//     variables::y_hit = vec.y;
//     variables::r_hit = sqrt( pow(variables::x_hit, 2) + pow(variables::y_hit, 2) );

//     treeCAL_Hits->Fill();
//   }

//   ///////////////////////////////////////////////////////////////////////
//   // CAL Rec Hits
//   for( auto hit : m_CALrecHits ) {
//     edm4hep::Vector3f vec = hit->getPosition();// mm
//     variables::E_hit = hit->getEnergy();
//     variables::x_hit = vec.x;
//     variables::y_hit = vec.y;
//     variables::r_hit = sqrt( pow(variables::x_hit, 2) + pow(variables::y_hit, 2) );
//     variables::t_hit = hit->getTime();

//     treeCAL_RecHits->Fill();
//   }

//   ///////////////////////////////////////////////////////////////////////
//   // CAL Clusters
//   for( auto cluster : m_CALclusters ) {
//     edm4hep::Vector3f vec = cluster->getPosition();// mm
//     variables::Nhits_cluster = cluster->getNhits();
//     variables::E_cluster = cluster->getEnergy();
//     variables::x_cluster = vec.x;
//     variables::y_cluster = vec.y;
//     variables::r_cluster = sqrt( pow(vec.x, 2) + pow(vec.y, 2) );
//     variables::t_cluster = cluster->getTime();
//     if( cluster->shapeParameters_size() > 0 ) {
//       variables::Radius_cluster = cluster->getShapeParameters(0);
//       variables::Dispersion_cluster = cluster->getShapeParameters(1);
//       variables::SigmaThetaPhi1_cluster = cluster->getShapeParameters(2);
//       variables::SigmaThetaPhi2_cluster = cluster->getShapeParameters(3);
//     } else {// TODO: find out why this case happens
//       variables::Radius_cluster = 0;
//       variables::Dispersion_cluster = 0;
//       variables::SigmaThetaPhi1_cluster = 0;
//       variables::SigmaThetaPhi2_cluster = 0;
//     }
   
//     treeCAL_Clusters->Fill();
//   }

// }

//-------------------------------------------------------------------------
void CALAnalysis::FillDiagnostics() {

  xhit_TopTrack2 = 0.0;
  yhit_TopTrack2 = 0.0;
  xhit_BotTrack2 = 0.0;
  yhit_TopTrack2 = 0.0;
  for( auto tracker_hit : m_edm4hepTrackerHits ){
    // Check the tracker hits and assign some variables for use later
    if ( tracker_hit->y() > 0 && tracker_hit->z() < -63988){ // Hit in top tracker, 2nd layer
      xhit_TopTrack2=tracker_hit->x();
      yhit_TopTrack2=tracker_hit->y();   
    }
    else if ( tracker_hit->y() <0 && tracker_hit->z() < -63988){ // Hit in bottom tracker, 2nd layer
      xhit_BotTrack2=tracker_hit->x();
      yhit_BotTrack2=tracker_hit->y();   
    }    
  }

  map<string, int> CALfield_idx_Map{ {"sector", 0}, {"module", 0}, {"fiber_x", 0}, {"fiber_y", 0}};
  // Reset histograms before looping over hits for new event
  ((TH2D *)gHistList->FindObject("h2ZXETop_1mm_Event"))->Reset();
  ((TH2D *)gHistList->FindObject("h2ZXETop_3mm_Event"))->Reset();
  ((TH2D *)gHistList->FindObject("h2ZXETop_9mm_Event"))->Reset();  
  ((TH2D *)gHistList->FindObject("h2ZXEBot_1mm_Event"))->Reset();
  ((TH2D *)gHistList->FindObject("h2ZXEBot_3mm_Event"))->Reset();
  ((TH2D *)gHistList->FindObject("h2ZXEBot_9mm_Event"))->Reset();  
  ((TH2D *)gHistList->FindObject("h2ZYETop_1mm_Event"))->Reset();
  ((TH2D *)gHistList->FindObject("h2ZYETop_3mm_Event"))->Reset();
  ((TH2D *)gHistList->FindObject("h2ZYETop_9mm_Event"))->Reset();  
  ((TH2D *)gHistList->FindObject("h2ZYEBot_1mm_Event"))->Reset();
  ((TH2D *)gHistList->FindObject("h2ZYEBot_3mm_Event"))->Reset();
  ((TH2D *)gHistList->FindObject("h2ZYEBot_9mm_Event"))->Reset();  

  for( auto hit : m_CALhits ) { 
    edm4hep::Vector3f vec = hit->getPosition();// mm
    // 17/01/24 - This is duplicated from above to get the module ID, slightly redundant? Works though
    auto id_dec   = m_geoSvc->detector()->readout( "LumiSpecCALHits" ).idSpec().decoder();
    vector<dd4hep::BitFieldElement> hitFields = id_dec->fields();
    // try to find the expected fields and store field index
    for( auto field : hitFields ) {
      if( CALfield_idx_Map.find( field.name() ) != CALfield_idx_Map.end() ) {
        CALfield_idx_Map[ field.name() ] = id_dec->index( field.name() );
      }
    }
        
    variables::mod_id = (int) id_dec->get(hit->getCellID(), CALfield_idx_Map["module"] );

    m_E_CALhits_total += hit->getEnergy();
    
    ((TH1D *)gHistList->FindObject("h1Eraw"))->Fill( hit->getEnergy() );
    ((TH2D *)gHistList->FindObject("h2ZE"))->Fill( vec.z, hit->getEnergy() );
    ((TH2D *)gHistList->FindObject("h2Zr"))->Fill( vec.z,  sqrt( pow(vec.x, 2) + pow(vec.y, 2) ));
    if (vec.y > 0 && hit->getEnergy() > 0.0001){ // Add condition that hit not look like noise
      m_E_CALtophits_total += hit->getEnergy();
      m_CALtophits_total++;
      if (variables::mod_id % 6 > 2 ){ // Modules 0-2 are x orientated (y info), 3-5 y orientated (x info), so modulo6 of the ID is >2 for x info (6-8 x, 9-11 y...)
	((TH1D *)gHistList->FindObject("h1XTop"))->Fill( vec.x );
	((TH2D *)gHistList->FindObject("h2ZXETop_1mm"))->Fill( vec.z, vec.x, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZXETop_3mm"))->Fill( vec.z, vec.x, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZXETop_9mm"))->Fill( vec.z, vec.x, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZXETop_1mm_Event"))->Fill( vec.z, vec.x, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZXETop_3mm_Event"))->Fill( vec.z, vec.x, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZXETop_9mm_Event"))->Fill( vec.z, vec.x, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZdXTop"))->Fill( vec.z, vec.x-variables::Xpositron, hit->getEnergy() );
      }
      if (variables::mod_id % 6 < 3 ){ // Modules 0-2 are x orientated (y info), 3-5 y orientated (x info), so modulo6 of the ID is < 3 for y info (6-8 x, 9-11 y...)
	((TH1D *)gHistList->FindObject("h1YTop"))->Fill( vec.y );
	((TH2D *)gHistList->FindObject("h2ZYETop_1mm"))->Fill( vec.z, vec.y, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZYETop_3mm"))->Fill( vec.z, vec.y, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZYETop_9mm"))->Fill( vec.z, vec.y, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZYETop_1mm_Event"))->Fill( vec.z, vec.y, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZYETop_3mm_Event"))->Fill( vec.z, vec.y, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZYETop_9mm_Event"))->Fill( vec.z, vec.y, hit->getEnergy() );
      }
      ((TH2D *)gHistList->FindObject("h2ZETop"))->Fill( vec.z, hit->getEnergy() );
    }
    else if ( vec.y < 0  && hit->getEnergy() > 0.0001){
      m_E_CALbothits_total += hit->getEnergy();
      m_CALbothits_total++;
      if (variables::mod_id % 6 > 2 ){
	((TH1D *)gHistList->FindObject("h1XBot"))->Fill( vec.x );
	((TH2D *)gHistList->FindObject("h2ZXEBot_1mm"))->Fill( vec.z, vec.x, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZXEBot_3mm"))->Fill( vec.z, vec.x, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZXEBot_9mm"))->Fill( vec.z, vec.x, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZXEBot_1mm_Event"))->Fill( vec.z, vec.x, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZXEBot_3mm_Event"))->Fill( vec.z, vec.x, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZXEBot_9mm_Event"))->Fill( vec.z, vec.x, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZdXBot"))->Fill( vec.z, vec.x-variables::Xelectron, hit->getEnergy() );
      }
      if (variables::mod_id % 6 < 3 ){ 
	((TH1D *)gHistList->FindObject("h1YBot"))->Fill( vec.y );
	((TH2D *)gHistList->FindObject("h2ZYEBot_1mm"))->Fill( vec.z, vec.y, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZYEBot_3mm"))->Fill( vec.z, vec.y, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZYEBot_9mm"))->Fill( vec.z, vec.y, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZYEBot_1mm_Event"))->Fill( vec.z, vec.y, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZYEBot_3mm_Event"))->Fill( vec.z, vec.y, hit->getEnergy() );
	((TH2D *)gHistList->FindObject("h2ZYEBot_9mm_Event"))->Fill( vec.z, vec.y, hit->getEnergy() );
      }
      ((TH2D *)gHistList->FindObject("h2ZEBot"))->Fill( vec.z, hit->getEnergy() );
    }
  } // Loop over hits for event done, use only total quantities now (not per hit info)

  ((TH1D *)gHistList->FindObject("h1ErawTotal"))->Fill(m_E_CALtophits_total + m_E_CALbothits_total);
  ((TH1D *)gHistList->FindObject("h1ErawTotalTop"))->Fill(m_E_CALtophits_total);
  ((TH1D *)gHistList->FindObject("h1ErawTotalBot"))->Fill(m_E_CALbothits_total);
  ((TH1D *)gHistList->FindObject("h1nHitsTop"))->Fill(m_CALtophits_total);
  ((TH1D *)gHistList->FindObject("h1nHitsBot"))->Fill(m_CALbothits_total);

  int fitStatus;
  double ypos_top = -10000;
  double ypos_bot = -10000;
  // Top detector hit filling condition
  if(m_CALtophits_total > 0 && m_E_CALtophits_total > 0.05){
    ((TH2D *)gHistList->FindObject("h2SampFracTop"))->Fill(variables::Epositron, (m_E_CALtophits_total/(variables::Epositron)));

    // Fit slices of h2ZXE
    ((TH2D *)gHistList->FindObject("h2ZXETop_1mm_Event"))->FitSlicesY(0,0,-1.0);
    TH1D* h2ZXETop_1mm_Event_1  = (TH1D*)gDirectory->Get("h2ZXETop_1mm_Event_1"); 
    posres_pol1->SetParameters(0,0);
    fitStatus = h2ZXETop_1mm_Event_1->Fit("posres_pol1","QN");
    if (fitStatus<2){
      double dx = posres_pol1->Eval(-63990) - xhit_TopTrack2; // Subtract tracker hit value
      ((TH2D *)gHistList->FindObject("h2EdXTop_1mm"))->Fill(m_E_CALtophits_total, dx);
      ((TH2D *)gHistList->FindObject("h2EMCdXTop_1mm"))->Fill(variables::Epositron, dx);
    }   
    ((TH2D *)gHistList->FindObject("h2ZXETop_3mm_Event"))->FitSlicesY(0,0,-1.0);
    TH1D* h2ZXETop_3mm_Event_1  = (TH1D*)gDirectory->Get("h2ZXETop_3mm_Event_1"); 
    posres_pol1->SetParameters(0,0);
    fitStatus = h2ZXETop_3mm_Event_1->Fit("posres_pol1","QN");
    if (fitStatus<2){
      double dx = posres_pol1->Eval(-63990) - xhit_TopTrack2; // Subtract tracker hit value
      ((TH2D *)gHistList->FindObject("h2EdXTop_3mm"))->Fill(m_E_CALtophits_total, dx);
      ((TH2D *)gHistList->FindObject("h2EMCdXTop_3mm"))->Fill(variables::Epositron, dx);
    }   
    ((TH2D *)gHistList->FindObject("h2ZXETop_9mm_Event"))->FitSlicesY(0,0,-1.0);
    TH1D* h2ZXETop_9mm_Event_1  = (TH1D*)gDirectory->Get("h2ZXETop_9mm_Event_1"); 
    posres_pol1->SetParameters(0,0);
    fitStatus = h2ZXETop_9mm_Event_1->Fit("posres_pol1","QN");
    if (fitStatus<2){
      double dx = posres_pol1->Eval(-63990) - xhit_TopTrack2; // Subtract tracker hit value
      ((TH2D *)gHistList->FindObject("h2EdXTop_9mm"))->Fill(m_E_CALtophits_total, dx);
      ((TH2D *)gHistList->FindObject("h2EMCdXTop_9mm"))->Fill(variables::Epositron, dx);
    } 
    // Fit slices of h2ZYE
    ((TH2D *)gHistList->FindObject("h2ZYETop_1mm_Event"))->FitSlicesY(0,0,-1.0);
    TH1D* h2ZYETop_1mm_Event_1  = (TH1D*)gDirectory->Get("h2ZYETop_1mm_Event_1"); 
    posres_pol1->SetParameters(0,0);
    fitStatus = h2ZYETop_1mm_Event_1->Fit("posres_pol1","QN");
    if (fitStatus<2){
      double dy = posres_pol1->Eval(-63990) - yhit_TopTrack2; // Subtract tracker hit value
      ((TH2D *)gHistList->FindObject("h2EdYTop_1mm"))->Fill(m_E_CALtophits_total, dy);
      ((TH2D *)gHistList->FindObject("h2EMCdYTop_1mm"))->Fill(variables::Epositron, dy);
    }   
    ((TH2D *)gHistList->FindObject("h2ZYETop_3mm_Event"))->FitSlicesY(0,0,-1.0);
    TH1D* h2ZYETop_3mm_Event_1  = (TH1D*)gDirectory->Get("h2ZYETop_3mm_Event_1"); 
    posres_pol1->SetParameters(0,0);
    fitStatus = h2ZYETop_3mm_Event_1->Fit("posres_pol1","QN");
    if (fitStatus<2){
      double dy = posres_pol1->Eval(-63990) - yhit_TopTrack2; // Subtract tracker hit value
      ypos_top = (posres_pol1->Eval(-64000))/10; // Evaluate y hit position of event on calorimeter face
      ((TH2D *)gHistList->FindObject("h2EdYTop_3mm"))->Fill(m_E_CALtophits_total, dy);
      ((TH2D *)gHistList->FindObject("h2EMCdYTop_3mm"))->Fill(variables::Epositron, dy);
    }   
    ((TH2D *)gHistList->FindObject("h2ZYETop_9mm_Event"))->FitSlicesY(0,0,-1.0);
    TH1D* h2ZYETop_9mm_Event_1  = (TH1D*)gDirectory->Get("h2ZYETop_9mm_Event_1"); 
    posres_pol1->SetParameters(0,0);
    fitStatus = h2ZYETop_9mm_Event_1->Fit("posres_pol1","QN");
    if (fitStatus<2){
      double dy = posres_pol1->Eval(-63990) - yhit_TopTrack2; // Subtract tracker hit value
      ((TH2D *)gHistList->FindObject("h2EdYTop_9mm"))->Fill(m_E_CALtophits_total, dy);
      ((TH2D *)gHistList->FindObject("h2EMCdYTop_9mm"))->Fill(variables::Epositron, dy);
    }
    for(int i = 0; i < 5; i++){
      if ( ypos_top > (7+(i+1)) && ypos_top < (23-(i+1)) ){
	((TH1D *)gHistList->FindObject(Form("h1CALTopAccept_%i",(i+1))))->Fill(variables::Epositron);
	((TH2D *)gHistList->FindObject(Form("h2SampFracTop_%i", (i+1))))->Fill(variables::Epositron, (m_E_CALtophits_total/(variables::Epositron)));
      }
    } 
  }
  // Bot detector hit filling condition
  if(m_CALbothits_total > 0 && m_E_CALbothits_total > 0.05){
    ((TH2D *)gHistList->FindObject("h2SampFracBot"))->Fill(variables::Eelectron, (m_E_CALbothits_total/(variables::Eelectron)));
    // Fit slices of h2ZXE
    ((TH2D *)gHistList->FindObject("h2ZXEBot_1mm_Event"))->FitSlicesY(0,0,-1.0);
    TH1D* h2ZXEBot_1mm_Event_1  = (TH1D*)gDirectory->Get("h2ZXEBot_1mm_Event_1"); 
    posres_pol1->SetParameters(0,0);
    fitStatus = h2ZXEBot_1mm_Event_1->Fit("posres_pol1","QN");
    if (fitStatus<2){
      double dx = posres_pol1->Eval(-63990) - xhit_BotTrack2; // Subtract tracker hit value
      ((TH2D *)gHistList->FindObject("h2EdXBot_1mm"))->Fill(m_E_CALbothits_total, dx);
      ((TH2D *)gHistList->FindObject("h2EMCdXBot_1mm"))->Fill(variables::Eelectron, dx);
    }   
    ((TH2D *)gHistList->FindObject("h2ZXEBot_3mm_Event"))->FitSlicesY(0,0,-1.0);
    TH1D* h2ZXEBot_3mm_Event_1  = (TH1D*)gDirectory->Get("h2ZXEBot_3mm_Event_1"); 
    posres_pol1->SetParameters(0,0);
    fitStatus = h2ZXEBot_3mm_Event_1->Fit("posres_pol1","QN");
    if (fitStatus<2){
      double dx = posres_pol1->Eval(-63990) - xhit_BotTrack2; // Subtract tracker hit value
      ((TH2D *)gHistList->FindObject("h2EdXBot_3mm"))->Fill(m_E_CALbothits_total, dx);
      ((TH2D *)gHistList->FindObject("h2EMCdXBot_3mm"))->Fill(variables::Eelectron, dx);
    }   
    ((TH2D *)gHistList->FindObject("h2ZXEBot_9mm_Event"))->FitSlicesY(0,0,-1.0);
    TH1D* h2ZXEBot_9mm_Event_1  = (TH1D*)gDirectory->Get("h2ZXEBot_9mm_Event_1"); 
    posres_pol1->SetParameters(0,0);
    fitStatus = h2ZXEBot_9mm_Event_1->Fit("posres_pol1","QN");
    if (fitStatus<2){
      double dx = posres_pol1->Eval(-63990) - xhit_BotTrack2; // Subtract tracker hit value
      ((TH2D *)gHistList->FindObject("h2EdXBot_9mm"))->Fill(m_E_CALbothits_total, dx);
      ((TH2D *)gHistList->FindObject("h2EMCdXBot_9mm"))->Fill(variables::Eelectron, dx);
    } 
    // Fit slices of h2ZYE
    ((TH2D *)gHistList->FindObject("h2ZYEBot_1mm_Event"))->FitSlicesY(0,0,-1.0);
    TH1D* h2ZYEBot_1mm_Event_1  = (TH1D*)gDirectory->Get("h2ZYEBot_1mm_Event_1"); 
    posres_pol1->SetParameters(0,0);
    fitStatus = h2ZYEBot_1mm_Event_1->Fit("posres_pol1","QN");
    if (fitStatus<2){
      double dy = posres_pol1->Eval(-63990) - yhit_BotTrack2; // Subtract tracker hit value
      ((TH2D *)gHistList->FindObject("h2EdYBot_1mm"))->Fill(m_E_CALbothits_total, dy);
      ((TH2D *)gHistList->FindObject("h2EMCdYBot_1mm"))->Fill(variables::Eelectron, dy);
    }   
    ((TH2D *)gHistList->FindObject("h2ZYEBot_3mm_Event"))->FitSlicesY(0,0,-1.0);
    TH1D* h2ZYEBot_3mm_Event_1  = (TH1D*)gDirectory->Get("h2ZYEBot_3mm_Event_1"); 
    posres_pol1->SetParameters(0,0);
    fitStatus = h2ZYEBot_3mm_Event_1->Fit("posres_pol1","QN");
    if (fitStatus<2){
      double dy = posres_pol1->Eval(-63990) - yhit_BotTrack2; // Subtract tracker hit value
      ypos_bot = (posres_pol1->Eval(-64000))/10; // Evaluate y hit position of event on calorimeter face
      ((TH2D *)gHistList->FindObject("h2EdYBot_3mm"))->Fill(m_E_CALbothits_total, dy);
      ((TH2D *)gHistList->FindObject("h2EMCdYBot_3mm"))->Fill(variables::Eelectron, dy);
    }   
    ((TH2D *)gHistList->FindObject("h2ZYEBot_9mm_Event"))->FitSlicesY(0,0,-1.0);
    TH1D* h2ZYEBot_9mm_Event_1  = (TH1D*)gDirectory->Get("h2ZYEBot_9mm_Event_1"); 
    posres_pol1->SetParameters(0,0);
    fitStatus = h2ZYEBot_9mm_Event_1->Fit("posres_pol1","QN");
    if (fitStatus<2){
      double dy = posres_pol1->Eval(-63990) - yhit_BotTrack2; // Subtract tracker hit value
      ((TH2D *)gHistList->FindObject("h2EdYBot_9mm"))->Fill(m_E_CALbothits_total, dy);
      ((TH2D *)gHistList->FindObject("h2EMCdYBot_9mm"))->Fill(variables::Eelectron, dy);
    } 
    for(int i = 0; i < 5; i++){
      if ( ypos_bot > (-23+(i+1)) && ypos_bot < (-7-(i+1)) ){
	((TH1D *)gHistList->FindObject(Form("h1CALBotAccept_%i",(i+1))))->Fill(variables::Eelectron);
	((TH2D *)gHistList->FindObject(Form("h2SampFracBot_%i", (i+1))))->Fill(variables::Eelectron, (m_E_CALbothits_total/(variables::Eelectron)));
      }
    }
  }
  // Coincidence filling condition
  if(m_CALtophits_total > 0 && m_CALbothits_total > 0 && m_E_CALtophits_total > 0.05 && m_E_CALbothits_total > 0.05){
    ((TH1D *)gHistList->FindObject("h1ErawTotalCoin"))->Fill(m_E_CALtophits_total + m_E_CALbothits_total);
    ((TH1D *)gHistList->FindObject("h1ErawTotalTop_Coin"))->Fill(m_E_CALtophits_total);
    ((TH1D *)gHistList->FindObject("h1ErawTotalBot_Coin"))->Fill(m_E_CALbothits_total);
    ((TH2D *)gHistList->FindObject("h2SampFracTop_Coin"))->Fill(variables::Epositron, (m_E_CALtophits_total/(variables::Epositron)));
    ((TH2D *)gHistList->FindObject("h2SampFracBot_Coin"))->Fill(variables::Eelectron, (m_E_CALbothits_total/(variables::Eelectron)));
    ((TH2D *)gHistList->FindObject("h2SampFracCoin"))->Fill(variables::Ephoton, ((m_E_CALtophits_total + m_E_CALbothits_total)/(variables::Ephoton)));
    for(int i = 0; i < 5; i++){
      if ( ypos_bot > (-23+(i+1)) && ypos_bot < (-7-(i+1)) && ypos_top > (7+(i+1)) && ypos_top < (23-(i+1)) ){
	((TH1D *)gHistList->FindObject(Form("h1CALCoinAccept_%i",(i+1))))->Fill(variables::Ephoton);
	((TH2D *)gHistList->FindObject(Form("h2SampFracCoin_%i", (i+1))))->Fill(variables::Ephoton, ((m_E_CALbothits_total+m_E_CALtophits_total)/(variables::Ephoton)));
      }
    }
  }
 
  for( auto adc : m_CALadc ) { ((TH1D *)gHistList->FindObject("hADCsignal"))->Fill( adc->getAmplitude() ); }
 
}

//-------------------------------------------------------------------------
void CALAnalysis::FillAcceptances() {

  // Top detector hit filling condition
  if(m_CALtophits_total > 0 && m_E_CALtophits_total > 0.05){
    ((TH1D *)gHistList->FindObject("h1CALTopAccept"))->Fill(variables::Epositron);
    ((TH2D *)gHistList->FindObject("h2CALTopAccept"))->Fill(variables::Epositron, m_E_CALtophits_total);
  }

  // Bot detector hit filling condition
  if(m_CALbothits_total > 0 && m_E_CALbothits_total > 0.05){
    ((TH1D *)gHistList->FindObject("h1CALBotAccept"))->Fill(variables::Eelectron);
    ((TH2D *)gHistList->FindObject("h2CALBotAccept"))->Fill(variables::Eelectron, m_E_CALbothits_total);
  }

  // Coincidence filling condition
  if(m_CALtophits_total > 0 && m_CALbothits_total > 0 && m_E_CALtophits_total > 0.05 && m_E_CALbothits_total > 0.05){
    ((TH1D *)gHistList->FindObject("h1CALCoinAccept"))->Fill(variables::Ephoton);
    ((TH2D *)gHistList->FindObject("h2CALCoinAccept"))->Fill(variables::Ephoton, m_E_CALtophits_total + m_E_CALbothits_total);
  }

}

//-------------------------------------------------------------------------
void CALAnalysis::CollectGoodClusters() {

  // Filter out unwanted low energy clusters
  // apply calibration correction
  // Put good clusters into a edm4eic::Cluster
  // m_GoodClusters.push_back( );

}

#endif

// 16/01/24 - Archived version
// //-------------------------------------------------------------------------
// void CALAnalysis::FillDiagnostics() {
 
//   double E_CALhits_total = 0.0;
//   double E_CALtophits_total = 0.0;
//   double E_CALbothits_total = 0.0;
//   double CALtophits_total = 0.0;
//   double CALbothits_total = 0.0;
  
//   for( auto hit : m_CALhits ) { 
//     edm4hep::Vector3f vec = hit->getPosition();// mm
//     E_CALhits_total += hit->getEnergy();
//     ((TH1D *)gHistList->FindObject("hEraw"))->Fill( hit->getEnergy() );
//     if (vec.y > 0){
//       E_CALtophits_total += hit->getEnergy();
//       CALtophits_total++;
//     }
//     else if ( vec.y < 0){
//       E_CALbothits_total += hit->getEnergy();
//       CALbothits_total++;
//     }
//   }
  
//   if( E_CALhits_total > 0 ) {
//     ((TH1D *)gHistList->FindObject("hErawTotal"))->Fill( E_CALhits_total ); 
//   }

//   if( E_CALtophits_total > 0 ){
//     // 16/01/24 - SJDK - Temporary measure for now whilst clustering not implemented, set m_EtopTotal (cluster energy) to sum of hits
//     m_EtopTotal = E_CALtophits_total;
//     ((TH1D *)gHistList->FindObject("hErawTotalTop"))->Fill( E_CALtophits_total );
//     ((TH2D *)gHistList->FindObject("hErawTotal_EMC_Top"))->Fill(variables::Eelectron, E_CALtophits_total ); 
//     ((TH2D *)gHistList->FindObject("hSampFracTopRaw"))->Fill( variables::Eelectron, ( E_CALtophits_total/variables::Eelectron ));
    
//   }

//   if( E_CALbothits_total > 0 ){
//     m_EbotTotal = E_CALbothits_total;
//     ((TH1D *)gHistList->FindObject("hErawTotalBot"))->Fill( E_CALbothits_total );
//     ((TH2D *)gHistList->FindObject("hErawTotal_EMC_Bot"))->Fill(variables::Epositron, E_CALbothits_total ); 
//     ((TH2D *)gHistList->FindObject("hSampFracBotRaw"))->Fill( variables::Epositron, ( E_CALbothits_total/variables::Epositron ));
//   }

//   if( E_CALtophits_total > 0 && E_CALbothits_total > 0 ){
//     ((TH2D *)gHistList->FindObject("hErawTotal_EMC_Coin"))->Fill(variables::Ephoton, E_CALhits_total );
//   }

//   ((TH1D *)gHistList->FindObject("hnHitsTop"))->Fill(CALtophits_total);
//   ((TH1D *)gHistList->FindObject("hnHitsBot"))->Fill(CALbothits_total);

//   for( auto adc : m_CALadc ) { ((TH1D *)gHistList->FindObject("hADCsignal"))->Fill( adc->getAmplitude() ); }
  
//   // SJDK - 13/06/23 - Title of this is a little misleading, this is Egamma compared to the TOTAL energy detected in both spectrometers, should make this clearer.
//   // SJDK - 16/01/23 - m_EtopTotal is based on *cluster energy, will be meaningless for now
//   ((TH2D *)gHistList->FindObject("hCALCluster_Eres"))->Fill( variables::Ephoton, m_EtopTotal + m_EbotTotal );
//   ((TH2D *)gHistList->FindObject("hCAL_Eres"))->Fill( variables::Ephoton, (variables::Ephoton - (m_EtopTotal + m_EbotTotal))/variables::Ephoton );  

//   ((TH1D *)gHistList->FindObject("hProtoClusterCount"))->Fill( m_CALprotoClusters.size() );
  
//   ((TH1D *)gHistList->FindObject("hClusterCount"))->Fill( m_CALclusters.size() );

//   ((TH2D *)gHistList->FindObject("hCALCluster_Eres"))->Fill( variables::Ephoton, m_EtopTotal + m_EbotTotal );

//   if( (m_EtopTotal + m_EbotTotal) > 0 ) { 
//     ((TH1D *)gHistList->FindObject("hEnergy"))->Fill( m_EtopTotal + m_EbotTotal );
//   }
  
//   if( m_EtopTotal > 0 ) { 
//     ((TH1D *)gHistList->FindObject("hEup"))->Fill( m_EtopTotal );
//     ((TH2D *)gHistList->FindObject("hCALTop_Eres"))->Fill( variables::Eelectron, (variables::Eelectron - (m_EtopTotal))/variables::Eelectron );
//     ((TH2D *)gHistList->FindObject("hSampFracTop"))->Fill( variables::Eelectron, ( m_EtopTotal/variables::Eelectron ));  
//   }

//   if( m_EbotTotal > 0 ) { 
//     ((TH1D *)gHistList->FindObject("hEdw"))->Fill( m_EbotTotal ); 
//     ((TH2D *)gHistList->FindObject("hCALBot_Eres"))->Fill( variables::Epositron, (variables::Epositron - (m_EbotTotal))/variables::Epositron );
//     ((TH2D *)gHistList->FindObject("hSampFracBot"))->Fill( variables::Epositron, ( m_EbotTotal/variables::Epositron ));
//   }

// }

// //-------------------------------------------------------------------------
// void CALAnalysis::FillAcceptances() {

//   // SJDK 14/06/23 - This acceptance definition is a bit odd, this is the detection of an electron or positron at the energy of the incoming gamma, add some new histograms
//   if( m_EtopTotal > 0 ) {
//     ((TH1D *)gHistList->FindObject("hCALTop_Acceptance"))->Fill( variables::Ephoton );
//   }
//   if( m_EbotTotal > 0 ) {
//     ((TH1D *)gHistList->FindObject("hCALBot_Acceptance"))->Fill( variables::Ephoton );
//   }
//   if( m_EtopTotal > 0 && m_EbotTotal > 0) { // Emin ~3.7 GeV
//     ((TH1D *)gHistList->FindObject("hCALCoincidence_Acceptance"))->Fill( variables::Ephoton );
//     ((TH2D *)gHistList->FindObject("hCALCoincidence_Acceptance_v2"))->Fill( (variables::Eelectron + variables::Epositron), (m_EtopTotal + m_EbotTotal)  );
//   }

//   // SJDK 14/06/23 - New acceptance histograms, individual calorimeter plots
//   if( m_EtopTotal > 0){
//     ((TH1D *)gHistList->FindObject("hCALTop_Acceptance_v2"))->Fill( variables::Eelectron );
//     ((TH2D *)gHistList->FindObject("hCALTop_Acceptance_v3"))->Fill( variables::Eelectron, m_EtopTotal );
//   }
//   if( m_EbotTotal > 0){
//     ((TH1D *)gHistList->FindObject("hCALBot_Acceptance_v2"))->Fill( variables::Epositron );
//     ((TH2D *)gHistList->FindObject("hCALBot_Acceptance_v3"))->Fill( variables::Epositron, m_EbotTotal );
//   }

//   // SJDK 22/06/23 - Some more new acceptance histograms, these are filled under the condition that you have something that vaguely looks correct compared to MC. The intention is to divide the Numer by the Denom histogram subsequently

//   // Fill numerator only if within +/- tolerance percentage of truth value (see constants.cc), fill denominator with all truth electrons
//   if ( ( m_EtopTotal < (variables::Eelectron + (variables::Eelectron * constants::Cal_Single_AcceptanceTol))) && ( m_EtopTotal > (variables::Eelectron - (variables::Eelectron * constants::Cal_Single_AcceptanceTol))) ){
//     ((TH1D *)gHistList->FindObject("hCALTop_Acceptance_v4_Numer"))->Fill( variables::Eelectron );
//   }
//   ((TH1D *)gHistList->FindObject("hCALTop_Acceptance_v4_Denom"))->Fill( variables::Eelectron );
  
//   if ( ( m_EbotTotal < (variables::Epositron + (variables::Epositron * constants::Cal_Single_AcceptanceTol))) && ( m_EbotTotal > (variables::Epositron - (variables::Epositron * constants::Cal_Single_AcceptanceTol))) ){
//     ((TH1D *)gHistList->FindObject("hCALBot_Acceptance_v4_Numer"))->Fill( variables::Epositron );
//   }
//   ((TH1D *)gHistList->FindObject("hCALBot_Acceptance_v4_Denom"))->Fill( variables::Epositron );

  
//   if ( ( (m_EbotTotal + m_EtopTotal) < (variables::Ephoton + (variables::Ephoton * constants::Cal_Coin_AcceptanceTol))) && ( (m_EbotTotal + m_EtopTotal) > (variables::Ephoton - (variables::Ephoton * constants::Cal_Coin_AcceptanceTol))) ){
//     ((TH1D *)gHistList->FindObject("hCALCoincidence_Acceptance_v3_Numer"))->Fill( variables::Ephoton );
//   }
//   ((TH1D *)gHistList->FindObject("hCALCoincidence_Acceptance_v3_Denom"))->Fill( variables::Ephoton );

// }
