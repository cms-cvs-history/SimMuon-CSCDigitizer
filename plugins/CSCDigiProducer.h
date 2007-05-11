#ifndef CSCDigiProducer_h
#define CSCDigiProducer_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimMuon/CSCDigitizer/src/CSCDigitizer.h"
#include "SimMuon/CSCDigitizer/src/CSCStripConditions.h"

class CSCDigiProducer : public edm::EDProducer
{
public:
  typedef CSCDigitizer::DigiSimLinks DigiSimLinks;

  explicit CSCDigiProducer(const edm::ParameterSet& ps);
  virtual ~CSCDigiProducer() {}

  /**Produces the EDM products,*/
  virtual void produce(edm::Event& e, const edm::EventSetup& c);

private:

  CSCDigitizer theDigitizer;
  CSCStripConditions * theStripConditions;
};

#endif
