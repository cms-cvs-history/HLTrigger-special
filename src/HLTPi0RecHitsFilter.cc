#include "HLTrigger/special/interface/HLTPi0RecHitsFilter.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/Math/interface/Point3D.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"


//Level 1 Trigger
#include "DataFormats/L1Trigger/interface/L1EmParticle.h"
#include "DataFormats/L1Trigger/interface/L1EmParticleFwd.h"
#include "CondFormats/L1TObjects/interface/L1CaloGeometry.h"
#include "CondFormats/DataRecord/interface/L1CaloGeometryRecord.h"
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

/// EgammaCoreTools
#include "RecoEcal/EgammaCoreTools/interface/PositionCalc.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalEtaPhiRegion.h"


// Ecal Mapping 
#include "DataFormats/EcalRawData/interface/EcalListOfFEDS.h"
#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "Geometry/EcalMapping/interface/EcalMappingRcd.h"
#include <DataFormats/FEDRawData/interface/FEDNumbering.h>

// Jets stuff
#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"

//// Ecal Electrons Id
#include "DataFormats/EcalDetId/interface/EcalElectronicsId.h"
#include "DataFormats/EcalDetId/interface/EcalTriggerElectronicsId.h"




#include "TVector3.h"

#define TWOPI 6.283185308

using namespace l1extra;



HLTPi0RecHitsFilter::HLTPi0RecHitsFilter(const edm::ParameterSet& iConfig)
{
  barrelHits_ = iConfig.getParameter< edm::InputTag > ("barrelHits");
  endcapHits_ = iConfig.getParameter< edm::InputTag > ("endcapHits");
  

  pi0BarrelHits_ = iConfig.getParameter< std::string > ("pi0BarrelHitCollection");

  pi0EndcapHits_ = iConfig.getParameter< std::string > ("pi0EndcapHitCollection");
  
    
  
  ///Below are not used at All
  //  gammaCandEtaSize_ = iConfig.getParameter<int> ("gammaCandEtaSize");
  //gammaCandPhiSize_ = iConfig.getParameter<int> ("gammaCandPhiSize");
  //if ( gammaCandPhiSize_ % 2 == 0 ||  gammaCandEtaSize_ % 2 == 0)
  //  edm::LogError("AlCaPi0RecHitsProducerError") << "Size of eta/phi for sliding window should be odd numbers";


  



  clusSeedThr_ = iConfig.getParameter<double> ("clusSeedThr");
  clusSeedThrEndCap_ = iConfig.getParameter<double> ("clusSeedThrEndCap");

  clusEtaSize_ = iConfig.getParameter<int> ("clusEtaSize");
  clusPhiSize_ = iConfig.getParameter<int> ("clusPhiSize");
  if ( clusPhiSize_ % 2 == 0 ||  clusEtaSize_ % 2 == 0)
    edm::LogError("AlCaPi0RecHitsProducerError") << "Size of eta/phi for simple clustering should be odd numbers";

  selePtGammaOne_ = iConfig.getParameter<double> ("selePtGammaOne");  
  selePtGammaTwo_ = iConfig.getParameter<double> ("selePtGammaTwo");  
  selePtPi0_ = iConfig.getParameter<double> ("selePtPi0");  
  seleMinvMaxPi0_ = iConfig.getParameter<double> ("seleMinvMaxPi0");  
  seleMinvMinPi0_ = iConfig.getParameter<double> ("seleMinvMinPi0");  
  seleXtalMinEnergy_ = iConfig.getParameter<double> ("seleXtalMinEnergy");
  seleNRHMax_ = iConfig.getParameter<int> ("seleNRHMax");
  //New Selection
  seleS4S9GammaOne_ = iConfig.getParameter<double> ("seleS4S9GammaOne");  
  seleS4S9GammaTwo_ = iConfig.getParameter<double> ("seleS4S9GammaTwo");  
  selePi0Iso_ = iConfig.getParameter<double> ("selePi0Iso");  
  selePi0BeltDR_ = iConfig.getParameter<double> ("selePi0BeltDR");  
  selePi0BeltDeta_ = iConfig.getParameter<double> ("selePi0BeltDeta");  
 

  ///for EndCap
  

  
  selePtGammaEndCap_ = iConfig.getParameter<double> ("selePtGammaEndCap");  
  selePtPi0EndCap_ = iConfig.getParameter<double> ("selePtPi0EndCap");   
  seleS4S9GammaEndCap_ = iConfig.getParameter<double> ("seleS4S9GammaEndCap");  
  seleMinvMaxPi0EndCap_ = iConfig.getParameter<double> ("seleMinvMaxPi0EndCap");  
  seleMinvMinPi0EndCap_ = iConfig.getParameter<double> ("seleMinvMinPi0EndCap");  
  ptMinForIsolationEndCap_ = iConfig.getParameter<double> ("ptMinForIsolationEndCap");
  selePi0IsoEndCap_ = iConfig.getParameter<double> ("selePi0IsoEndCap");  
    
  
  

  ParameterLogWeighted_ = iConfig.getParameter<bool> ("ParameterLogWeighted");
  ParameterX0_ = iConfig.getParameter<double> ("ParameterX0");
  ParameterT0_barl_ = iConfig.getParameter<double> ("ParameterT0_barl");
  ParameterT0_endc_ = iConfig.getParameter<double> ("ParameterT0_endc");
  ParameterT0_endcPresh_ = iConfig.getParameter<double> ("ParameterT0_endcPresh");
  ParameterW0_ = iConfig.getParameter<double> ("ParameterW0");
  
  
  
  ///  detaL1_ = iConfig.getParameter<double> ("detaL1");
  ////dphiL1_ = iConfig.getParameter<double> ("dphiL1");
  ///  UseMatchedL1Seed_ = iConfig.getParameter<bool> ("UseMatchedL1Seed"); 

    

  l1IsolatedTag_ = iConfig.getParameter< edm::InputTag > ("l1IsolatedTag");
  l1NonIsolatedTag_ = iConfig.getParameter< edm::InputTag > ("l1NonIsolatedTag");
  l1SeedFilterTag_ = iConfig.getParameter< edm::InputTag > ("l1SeedFilterTag");

    

  debug_ = iConfig.getParameter<int> ("debugLevel");
  storeIsoClusRecHit_ = iConfig.getParameter<bool> ("storeIsoClusRecHit");
  ptMinForIsolation_ = iConfig.getParameter<double> ("ptMinForIsolation");
  
  
  

  ptMinEMObj_ = iConfig.getParameter<double>("ptMinEMObj");
  EMregionEtaMargin_ = iConfig.getParameter<double>("EMregionEtaMargin");
  EMregionPhiMargin_ = iConfig.getParameter<double>("EMregionPhiMargin");

  
  
  Jets_ = iConfig.getUntrackedParameter<bool>("Jets",false);

  if( Jets_){
    
    JETSdoCentral_ = iConfig.getUntrackedParameter<bool>("JETSdoCentral",true);
    JETSdoForward_ = iConfig.getUntrackedParameter<bool>("JETSdoForward",true);
    JETSdoTau_ = iConfig.getUntrackedParameter<bool>("JETSdoTau",true);
    
    JETSregionEtaMargin_ = iConfig.getUntrackedParameter<double>("JETS_regionEtaMargin",1.0);
    JETSregionPhiMargin_ = iConfig.getUntrackedParameter<double>("JETS_regionPhiMargin",1.0);

    Ptmin_jets_ = iConfig.getUntrackedParameter<double>("Ptmin_jets",0.);
    
    
    CentralSource_ = iConfig.getUntrackedParameter<edm::InputTag>("CentralSource");
    ForwardSource_ = iConfig.getUntrackedParameter<edm::InputTag>("ForwardSource");
    TauSource_ = iConfig.getUntrackedParameter<edm::InputTag>("TauSource");
    
    
  }
    
  

  TheMapping = new EcalElectronicsMapping();
  first_ = true;
  
   
 
  providedParameters.insert(std::make_pair("LogWeighted",ParameterLogWeighted_));
  providedParameters.insert(std::make_pair("X0",ParameterX0_));
  providedParameters.insert(std::make_pair("T0_barl",ParameterT0_barl_));
  providedParameters.insert(std::make_pair("T0_endc",ParameterT0_endc_));
  providedParameters.insert(std::make_pair("T0_endcPresh",ParameterT0_endcPresh_));
  providedParameters.insert(std::make_pair("W0",ParameterW0_));
  
  posCalculator_ = PositionCalc(providedParameters);
  
  

  //register your products
  produces< EBRecHitCollection >(pi0BarrelHits_);

  produces< EERecHitCollection >(pi0EndcapHits_);
  
  
  
}





