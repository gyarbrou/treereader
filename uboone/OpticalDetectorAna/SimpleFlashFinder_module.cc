////////////////////////////////////////////////////////////////////////
// Class:       SimpleFlashFinder
// Module Type: producer
// File:        SimpleFlashFinder_module.cc
//
// Generated at Fri Oct  9 18:52:10 2015 by David Caratelli using artmod
// from cetpkgsupport v1_08_06.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// data-products
#include "RecoBase/OpHit.h"
#include "RecoBase/OpFlash.h"

#include <memory>
#include <iostream>

class SimpleFlashFinder;

class SimpleFlashFinder : public art::EDProducer {
public:
  explicit SimpleFlashFinder(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  SimpleFlashFinder(SimpleFlashFinder const &) = delete;
  SimpleFlashFinder(SimpleFlashFinder &&) = delete;
  SimpleFlashFinder & operator = (SimpleFlashFinder const &) = delete;
  SimpleFlashFinder & operator = (SimpleFlashFinder &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;


private:

  // Declare member data here.
  std::string fOpHitProducer;

};


SimpleFlashFinder::SimpleFlashFinder(fhicl::ParameterSet const & p)
// :
// Initialize member data here.
{
  produces< std::vector<recob::OpFlash>   >();
  //produces< std::vector<recob::OpHit>     >();
  fOpHitProducer = p.get<std::string>("OpFlashProducer");
}

void SimpleFlashFinder::produce(art::Event & e)
{

  // produce OpFlash data-product to be filled within module
  std::unique_ptr< std::vector<recob::OpFlash> > opflashes(new std::vector<recob::OpFlash>);

  // load OpHits previously created
  art::Handle<std::vector<recob::OpHit> > ophit_h;
  e.getByLabel(fOpHitProducer,ophit_h);
  // make sure hits look good
  if(!ophit_h.isValid()) {
    std::cerr<<"\033[93m[ERROR]\033[00m ... could not locate OpHit!"<<std::endl;
    throw std::exception();
  }

  // how to reconstruct OpFlashes:
  // divide the event in 100 ns windows
  // count up all the OpHits in each 100 ns window
  // and add up their charge
  // each 100 ns time-window will produce an OpFlash
  // provided > 0 PE are deposited in this time-window
  
  // find the OpHit time boundaries:
  // hit with smalles and largest time
  double ophit_t_min =  1.e9;
  double ophit_t_max = -1.e9;
  for(auto const& hit : *ophit_h) {
    if (hit.PeakTime() > ophit_t_max)
      ophit_t_max = hit.PeakTime();
    if (hit.PeakTime() < ophit_t_min)
      ophit_t_min = hit.PeakTime();
  }

  // optical hit time boundaries now set and in usec
  // how many 100 ns wide bins do we need?
  int n_bins = int((ophit_t_max-ophit_t_min)/0.1)+1;
  
  
  // collect the total optical charge collected in the hits
  // in the various 100 ns-wide time slices
  std::vector<double> OpCharge(n_bins,0.);
  for(auto const& hit : *ophit_h) {
    // figure out which 100-ns time bin this should go to
    double time = hit.PeakTime();
    int bin = int((time-ophit_t_min)/0.1);
    if (bin > n_bins){
      std::cout << "we should not have a hit at this time! something is wrong" << std::endl;
      continue;
    }
    OpCharge[bin] += hit.PE();
  }// for all OpHits



  // now loop through the 100-ns windows and create flashes
  for (size_t i=0; i < OpCharge.size(); i++){
    if (OpCharge[i] <= 0)
      continue;
    // for now, each individual PMT is assigned 0 PE
    // except for the 1st pmt which gets all the charge
    std::vector<double> PEperPMT(32,0.);
    PEperPMT[0] = OpCharge[i];
    // flash time = bin*100 ns + 50 ns
    double time = i*0.1+0.05+ophit_t_min;
    recob::OpFlash flash(time,0.05,time,0,PEperPMT);
    opflashes->push_back(flash);
  }// for all 100 ns time-intervals


  e.put(std::move(opflashes));
}

DEFINE_ART_MODULE(SimpleFlashFinder)
