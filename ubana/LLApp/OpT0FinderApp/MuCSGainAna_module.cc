////////////////////////////////////////////////////////////////////////
// Class:       MuCSGainAna
// Module Type: producer
// File:        MuCSGainAna_module.cc
//
// Generated at Mon Oct 17 12:10:40 2016 by Marco Del Tutto using artmod
// from cetpkgsupport v1_10_02.
////////////////////////////////////////////////////////////////////////

#include "canvas/Persistency/Common/FindManyP.h"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

/*
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
*/

#include <memory>

#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/OpFlash.h"
#include "lardataobj/RecoBase/OpHit.h"
#include "lardataobj/AnalysisBase/CosmicTag.h"
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/CryostatGeo.h"
#include "larcorealg/Geometry/PlaneGeo.h"
#include "larcorealg/Geometry/OpDetGeo.h"
#include "ubcore/Geometry/UBOpReadoutMap.h"

#include <memory>

#include "ubcore/LLBasicTool/GeoAlgo/GeoTrajectory.h"
#include "ubreco/LLSelectionTool/OpT0Finder/Base/FlashMatchManager.h"
#include "ubreco/LLSelectionTool/OpT0Finder/Algorithms/LightPath.h"
#include "TString.h"
#include "TTree.h"

class MuCSGainAna;

class MuCSGainAna : public art::EDProducer {
public:
  explicit MuCSGainAna(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  MuCSGainAna(MuCSGainAna const &) = delete;
  MuCSGainAna(MuCSGainAna &&) = delete;
  MuCSGainAna & operator = (MuCSGainAna const &) = delete;
  MuCSGainAna & operator = (MuCSGainAna &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;


private:

  std::string _ophit_producer;
  TTree* _tree;
  double _area;
  double _amp;
  double _pe;
  int    _ch;
  double _twidth;
  double _tpeak;

  // Declare member data here.

};


MuCSGainAna::MuCSGainAna(fhicl::ParameterSet const & p)
// :
// Initialize member data here.
{
  _ophit_producer = p.get<std::string>("OpHitProducer");

  art::ServiceHandle<art::TFileService> fs;

  _tree = fs->make<TTree>("tree","");

  _tree->Branch("area",&_area,"area/D");
  _tree->Branch("amp",&_amp,"amp/D");
  _tree->Branch("pe",&_pe,"pe/D");
  _tree->Branch("ch",&_ch,"ch/I");
  _tree->Branch("twidth",&_twidth,"twidth/D");
  _tree->Branch("tpeak",&_tpeak,"tpeak/D");


  // Call appropriate produces<>() functions here.
}

void MuCSGainAna::produce(art::Event & e)
{
  // Implementation of required member function here.
  ::art::Handle<std::vector<recob::OpHit> > ophit_h;
  e.getByLabel(_ophit_producer.c_str(),ophit_h);

  for (size_t n = 0; n < ophit_h->size(); n++) {

    auto const& oph = (*ophit_h)[n];

    _ch = oph.OpChannel();
    if(_ch > 231 || _ch < 200)  continue;
    _tpeak = oph.PeakTime();
    if(0.0 < _tpeak && _tpeak < 30) continue;
    _amp = oph.Amplitude();
    _pe = oph.PE();
    _area = oph.Area();
    _twidth = oph.Width();

    _tree->Fill();
  }

  

}

DEFINE_ART_MODULE(MuCSGainAna)