HLTPi0RecHitsFilter::~HLTPi0RecHitsFilter()
{
 
  //  TimingReport::current()->dump(std::cout);

}


// ------------ method called to produce the data  ------------
bool
HLTPi0RecHitsFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;
  using namespace trigger;

  

  if (first_) {
    edm::ESHandle< EcalElectronicsMapping > ecalmapping;
    iSetup.get< EcalMappingRcd >().get(ecalmapping);
    const EcalElectronicsMapping* TheMapping_ = ecalmapping.product();
    *TheMapping = *TheMapping_;
    first_ = false;


    edm::ESHandle<CaloGeometry> geoHandle;
    // changes in 210pre5 to move to alignable geometry
    ///  iSetup.get<IdealGeometryRecord>().get(geoHandle);

    iSetup.get<CaloGeometryRecord>().get(geoHandle); 
    geometry_eb = geoHandle->getSubdetectorGeometry(DetId::Ecal,EcalBarrel);
    geometry_ee = geoHandle->getSubdetectorGeometry(DetId::Ecal,EcalEndcap);
    geometry_es = geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalPreshower);
    
    edm::ESHandle<CaloTopology> pTopology;
    iSetup.get<CaloTopologyRecord>().get(pTopology);
    topology_eb = pTopology->getSubdetectorTopology(DetId::Ecal,EcalBarrel);
    topology_ee = pTopology->getSubdetectorTopology(DetId::Ecal,EcalEndcap);
    

  }                 
  
  // Get the CaloGeometry
  edm::ESHandle<L1CaloGeometry> l1CaloGeom ;
  iSetup.get<L1CaloGeometryRecord>().get(l1CaloGeom) ;
    
  
  edm::Handle<trigger::TriggerFilterObjectWithRefs> L1SeedOutput;
  iEvent.getByLabel (l1SeedFilterTag_,L1SeedOutput);

  edm::Handle< l1extra::L1EmParticleCollection > l1EGIso;
  iEvent.getByLabel(l1IsolatedTag_, l1EGIso ) ;
  edm::Handle< l1extra::L1EmParticleCollection > l1EGNonIso ;
  iEvent.getByLabel(l1NonIsolatedTag_, l1EGNonIso ) ;


  
  

  bool accept=false;



  
  ///first get all the FEDs around EM objects with PT > defined value. 
  
  FEDListUsed.clear();
  vector<int>::iterator it; 
  for( l1extra::L1EmParticleCollection::const_iterator emItr = l1EGIso->begin();
       emItr != l1EGIso->end() ;++emItr ){
    
    float pt = emItr -> pt();


    if (debug_ >= 1) std::cout << " here is a L1 Iso EG Seed  with (eta,phi) = " <<
		       emItr->eta()<< " " << emItr->phi() << " and pt " << pt << std::endl;
    

    if( pt< ptMinEMObj_ ) continue; 

    int etaIndex = emItr->gctEmCand()->etaIndex() ;
    int phiIndex = emItr->gctEmCand()->phiIndex() ;
    double etaLow  = l1CaloGeom->etaBinLowEdge( etaIndex ) ;
    double etaHigh = l1CaloGeom->etaBinHighEdge( etaIndex ) ;
    double phiLow  = l1CaloGeom->emJetPhiBinLowEdge( phiIndex ) ;
    double phiHigh = l1CaloGeom->emJetPhiBinHighEdge( phiIndex ) ;
    
    std::vector<int> feds = ListOfFEDS(etaLow, etaHigh, phiLow, phiHigh, EMregionEtaMargin_, EMregionPhiMargin_);
    for (int n=0; n < (int)feds.size(); n++) {
      int fed = feds[n];
      it = find(FEDListUsed.begin(),FEDListUsed.end(),fed);
      if( it == FEDListUsed.end()){
	FEDListUsed.push_back(fed);
      }
    }
  }
  
  for( l1extra::L1EmParticleCollection::const_iterator emItr = l1EGNonIso->begin();
       emItr != l1EGNonIso->end() ;++emItr ){
    
    float pt = emItr -> pt();
    
    
    if (debug_ >= 1) std::cout << " here is a L1 NonIso EG Seed  with (eta,phi) = " <<
		       emItr->eta()<< " " << emItr->phi() << " and pt " << pt << std::endl;
    
    if( pt< ptMinEMObj_ ) continue; 
    

    int etaIndex = emItr->gctEmCand()->etaIndex() ;
    int phiIndex = emItr->gctEmCand()->phiIndex() ;
    double etaLow  = l1CaloGeom->etaBinLowEdge( etaIndex ) ;
    double etaHigh = l1CaloGeom->etaBinHighEdge( etaIndex ) ;
    double phiLow  = l1CaloGeom->emJetPhiBinLowEdge( phiIndex ) ;
    double phiHigh = l1CaloGeom->emJetPhiBinHighEdge( phiIndex ) ;
    
    std::vector<int> feds = ListOfFEDS(etaLow, etaHigh, phiLow, phiHigh, EMregionEtaMargin_, EMregionPhiMargin_);
    for (int n=0; n < (int)feds.size(); n++) {
      int fed = feds[n];
      it = find(FEDListUsed.begin(),FEDListUsed.end(),fed);
      if( it == FEDListUsed.end()){
	FEDListUsed.push_back(fed);
      }
    }
  }
  

  if( Jets_ ){
    
    double epsilon = 0.01;
    
    if (JETSdoCentral_) {
      
      edm::Handle<L1JetParticleCollection> jetColl;
      iEvent.getByLabel(CentralSource_,jetColl);
      
      for (L1JetParticleCollection::const_iterator jetItr=jetColl->begin(); jetItr != jetColl->end(); jetItr++) {
	
	double pt    =   jetItr-> pt();
	double eta   =   jetItr-> eta();
	double phi   =   jetItr-> phi();
	
	if (debug_ >= 1) std::cout << " here is a L1 CentralJet Seed  with (eta,phi) = " <<
			   eta << " " << phi << " and pt " << pt << std::endl;
	if (pt < Ptmin_jets_ ) continue;
	
	std::vector<int> feds = ListOfFEDS(eta, eta, phi-epsilon, phi+epsilon, JETSregionEtaMargin_, JETSregionPhiMargin_);
	for (int n=0; n < (int)feds.size(); n++) {
	  int fed = feds[n];
	  it = find(FEDListUsed.begin(),FEDListUsed.end(),fed);
	  if( it == FEDListUsed.end()){
	    FEDListUsed.push_back(fed);
	  }
	}
      }
      
    }

    if (JETSdoForward_) {

      edm::Handle<L1JetParticleCollection> jetColl;
      iEvent.getByLabel(ForwardSource_,jetColl);

      for (L1JetParticleCollection::const_iterator jetItr=jetColl->begin(); jetItr != jetColl->end(); jetItr++) {

	double pt    =  jetItr -> pt();
	double eta   =  jetItr -> eta();
	double phi   =  jetItr -> phi();
	  
	if (debug_ >= 1) std::cout << " here is a L1 ForwardJet Seed  with (eta,phi) = " <<
			   eta << " " << phi << " and pt " << pt << std::endl;
	if (pt < Ptmin_jets_ ) continue;
	  
	std::vector<int> feds = ListOfFEDS(eta, eta, phi-epsilon, phi+epsilon, JETSregionEtaMargin_, JETSregionPhiMargin_);
	  
	for (int n=0; n < (int)feds.size(); n++) {
	  int fed = feds[n];
	  it = find(FEDListUsed.begin(),FEDListUsed.end(),fed);
	  if( it == FEDListUsed.end()){
	    FEDListUsed.push_back(fed);
	  }
	}

      }
    }

    if (JETSdoTau_) {

      edm::Handle<L1JetParticleCollection> jetColl;
      iEvent.getByLabel(TauSource_,jetColl);

      for (L1JetParticleCollection::const_iterator jetItr=jetColl->begin(); jetItr != jetColl->end(); jetItr++) {

	double pt    =  jetItr -> pt();
	double eta   =  jetItr -> eta();
	double phi   =  jetItr -> phi();

	if (debug_ >= 1) std::cout << " here is a L1 TauJet Seed  with (eta,phi) = " <<
		      eta << " " << phi << " and pt " << pt << std::endl;
	if (pt < Ptmin_jets_ ) continue;

	std::vector<int> feds = ListOfFEDS(eta, eta, phi-epsilon, phi+epsilon, JETSregionEtaMargin_, JETSregionPhiMargin_);
	for (int n=0; n < (int)feds.size(); n++) {
	  int fed = feds[n];
	  it = find(FEDListUsed.begin(),FEDListUsed.end(),fed);
	  if( it == FEDListUsed.end()){
	    FEDListUsed.push_back(fed);
	  }
	}
		
      }
    }
    

  }

  //// end of getting FED List
  ///separate into barrel and endcap to speed up when checking
  FEDListUsedBarrel.clear();
  FEDListUsedEndcap.clear();
  for(  int j=0; j< int(FEDListUsed.size());j++){
    int fed = FEDListUsed[j];
    
    if( fed >= 10 && fed <= 45){
      FEDListUsedBarrel.push_back(fed);
    }else FEDListUsedEndcap.push_back(fed);
  }
  
  
  

  ///==============Start to process barrel part==================///
  

  Handle<EBRecHitCollection> barrelRecHitsHandle;

  iEvent.getByLabel(barrelHits_,barrelRecHitsHandle);
  if (!barrelRecHitsHandle.isValid()) {
    LogDebug("") << "AlCaPi0RecHitsProducer: Error! can't get product barrel hits!" << std::endl;
  }
  
  const EcalRecHitCollection *hitCollection_p = barrelRecHitsHandle.product();


  if(debug_>=1) std::cout<<" pi0 barrel_input_size: "<<iEvent.id().run()<<" "<<iEvent.id().event()<<" "<<hitCollection_p->size()<<std::endl;
  


  std::vector<EcalRecHit> seeds;
  seeds.clear();

  vector<EBDetId> usedXtals;
  usedXtals.clear();
  
  detIdEBRecHits.clear(); //// EBDetId
  EBRecHits.clear();  /// EcalRecHit
  
  ////make seeds. 
  EBRecHitCollection::const_iterator itb;
  for (itb=barrelRecHitsHandle->begin(); itb!=barrelRecHitsHandle->end(); itb++) {
    double energy = itb->energy();
    if( energy < seleXtalMinEnergy_) continue; 

    EBDetId det = itb->id();
    int fed = TheMapping->DCCid(det);

    it = find(FEDListUsedBarrel.begin(),FEDListUsedBarrel.end(),fed);
    if(it == FEDListUsedBarrel.end()) continue; 
    
    detIdEBRecHits.push_back(det);
    EBRecHits.push_back(*itb);
    

    if (energy > clusSeedThr_) seeds.push_back(*itb);

  }
  
  
  

  
  //Create empty output collections
  std::auto_ptr< EBRecHitCollection > pi0EBRecHitCollection( new EBRecHitCollection );
  
  
  
  

  int nClus;
  vector<float> eClus;
  vector<float> etClus;
  vector<float> etaClus;
  vector<float> phiClus;
  vector<EBDetId> max_hit;
  vector< vector<EcalRecHit> > RecHitsCluster;
  vector<float> s4s9Clus;

  nClus=0;

  // Make own simple clusters (3x3, 5x5 or clusPhiSize_ x clusEtaSize_)
  sort(seeds.begin(), seeds.end(), ecalRecHitLess());

  for (std::vector<EcalRecHit>::iterator itseed=seeds.begin(); itseed!=seeds.end(); itseed++) {
    EBDetId seed_id = itseed->id();
    std::vector<EBDetId>::const_iterator usedIds;
    
    bool seedAlreadyUsed=false;
    for(usedIds=usedXtals.begin(); usedIds!=usedXtals.end(); usedIds++){
      if(*usedIds==seed_id){
	seedAlreadyUsed=true;
	break; 
      }
    }

    if(seedAlreadyUsed)continue;

    std::vector<DetId> clus_v = topology_eb->getWindow(seed_id,clusEtaSize_,clusPhiSize_);	
    std::vector<DetId> clus_used;
    

    vector<EcalRecHit> RecHitsInWindow;
    
    double simple_energy = 0; 

    for (std::vector<DetId>::iterator det=clus_v.begin(); det!=clus_v.end(); det++) {
      EBDetId EBdet = *det;
      
      bool  HitAlreadyUsed=false;
      for(usedIds=usedXtals.begin(); usedIds!=usedXtals.end(); usedIds++){
	if(*usedIds==*det){
	  HitAlreadyUsed=true;
	  break;
	}
      }
     
      if(HitAlreadyUsed)continue;
      
      ///once again. check FED of this det.
      int fed = TheMapping->DCCid(EBdet);
      it = find(FEDListUsedBarrel.begin(),FEDListUsedBarrel.end(),fed);
      if(it == FEDListUsedBarrel.end()) continue; 
      

      std::vector<EBDetId>::iterator itdet = find( detIdEBRecHits.begin(),detIdEBRecHits.end(),EBdet);
      if(itdet == detIdEBRecHits.end()) continue; 
      
      int nn = int(itdet - detIdEBRecHits.begin());
      usedXtals.push_back(*det);
      RecHitsInWindow.push_back(EBRecHits[nn]);
      clus_used.push_back(*det);
      simple_energy = simple_energy + EBRecHits[nn].energy();
      
            
    }
    
    
    
    math::XYZPoint clus_pos = posCalculator_.Calculate_Location(clus_used,hitCollection_p,geometry_eb,geometry_es);
    
    
    float theta_s = 2. * atan(exp(-clus_pos.eta()));
    float p0x_s = simple_energy * sin(theta_s) * cos(clus_pos.phi());
    float p0y_s = simple_energy * sin(theta_s) * sin(clus_pos.phi());
    float et_s = sqrt( p0x_s*p0x_s + p0y_s*p0y_s);
    
        

    
    eClus.push_back(simple_energy);
    etClus.push_back(et_s);
    etaClus.push_back(clus_pos.eta());
    phiClus.push_back(clus_pos.phi());
    max_hit.push_back(seed_id);
    RecHitsCluster.push_back(RecHitsInWindow);
    //Compute S4/S9 variable
    //We are not sure to have 9 RecHits so need to check eta and phi:

    ///check s4s9
    float s4s9_tmp[4];
    for(int i=0;i<4;i++)s4s9_tmp[i]= 0;

    int seed_ieta = seed_id.ieta();
    int seed_iphi = seed_id.iphi();
    
    convxtalid( seed_iphi,seed_ieta);
    
    
    for(unsigned int j=0; j<RecHitsInWindow.size();j++){
      EBDetId det = (EBDetId)RecHitsInWindow[j].id(); 
      
      int ieta = det.ieta();
      int iphi = det.iphi();
      
      convxtalid(iphi,ieta);
      
      float en = RecHitsInWindow[j].energy(); 
      
      int dx = diff_neta_s(seed_ieta,ieta);
      int dy = diff_nphi_s(seed_iphi,iphi);
      
      if(dx <= 0 && dy <=0) s4s9_tmp[0] += en; 
      if(dx >= 0 && dy <=0) s4s9_tmp[1] += en; 
      if(dx <= 0 && dy >=0) s4s9_tmp[2] += en; 
      if(dx >= 0 && dy >=0) s4s9_tmp[3] += en; 
      
      
    }
    
    float s4s9_max = *max_element( s4s9_tmp,s4s9_tmp+4)/simple_energy; 
    s4s9Clus.push_back(s4s9_max);
    


    if(debug_>=1){
      std::cout<<"pi0 cluster (n,nxt,e,et eta,phi,s4s9) "<<nClus<<" "<<int(RecHitsInWindow.size())<<" "<<eClus[nClus]<<" "<<" "<<etClus[nClus]<<" "<<etaClus[nClus]<<" "<<phiClus[nClus]<<" "<<s4s9Clus[nClus]<<std::endl;
    }
    
    


    nClus++;
    if (nClus == MAXCLUS) return accept;
  }
  
  
  


  // Selection, based on Simple clustering
  //pi0 candidates
  
  int npi0_s=0;
  ////to avoid duplicated push_back rechit
  vector<int> indClusSelected; 


  for(Int_t i=0 ; i<nClus ; i++){
    for(Int_t j=i+1 ; j<nClus ; j++){
      
      if( etClus[i]>selePtGammaOne_ && etClus[j]>selePtGammaTwo_ && s4s9Clus[i]>seleS4S9GammaOne_ && s4s9Clus[j]>seleS4S9GammaTwo_){
	float theta_0 = 2. * atan(exp(-etaClus[i]));
	float theta_1 = 2. * atan(exp(-etaClus[j]));
        
	float p0x = eClus[i] * sin(theta_0) * cos(phiClus[i]);
	float p1x = eClus[j] * sin(theta_1) * cos(phiClus[j]);
	float p0y = eClus[i] * sin(theta_0) * sin(phiClus[i]);
	float p1y = eClus[j] * sin(theta_1) * sin(phiClus[j]);
	float p0z = eClus[i] * cos(theta_0);
	float p1z = eClus[j] * cos(theta_1);
        
	float pt_pi0 = sqrt( (p0x+p1x)*(p0x+p1x) + (p0y+p1y)*(p0y+p1y));

	if (pt_pi0 < selePtPi0_)continue;
	float m_inv = sqrt ( (eClus[i] + eClus[j])*(eClus[i] + eClus[j]) - (p0x+p1x)*(p0x+p1x) - (p0y+p1y)*(p0y+p1y) - (p0z+p1z)*(p0z+p1z) );  

	if ( (m_inv<seleMinvMaxPi0_) && (m_inv>seleMinvMinPi0_) ){
	  

	  //New Loop on cluster to measure isolation:
	  vector<int> IsoClus;
	  IsoClus.clear();
	  float Iso = 0;
	  TVector3 pi0vect = TVector3((p0x+p1x), (p0y+p1y), (p0z+p1z));
	  for(Int_t k=0 ; k<nClus ; k++){

	    if(etClus[k] < ptMinForIsolation_) continue; 
	    
	    
	    if(k==i || k==j)continue;
	    
	    TVector3 Clusvect = TVector3(eClus[k] * sin(2. * atan(exp(-etaClus[k]))) * cos(phiClus[k]), eClus[k] * sin(2. * atan(exp(-etaClus[k]))) * sin(phiClus[k]) , eClus[k] * cos(2. * atan(exp(-etaClus[k]))));
	    float dretaclpi0 = fabs(etaClus[k] - pi0vect.Eta());
	    float drclpi0 = Clusvect.DeltaR(pi0vect);
	    
	    if((drclpi0<selePi0BeltDR_) && (dretaclpi0<selePi0BeltDeta_) ){
	      Iso = Iso + etClus[k];
	      IsoClus.push_back(k);
	    }
	  }

	  if(Iso/pt_pi0<selePi0Iso_){

	    

	    it = find(indClusSelected.begin(),indClusSelected.end(),i);
	    if( it == indClusSelected.end()){
	      indClusSelected.push_back(i);
	      for(unsigned int Rec=0;Rec<RecHitsCluster[i].size();Rec++) pi0EBRecHitCollection->push_back(RecHitsCluster[i][Rec]);
	    }
	    
	    it = find(indClusSelected.begin(),indClusSelected.end(),j);
	    if( it == indClusSelected.end()){
	      indClusSelected.push_back(j);
	      for(unsigned int Rec2=0;Rec2<RecHitsCluster[j].size();Rec2++) pi0EBRecHitCollection->push_back(RecHitsCluster[j][Rec2]);
	    }
	    
	    if( storeIsoClusRecHit_){

	      for(unsigned int iii=0 ; iii<IsoClus.size() ; iii++){   
		int ind = IsoClus[iii];
		it = find(indClusSelected.begin(),indClusSelected.end(),ind);
		if( it == indClusSelected.end()){
		  indClusSelected.push_back(ind);
		  for(unsigned int Rec3=0;Rec3<RecHitsCluster[ind].size();Rec3++) pi0EBRecHitCollection->push_back(RecHitsCluster[ind][Rec3]);
		}
	      } 
	    }
	    
	    npi0_s++;
	  }
	  
	  if(npi0_s == MAXPI0S) return false; 
	}
      }
    } // End of the "j" loop over Simple Clusters
  } // End of the "i" loop over Simple Clusters

  

  if(debug_>=1) std::cout<<" pi0 barrel_ouput_size: "<<iEvent.id().run()<<" "<<iEvent.id().event()<<" "<<pi0EBRecHitCollection->size()<<std::endl;
  ///==============End of  barrel ==================///
  
  


  ///==============Start of  Endcap ==================///
  


  Handle<EERecHitCollection> endcapRecHitsHandle;

  iEvent.getByLabel(endcapHits_,endcapRecHitsHandle);
  if (!endcapRecHitsHandle.isValid()) {
    LogDebug("") << "AlCaPi0RecHitsProducer: Error! can't get product eeRecHit!" << std::endl;
  }
  
  const EcalRecHitCollection *hitCollection_e = endcapRecHitsHandle.product();
  if(debug_>=1) std::cout<<" pi0 endcap_input_size: "<<iEvent.id().run()<<" "<<iEvent.id().event()<<" "<<hitCollection_e->size()<<std::endl;
  

  
  detIdEERecHits.clear(); //// EEDetId
  EERecHits.clear();  /// EcalRecHit


  std::vector<EcalRecHit> seedsEndCap;
  seedsEndCap.clear();

  vector<EEDetId> usedXtalsEndCap;
  usedXtalsEndCap.clear();
  
  

  ////make seeds. 
  EERecHitCollection::const_iterator ite;
  for (ite=endcapRecHitsHandle->begin(); ite!=endcapRecHitsHandle->end(); ite++) {
    double energy = ite->energy();
    if( energy < seleXtalMinEnergy_) continue; 
    
    EEDetId det = ite->id();
    EcalElectronicsId elid = TheMapping->getElectronicsId(det);
    int fed = elid.dccId();
    it = find(FEDListUsedEndcap.begin(),FEDListUsedEndcap.end(),fed);
    if(it == FEDListUsedEndcap.end()) continue; 
    

    detIdEERecHits.push_back(det);
    EERecHits.push_back(*ite);
    

    if (energy > clusSeedThrEndCap_) seedsEndCap.push_back(*ite);
    
  }
  
  
  
  
  //Create empty output collections
  std::auto_ptr< EERecHitCollection > pi0EERecHitCollection( new EERecHitCollection );
  
  

  
  int nClusEndCap;
  vector<float> eClusEndCap;
  vector<float> etClusEndCap;
  vector<float> etaClusEndCap;
  vector<float> phiClusEndCap;
  vector< vector<EcalRecHit> > RecHitsClusterEndCap;
  vector<float> s4s9ClusEndCap;
  nClusEndCap=0;
  
    
  
  
  // Make own simple clusters (3x3, 5x5 or clusPhiSize_ x clusEtaSize_)
  sort(seedsEndCap.begin(), seedsEndCap.end(), ecalRecHitLess());
  
  
  for (std::vector<EcalRecHit>::iterator itseed=seedsEndCap.begin(); itseed!=seedsEndCap.end(); itseed++) {
    EEDetId seed_id = itseed->id();
    std::vector<EEDetId>::const_iterator usedIds;
    
    bool seedAlreadyUsed=false;
    for(usedIds=usedXtalsEndCap.begin(); usedIds!=usedXtalsEndCap.end(); usedIds++){
      if(*usedIds==seed_id){
	seedAlreadyUsed=true;
	break; 
      }
    }

    if(seedAlreadyUsed)continue;

    std::vector<DetId> clus_v = topology_ee->getWindow(seed_id,clusEtaSize_,clusPhiSize_);	
    std::vector<DetId> clus_used;
    
    

    vector<EcalRecHit> RecHitsInWindow;
    
    double simple_energy = 0; 

    for (std::vector<DetId>::iterator det=clus_v.begin(); det!=clus_v.end(); det++) {
      EEDetId EEdet = *det;
      
      bool  HitAlreadyUsed=false;
      for(usedIds=usedXtalsEndCap.begin(); usedIds!=usedXtalsEndCap.end(); usedIds++){
	if(*usedIds==*det){
	  HitAlreadyUsed=true;
	  break;
	}
      }
     
      if(HitAlreadyUsed)continue;
      
      ///once again. check FED of this det.
      EcalElectronicsId elid = TheMapping->getElectronicsId(EEdet);
      int fed = elid.dccId();
      it = find(FEDListUsedEndcap.begin(),FEDListUsedEndcap.end(),fed);
      if(it == FEDListUsedEndcap.end()) continue; 
      
      
      std::vector<EEDetId>::iterator itdet = find( detIdEERecHits.begin(),detIdEERecHits.end(),EEdet);
      if(itdet == detIdEERecHits.end()) continue; 
      
      
      int nn = int(itdet - detIdEERecHits.begin());
      usedXtalsEndCap.push_back(*det);
      RecHitsInWindow.push_back(EERecHits[nn]);
      clus_used.push_back(*det);
      simple_energy = simple_energy + EERecHits[nn].energy();
      
      
    }
    
        
    math::XYZPoint clus_pos = posCalculator_.Calculate_Location(clus_used,hitCollection_e,geometry_ee,geometry_es);
    

    float theta_s = 2. * atan(exp(-clus_pos.eta()));
    float p0x_s = simple_energy * sin(theta_s) * cos(clus_pos.phi());
    float p0y_s = simple_energy * sin(theta_s) * sin(clus_pos.phi());
    float et_s = sqrt( p0x_s*p0x_s + p0y_s*p0y_s);
    

    
    eClusEndCap.push_back(simple_energy);
    etClusEndCap.push_back(et_s);
    etaClusEndCap.push_back(clus_pos.eta());
    phiClusEndCap.push_back(clus_pos.phi());
    //    max_hit.push_back(seed_id);
    RecHitsClusterEndCap.push_back(RecHitsInWindow);
    //Compute S4/S9 variable
    //We are not sure to have 9 RecHits so need to check eta and phi:
    float s4s9_tmp[4];
    for(int i=0;i<4;i++) s4s9_tmp[i]= 0; 
    
    int ixSeed = seed_id.ix();
    int iySeed = seed_id.iy();
    for(unsigned int j=0; j<RecHitsInWindow.size();j++){
      EEDetId det_this = (EEDetId)RecHitsInWindow[j].id(); 
      int dx = ixSeed - det_this.ix();
      int dy = iySeed - det_this.iy();
      
      float en = RecHitsInWindow[j].energy(); 
      
      if(dx <= 0 && dy <=0) s4s9_tmp[0] += en; 
      if(dx >= 0 && dy <=0) s4s9_tmp[1] += en; 
      if(dx <= 0 && dy >=0) s4s9_tmp[2] += en; 
      if(dx >= 0 && dy >=0) s4s9_tmp[3] += en; 
      
    }
    s4s9ClusEndCap.push_back(*max_element( s4s9_tmp,s4s9_tmp+4)/simple_energy);
    
    
    
    if(debug_>=1){
      std::cout<<"pi0 ee cluster (n,nxt,e,et eta,phi,s4s9) "<<nClusEndCap<<" "<<int(RecHitsInWindow.size())<<" "<<eClusEndCap[nClusEndCap]<<" "<<" "<<etClusEndCap[nClusEndCap]<<" "<<etaClusEndCap[nClusEndCap]<<" "<<phiClusEndCap[nClusEndCap]<<" "<<s4s9ClusEndCap[nClusEndCap]<<std::endl;
    }
    

    nClusEndCap++;
    if (nClusEndCap == MAXCLUS) return false; 
  }
  
  
  
  
  // Selection, based on Simple clustering
  //pi0 candidates
  //  static const int MAXPI0S = 200;
  int npi0_se=0;
  ////to avoid duplicated push_back rechit
  vector<int> indClusEndCapSelected; 
  
  

  for(Int_t i=0 ; i<nClusEndCap ; i++){
    for(Int_t j=i+1 ; j<nClusEndCap ; j++){
      
      if( etClusEndCap[i]>selePtGammaEndCap_ && etClusEndCap[j]>selePtGammaEndCap_ && s4s9ClusEndCap[i]>seleS4S9GammaEndCap_ && s4s9ClusEndCap[j]>seleS4S9GammaEndCap_){
	float theta_0 = 2. * atan(exp(-etaClusEndCap[i]));
	float theta_1 = 2. * atan(exp(-etaClusEndCap[j]));
        
	float p0x = eClusEndCap[i] * sin(theta_0) * cos(phiClusEndCap[i]);
	float p1x = eClusEndCap[j] * sin(theta_1) * cos(phiClusEndCap[j]);
	float p0y = eClusEndCap[i] * sin(theta_0) * sin(phiClusEndCap[i]);
	float p1y = eClusEndCap[j] * sin(theta_1) * sin(phiClusEndCap[j]);
	float p0z = eClusEndCap[i] * cos(theta_0);
	float p1z = eClusEndCap[j] * cos(theta_1);
        
	float pt_pi0 = sqrt( (p0x+p1x)*(p0x+p1x) + (p0y+p1y)*(p0y+p1y));
	
	if (pt_pi0 < selePtPi0EndCap_)continue;
	float m_inv = sqrt ( (eClusEndCap[i] + eClusEndCap[j])*(eClusEndCap[i] + eClusEndCap[j]) - (p0x+p1x)*(p0x+p1x) - (p0y+p1y)*(p0y+p1y) - (p0z+p1z)*(p0z+p1z) );  

	if ( (m_inv<seleMinvMaxPi0EndCap_) && (m_inv>seleMinvMinPi0EndCap_) ){
	  

	  //New Loop on cluster to measure isolation:
	  vector<int> IsoClus;
	  IsoClus.clear();
	  float Iso = 0;
	  TVector3 pi0vect = TVector3((p0x+p1x), (p0y+p1y), (p0z+p1z));
	  for(Int_t k=0 ; k<nClusEndCap ; k++){

	    if(etClusEndCap[k] < ptMinForIsolationEndCap_) continue; 
	    
	    
	    if(k==i || k==j)continue;
	    
	    TVector3 Clusvect = TVector3(eClusEndCap[k] * sin(2. * atan(exp(-etaClusEndCap[k]))) * cos(phiClusEndCap[k]), eClusEndCap[k] * sin(2. * atan(exp(-etaClusEndCap[k]))) * sin(phiClusEndCap[k]) , eClusEndCap[k] * cos(2. * atan(exp(-etaClusEndCap[k]))));
	    float dretaclpi0 = fabs(etaClusEndCap[k] - pi0vect.Eta());
	    float drclpi0 = Clusvect.DeltaR(pi0vect);
	    
	    if(drclpi0<selePi0BeltDR_ && dretaclpi0<selePi0BeltDeta_ ){
	      Iso = Iso + etClusEndCap[k];
	      IsoClus.push_back(k);
	    }
	  }
	  
	  if(Iso/pt_pi0<selePi0IsoEndCap_){
	    

	    it = find(indClusEndCapSelected.begin(),indClusEndCapSelected.end(),i);
	    if( it == indClusEndCapSelected.end()){
	      indClusEndCapSelected.push_back(i);
	      for(unsigned int Rec=0;Rec<RecHitsClusterEndCap[i].size();Rec++) pi0EERecHitCollection->push_back(RecHitsClusterEndCap[i][Rec]);
	    }
	    
	    it = find(indClusEndCapSelected.begin(),indClusEndCapSelected.end(),j);
	    if( it == indClusEndCapSelected.end()){
	      indClusEndCapSelected.push_back(j);
	      for(unsigned int Rec2=0;Rec2<RecHitsClusterEndCap[j].size();Rec2++) pi0EERecHitCollection->push_back(RecHitsClusterEndCap[j][Rec2]);
	    }
	    
	    if( storeIsoClusRecHit_){

	      for(unsigned int iii=0 ; iii<IsoClus.size() ; iii++){   
		int ind = IsoClus[iii];
		it = find(indClusEndCapSelected.begin(),indClusEndCapSelected.end(),ind);
		if( it == indClusEndCapSelected.end()){
		  indClusEndCapSelected.push_back(ind);
		  for(unsigned int Rec3=0;Rec3<RecHitsClusterEndCap[ind].size();Rec3++) pi0EERecHitCollection->push_back(RecHitsClusterEndCap[ind][Rec3]);
		}
	      } 
	    }
	    
	    npi0_se++;
	  }
	  
	  if(npi0_se == MAXPI0S) return false; 
	}
      }
    } // End of the "j" loop over Simple Clusters
  } // End of the "i" loop over Simple Clusters
  
  ////==============End of endcap ===============///
  
  if(debug_>=1) std::cout<<" pi0 endcap_ouput_size: "<<iEvent.id().run()<<" "<<iEvent.id().event()<<" "<<pi0EERecHitCollection->size()<<std::endl;
  

  //Put selected information in the event
  int pi0_collsize = pi0EBRecHitCollection->size();
  int pi0_collsizeEndCap = pi0EERecHitCollection->size();
  
  
  


  if( pi0_collsize < 2 && pi0_collsizeEndCap <2) return false; 
  
  


  iEvent.put( pi0EBRecHitCollection, pi0BarrelHits_);
  iEvent.put( pi0EERecHitCollection, pi0EndcapHits_);

  

  return true; 
  
  
  
  
  
}





