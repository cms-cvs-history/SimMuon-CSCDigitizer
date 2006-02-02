#include "Utilities/Timing/interface/TimingReport.h" 
#include "SimMuon/CSCDigitizer/src/CSCDigitizer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimMuon/CSCDigitizer/src/CSCDetectorHit.h"
#include "SimMuon/CSCDigitizer/src/CSCWireHitSim.h"
#include "SimMuon/CSCDigitizer/src/CSCStripHitSim.h"
#include "SimMuon/CSCDigitizer/src/CSCDriftSim.h"
#include "SimMuon/CSCDigitizer/src/CSCWireElectronicsSim.h"
#include "SimMuon/CSCDigitizer/src/CSCStripElectronicsSim.h"
#include "Geometry/CSCSimAlgo/interface/CSCLayer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <iostream>


// default constructor allocates default wire and strip digitizers
CSCDigitizer::CSCDigitizer() {

  theDriftSim = new CSCDriftSim();
  theWireHitSim          = new CSCWireHitSim(theDriftSim);
  theStripHitSim         = new CSCStripHitSim();
  theWireElectronicsSim  = new CSCWireElectronicsSim(0., true);
  theStripElectronicsSim = new CSCStripElectronicsSim(8, true, true);

}


CSCDigitizer::~CSCDigitizer() {
  delete theStripElectronicsSim;
  delete theWireElectronicsSim;
  delete theStripHitSim;
  delete theWireHitSim;
  delete theDriftSim;
}



void CSCDigitizer::doAction(MixCollection<PSimHit> & simHits, 
                            CSCWireDigiCollection & wireDigis, 
                            CSCStripDigiCollection & stripDigis, 
                            CSCComparatorDigiCollection & comparators) 
{
  // arrange the hits by layer
  std::map<int, edm::PSimHitContainer> hitMap;
  for(MixCollection<PSimHit>::MixItr hitItr = simHits.begin();
      hitItr != simHits.end(); ++hitItr) 
  {
    hitMap[hitItr->detUnitId()].push_back(*hitItr);
  }

  // now loop over layers and run the simulation for each one
  for(std::map<int, edm::PSimHitContainer>::const_iterator hitMapItr = hitMap.begin();
      hitMapItr != hitMap.end(); ++hitMapItr)
  {
    const CSCLayer * layer = findLayer(hitMapItr->first);
    const edm::PSimHitContainer & layerSimHits = hitMapItr->second;

    std::vector<CSCDetectorHit> newWireHits, newStripHits;
  
    LogDebug("CSCDigitizer") << "CSCDigitizer: found " << layerSimHits.size() <<" hit(s) in layer";

    // turn the edm::PSimHits into WireHits, using the WireHitSim
    {
      TimeMe t("CSCWireHitSim");
      newWireHits.swap(theWireHitSim->simulate(layer, layerSimHits));
    }
    if(!newWireHits.empty()) {
      TimeMe t("CSCStripHitSim");
      newStripHits.swap(theStripHitSim->simulate(layer, newWireHits));
    }

    // turn the hits into wire digis, using the electronicsSim
    {
      TimeMe t("CSCWireElectronicsSim");
      theWireElectronicsSim->simulate(layer, newWireHits);
      theWireElectronicsSim->fillDigis(wireDigis);
    }  
    {
      TimeMe t("CSCStripElectronicsSim");
      theStripElectronicsSim->simulate(layer, newStripHits);
      theStripElectronicsSim->fillDigis(stripDigis, comparators);
    }
  }
}


void CSCDigitizer::setMagneticField(const MagneticField * field) {
  theDriftSim->setMagneticField(field);
}


const CSCLayer * CSCDigitizer::findLayer(int detId) const {
  assert(theTrackingGeometry != 0);
  const GeomDetUnit* detUnit = theTrackingGeometry->idToDet(CSCDetId(detId));
  return dynamic_cast<const CSCLayer *>(detUnit);
}
