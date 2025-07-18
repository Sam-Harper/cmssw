#ifndef DataFormats_Scouting_Run3ScoutingEBRecHitCompact_h
#define DataFormats_Scouting_Run3ScoutingEBRecHitCompact_h

#include <vector>

// Run-3 HLT-Scouting data format for EBRecHits
//
// IMPORTANT: any changes to Run3ScoutingEBRecHit must be backward-compatible !

class Run3ScoutingEBRecHitCompact {
public:
  Run3ScoutingEBRecHitCompact(int16_t energy, int16_t time, int16_t detId, uint32_t flags) : energy_{energy}, time_{time}, detId_{detId}, flags_{flags} {}

  Run3ScoutingEBRecHitCompact() : energy_{0}, time_{0}, detId_{0}, flags_{0} {}

  int16_t energy() const { return energy_; }
  int16_t time() const { return time_; }
  int16_t detId() const { return detId_; }
  uint32_t flags() const { return flags_; }

private:
  int16_t energy_;
  int16_t time_;
  int16_t detId_;
  uint32_t flags_;
};

using Run3ScoutingEBRecHitCompactCollection = std::vector<Run3ScoutingEBRecHitCompact>;

#endif
