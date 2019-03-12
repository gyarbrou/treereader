////////////////////////////////////////////////////////////////////////
// Class:       T0TrackFilter
// Module Type: filter
// File:        T0TrackFilter_module.cc
//
// Generated at Wed Sep 14 08:59:22 2016 by Kazuhiro Terao using artmod
// from cetpkgsupport v1_10_02.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/OpFlash.h"
#include "lardataobj/AnalysisBase/CosmicTag.h"
#include "lardataobj/AnalysisBase/T0.h"
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

class T0TrackFilter;

class T0TrackFilter : public art::EDFilter {
public:
  explicit T0TrackFilter(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  T0TrackFilter(T0TrackFilter const &) = delete;
  T0TrackFilter(T0TrackFilter &&) = delete;
  T0TrackFilter & operator = (T0TrackFilter const &) = delete;
  T0TrackFilter & operator = (T0TrackFilter &&) = delete;

  // Required functions.
  bool filter(art::Event & e) override;


private:

  ::flashana::FlashMatchManager _mgr;
  
  std::vector<flashana::FlashMatch_t> _result;
  
  // Configurable params
  std::string _config_file;
  std::string _track_producer;
  std::string _opflash_producer_beam;
  std::string _opflash_producer_cosmic;
  double _flash_trange_start;
  double _flash_trange_end;
  size_t _num_tracks;
  std::vector<double> _gain_correction;

  TTree* _tree1;
  int _run, _subrun, _event, _matchid;
  int _tpcid, _flashid;
  double _tpc_xmin, _qll_xmin;
  double _t0, _score;
  double _hypo_pe, _flash_pe;

  TTree* _tree2;
  std::vector<double> _flash_spec;
  std::vector<double> _hypo_spec;
};


T0TrackFilter::T0TrackFilter(fhicl::ParameterSet const & p)
// :
// Initialize member data here.
{
  _track_producer          = p.get<std::string>("TrackProducer");
  _opflash_producer_beam   = p.get<std::string>("BeamOpFlashProducer");
  _opflash_producer_cosmic = p.get<std::string>("CosmicOpFlashProducer");
  _flash_trange_start      = p.get<double>("FlashVetoTimeStart");
  _flash_trange_end        = p.get<double>("FlashVetoTimeEnd");
  _num_tracks              = p.get<size_t>("MaxTrackCount");
  _gain_correction         = p.get<std::vector<double> >("GainCorrection");

  ::art::ServiceHandle<geo::Geometry> geo;
  ::art::ServiceHandle<geo::UBOpReadoutMap> ub_geo;
  
  if(geo->NOpDets() != _gain_correction.size()) {
    std::cout << "GainCorrection array size is " << _gain_correction.size() << " != # OpDet " << geo->NOpDets() << std::endl;
    throw std::exception();
  }
  
  _mgr.Configure(p.get<flashana::Config_t>("FlashMatchConfig"));

  _flash_spec.resize(geo->NOpDets(),0.);
  _hypo_spec.resize(geo->NOpDets(),0.);

  art::ServiceHandle<art::TFileService> fs;

  _tree1 = fs->make<TTree>("tree","");
  _tree1->Branch("run",&_run,"run/I");
  _tree1->Branch("subrun",&_subrun,"subrun/I");
  _tree1->Branch("event",&_event,"event/I");
  _tree1->Branch("matchid",&_matchid,"matchid/I");
  _tree1->Branch("tpcid",&_tpcid,"tpcid/I");
  _tree1->Branch("flashid",&_flashid,"flashid/I");
  _tree1->Branch("tpc_xmin",&_tpc_xmin,"tpc_xmin/D");
  _tree1->Branch("qll_xmin",&_qll_xmin,"qll_xmin/D");
  _tree1->Branch("t0",&_t0,"t0/D");
  _tree1->Branch("score",&_score,"score/D");
  _tree1->Branch("hypo_pe",&_hypo_pe,"hypo_pe/D");
  _tree1->Branch("flash_pe",&_flash_pe,"flash_pe/D");


  _tree2 = fs->make<TTree>("spectree","");
  _tree2->Branch("run",&_run,"run/I");
  _tree2->Branch("subrun",&_subrun,"subrun/I");
  _tree2->Branch("event",&_event,"event/I");
  _tree2->Branch("matchid",&_matchid,"matchid/I");
  _tree2->Branch("flash_spec","std::vector<double>",&_flash_spec);
  _tree2->Branch("hypo_spec","std::vector<double>",&_hypo_spec);
}

bool T0TrackFilter::filter(art::Event & e)
{
  _mgr.Reset();
  _result.clear();
  // _mgr.PrintConfig();

  ::art::ServiceHandle<geo::Geometry> geo;
  ::art::ServiceHandle<geo::UBOpReadoutMap> ub_geo;

  ::art::Handle<std::vector<anab::T0> > t0_h;
  e.getByLabel(Form("T0TrackTagger%s",_track_producer.c_str()),t0_h);
  if(!t0_h.isValid() || t0_h->empty()) return false;

  art::FindManyP<recob::OpFlash> flash_ptr_coll_v(t0_h, e, Form("T0TrackTagger%s",_track_producer.c_str()));
  art::FindManyP<recob::Track>   track_ptr_coll_v(t0_h, e, Form("T0TrackTagger%s",_track_producer.c_str()));
  std::set<const art::Ptr<recob::OpFlash> > flash_ptr_s;
  std::set<const art::Ptr<recob::Track> >   track_ptr_s;
  // See if there is an interesting flash
  for(size_t i=0; i<flash_ptr_coll_v.size(); ++i) {

    if(flash_ptr_coll_v.at(i).size()>1 || track_ptr_coll_v.at(i).size()>1) { 
      std::cout << "More than 1 association found!" << std::endl;
      throw std::exception();
    }
    const art::Ptr<recob::OpFlash> flash_ptr = flash_ptr_coll_v.at(i).at(0);
    const art::Ptr<recob::Track>   track_ptr = track_ptr_coll_v.at(i).at(0);
    if(flash_ptr->Time() < _flash_trange_start || _flash_trange_end < flash_ptr->Time()) continue;

    flash_ptr_s.insert(flash_ptr);
    track_ptr_s.insert(track_ptr);
    if(track_ptr_s.size()
  }

  if(flash_ptr_s.empty()) return false;


  const art::Ptr<recob::Track> mucs_track_ptr = ptr_coll_v.at(0).at(0);












  ::art::Handle<std::vector<recob::OpFlash> > beamflash_h;
  e.getByLabel(_opflash_producer_beam,beamflash_h);
  
  ::art::Handle<std::vector<recob::OpFlash> > cosmicflash_h;
  e.getByLabel(_opflash_producer_cosmic,cosmicflash_h);

  if( (!beamflash_h.isValid() || beamflash_h->empty()) && (!cosmicflash_h.isValid() || cosmicflash_h->empty()) )
    return false;

  size_t flash_id=0;
  if(beamflash_h.isValid()) {
    for (size_t n = 0; n < beamflash_h->size(); n++) {
      
      auto const& flash = (*beamflash_h)[n];
      if(flash.Time() < _flash_trange_start || _flash_trange_end < flash.Time())
	continue;
      
      ::flashana::Flash_t f;
      f.x = f.x_err = 0;
      f.y = flash.YCenter();
      f.z = flash.ZCenter();
      f.y_err = flash.YWidth();
      f.z_err = flash.ZWidth();
      f.pe_v.resize(geo->NOpDets());
      f.pe_err_v.resize(geo->NOpDets());
      for (unsigned int i = 0; i < f.pe_v.size(); i++) {
	unsigned int opdet = geo->OpDetFromOpChannel(i);
	f.pe_v[opdet] = flash.PE(i) * _gain_correction[i];
	f.pe_err_v[opdet] = sqrt(flash.PE(i) * _gain_correction[i]);
      }
      f.time = flash.Time();
      f.idx = flash_id;
      ++flash_id;
      _mgr.Emplace(std::move(f));
    }
  }
  
  if(cosmicflash_h.isValid()) {
    for (size_t n = 0; n < cosmicflash_h->size(); n++) {
      
      auto const& flash = (*cosmicflash_h)[n];
      if(flash.Time() < _flash_trange_start || _flash_trange_end < flash.Time())
	continue;
      
      ::flashana::Flash_t f;
      f.x = f.x_err = 0;
      f.y = flash.YCenter();
      f.z = flash.ZCenter();
      f.y_err = flash.YWidth();
      f.z_err = flash.ZWidth();
      f.pe_v.resize(geo->NOpDets());
      f.pe_err_v.resize(geo->NOpDets());
      for (unsigned int i = 0; i < f.pe_v.size(); i++) {
	unsigned int opdet = geo->OpDetFromOpChannel(i);
	if(flash.PE(i) == 0.) {
	  f.pe_v[opdet]=-1.;
	  f.pe_err_v[opdet]=-1.;
	}else{
	  f.pe_v[opdet] = flash.PE(i) * _gain_correction[i] / 0.424;
	  f.pe_err_v[opdet] = sqrt(flash.PE(i) * _gain_correction[i]) / 0.424;
	}
      }
      f.time = flash.Time();
      f.idx = flash_id;
      ++flash_id;
      _mgr.Emplace(std::move(f));
    }
  }
  if(!flash_id) {
    std::cerr << "No relevant flash found... " << std::endl;
    return false;
  }

  ::art::Handle<std::vector<recob::Track> > track_h;
  e.getByLabel(_track_producer,track_h);
  if(!track_h.isValid() || track_h->empty())  {
    std::cerr << "Cosmic tag exists but not track! Cannot happen..." << std::endl;
    throw std::exception();
  }

  art::FindManyP<recob::Track> ptr_coll_v(ctag_h, e, Form("MuCSTrackTagger%s",_track_producer.c_str()));
  if(ptr_coll_v.size() != 1) {
    std::cout << ">1 association in this event!" << std::endl;
    return false;
  }
  if(ptr_coll_v.at(0).size() !=1) {
    std::cout << ">1 associated track from one ctag?!" << std::endl;
    return false;
  }

  const art::Ptr<recob::Track> mucs_track_ptr = ptr_coll_v.at(0).at(0);

  ::geoalgo::Vector mucs_track_start(mucs_track_ptr->LocationAtPoint(0));
  ::geoalgo::Vector mucs_track_end(mucs_track_ptr->LocationAtPoint(mucs_track_ptr->NumberTrajectoryPoints()-1));
  ::geoalgo::AABox fidvol(10, (-1.)*(geo->DetHalfHeight())+10., 10.,
			  geo->DetHalfWidth()*2-10., geo->DetHalfHeight()-10., geo->DetLength() -10.);
  if(fidvol.Contain(mucs_track_start) || fidvol.Contain(mucs_track_end)) {
    //std::cerr << "Start/End not close enough to the detector edge!" << storage->event_id() << std::endl;
    std::cerr << "Start: (" << mucs_track_start[0] << "," << mucs_track_start[1] << "," << mucs_track_start[2] << ")" << std::endl;
    std::cerr << "End: (" << mucs_track_end[0] << "," << mucs_track_end[1] << "," << mucs_track_end[2] << ")" << std::endl;
    return false;
  }

  ::geoalgo::Trajectory mucs_geotrj;
  mucs_geotrj.reserve(mucs_track_ptr->NumberTrajectoryPoints() - 1);
  for (size_t i = 0; i < mucs_track_ptr->NumberTrajectoryPoints(); ++i) {
    ::geoalgo::Vector pt(mucs_track_ptr->LocationAtPoint(i));
    mucs_geotrj.emplace_back(std::move(pt));
  }
  auto mucs_qcluster = ((flashana::LightPath*)(_mgr.GetCustomAlgo("LightPath")))->FlashHypothesis(mucs_geotrj);
  _mgr.Emplace(std::move(mucs_qcluster));
  
  for (size_t trk_idx=0; trk_idx<track_h->size(); trk_idx++) {

    const art::Ptr<recob::Track> track_ptr(track_h,trk_idx);

    if(track_ptr == mucs_track_ptr) {
      std::cout << "Skipping MuCS track" << std::endl;
      continue;
    }
    
    if(_mgr.QClusterArray().size() >= _num_tracks) break;
    
    mucs_geotrj.resize(track_ptr->NumberTrajectoryPoints(),::geoalgo::Vector(0.,0.,0.));
    for (size_t pt_idx=0; pt_idx < track_ptr->NumberTrajectoryPoints(); ++pt_idx) {
      auto const& pt = track_ptr->LocationAtPoint(pt_idx);
      mucs_geotrj[pt_idx][0] = pt[0];
      mucs_geotrj[pt_idx][1] = pt[1];
      mucs_geotrj[pt_idx][2] = pt[2];
    }
    auto qcluster = ((flashana::LightPath*)(_mgr.GetCustomAlgo("LightPath")))->FlashHypothesis(mucs_geotrj);
    _mgr.Emplace(std::move(qcluster));
  }

  _result = _mgr.Match();

  _run    = e.id().run();
  _subrun = e.id().subRun();
  _event  = e.id().event();
  _matchid = 0;
  for(_matchid=0; _matchid < (int)(_result.size()); ++_matchid) {

    auto const& match = _result[_matchid];
    _tpcid    = match.tpc_id;
    _flashid  = match.flash_id;
    _score    = match.score;
    _qll_xmin = match.tpc_point.x;

    _tpc_xmin = 1.e4;
    for(auto const& pt : _mgr.QClusterArray()[_tpcid])

      if(pt.x < _tpc_xmin) _tpc_xmin = pt.x;

    auto const& flash = _mgr.FlashArray()[_flashid];

    _t0 = flash.time;

    if(_hypo_spec.size() != match.hypothesis.size()) {
      std::cout << "Hypothesis size mismatch!" << std::endl;
      throw std::exception();
    }

    for(size_t pmt=0; pmt<_hypo_spec.size(); ++pmt) _hypo_spec[pmt]  = match.hypothesis[pmt];
    for(size_t pmt=0; pmt<_hypo_spec.size(); ++pmt) _flash_spec[pmt] = flash.pe_v[pmt];

    _flash_pe = 0.;
    _hypo_pe  = 0.;
    for(auto const& v : _hypo_spec) _hypo_pe += v;
    for(auto const& v : _flash_spec) _flash_pe += v;

    _tree1->Fill();
    _tree2->Fill();
  }
  return true;
}

DEFINE_ART_MODULE(T0TrackFilter)