/////FED list 
std::vector<int> HLTPi0RecHitsFilter::ListOfFEDS(double etaLow, double etaHigh, double phiLow, 
					 double phiHigh, double etamargin, double phimargin)
{

	std::vector<int> FEDs;

	if (phimargin > Geom::pi()) phimargin =  Geom::pi() ;

	
	if (debug_>=2) std::cout << " etaLow etaHigh phiLow phiHigh " << etaLow << " " << 
			etaHigh << " " << phiLow << " " << phiHigh << std::endl;

        etaLow -= etamargin;
        etaHigh += etamargin;
        double phiMinus = phiLow - phimargin;
        double phiPlus = phiHigh + phimargin;

        bool all = false;
        double dd = fabs(phiPlus-phiMinus);
	if (debug_>=2) std::cout << " dd = " << dd << std::endl;
        if (dd > 2.*Geom::pi() ) all = true;

        while (phiPlus > Geom::pi()) { phiPlus -= 2.*Geom::pi() ; }
        while (phiMinus < 0) { phiMinus += 2.*Geom::pi() ; }
        if ( phiMinus > Geom::pi()) phiMinus -= 2.*Geom::pi() ;

        double dphi = phiPlus - phiMinus;
        if (dphi < 0) dphi += 2.*Geom::pi() ;
	if (debug_>=2) std::cout << "dphi = " << dphi << std::endl;
        if (dphi > Geom::pi()) {
                int fed_low1 = TheMapping -> GetFED(etaLow,phiMinus*180./Geom::pi());
                int fed_low2 = TheMapping -> GetFED(etaLow,phiPlus*180./Geom::pi());
		if (debug_>=2) std::cout << "fed_low1 fed_low2 " << fed_low1 << " " << fed_low2 << std::endl;
                if (fed_low1 == fed_low2) all = true;
                int fed_hi1 = TheMapping -> GetFED(etaHigh,phiMinus*180./Geom::pi());
                int fed_hi2 = TheMapping -> GetFED(etaHigh,phiPlus*180./Geom::pi());
		if (debug_>=2) std::cout << "fed_hi1 fed_hi2 " << fed_hi1 << " " << fed_hi2 << std::endl;
                if (fed_hi1 == fed_hi2) all = true;
        }

	if (all) {
		if (debug_>=2) std::cout << " unpack everything in phi ! " << std::endl;
		phiMinus = -20 * Geom::pi() / 180.;  // -20 deg
		phiPlus = -40 * Geom::pi() / 180.;  // -20 deg
	}

        if (debug_>=2) std::cout << " with margins : " << etaLow << " " << etaHigh << " " << 
			phiMinus << " " << phiPlus << std::endl;


        const EcalEtaPhiRegion ecalregion(etaLow,etaHigh,phiMinus,phiPlus);

        FEDs = TheMapping -> GetListofFEDs(ecalregion);

/*
	if (debug_) {
           int nn = (int)FEDs.size();
           for (int ii=0; ii < nn; ii++) {
                   std::cout << "unpack fed " << FEDs[ii] << std::endl;
           }
   	   }
*/

        return FEDs;

}


