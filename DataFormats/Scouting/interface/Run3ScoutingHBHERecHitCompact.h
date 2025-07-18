#ifndef DataFormats_Scouting_Run3ScoutingHBHERecHitCompact_h
#define DataFormats_Scouting_Run3ScoutingHBHERecHitCompact_h

#include <vector>

// Run-3 HLT-Scouting data format for HBHERecHits
//
// IMPORTANT: any changes to Run3ScoutingHBHERecHit must be backward-compatible !

class Run3ScoutingHBHERecHitCompact {
public:
  Run3ScoutingHBHERecHitCompact(float energy, unsigned int detId) : energy_{energy}, detId_{detId} {}

  Run3ScoutingHBHERecHitCompact() : energy_{0}, detId_{0} {}

  float energy() const { return energy_; }
  unsigned int detId() const { return detId_; }

private:
  uint16_t energy_;
  unsigned int detId_;
};

using Run3ScoutingHBHERecHitCompactCollection = std::vector<Run3ScoutingHBHERecHitCompact>;

#endif