////already existing , int EcalElectronicsMapping::DCCid(const EBDetId& id)
int HLTPi0RecHitsFilter::convertSmToFedNumbBarrel(int ieta, int smId){
    
  if( ieta<=-1) return smId - 9; 
  else return smId + 27; 
  
  
}


void HLTPi0RecHitsFilter::convxtalid(Int_t &nphi,Int_t &neta)
{
  // Barrel only
  // Output nphi 0...359; neta 0...84; nside=+1 (for eta>0), or 0 (for eta<0).
  // neta will be [-85,-1] , or [0,84], the minus sign indicates the z<0 side.
  
  if(neta > 0) neta -= 1;
  if(nphi > 359) nphi=nphi-360;
  
  // final check
  if(nphi >359 || nphi <0 || neta< -85 || neta > 84)
    {
      std::cout <<" unexpected fatal error in HLTPi0RecHitsFilter::convxtalid "<<  nphi <<  " " << neta <<  " " <<std::endl;
      //exit(1);
    }
} //end of convxtalid




int HLTPi0RecHitsFilter::diff_neta_s(Int_t neta1, Int_t neta2){
  Int_t mdiff;
  mdiff=(neta1-neta2);
  return mdiff;
}

// Calculate the distance in xtals taking into account the periodicity of the Barrel
int HLTPi0RecHitsFilter::diff_nphi_s(Int_t nphi1,Int_t nphi2) {
   Int_t mdiff;
   if(abs(nphi1-nphi2) < (360-abs(nphi1-nphi2))) {
     mdiff=nphi1-nphi2;
   }
   else {
   mdiff=360-abs(nphi1-nphi2);
   if(nphi1>nphi2) mdiff=-mdiff;
   }
   return mdiff;
}
