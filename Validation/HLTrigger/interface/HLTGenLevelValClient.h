#ifndef VALIDATION_HLTRIGGER_HLTGENLEVELVALCLIENT
#define VALIDATION_HLTRIGGER_HLTGENLEVELVALCLIENT

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DQMServices/Core/interface/DQMEDHarvester.h"
#include "DQMServices/Core/interface/DQMStore.h"

#include <vector>
#include <string>

class HLTGenLevelValClient : public DQMEDHarvester {
private:
  // DQMStore* dbe_; //dbe seems to be the standard name for this, I dont know why. We of course dont own it
  std::string dirName_;

  std::vector<std::string> filterNames_;  //names of the filters
  
  std::vector<std::string> effVars_;

  bool runClientEndLumiBlock_;
  bool runClientEndRun_;
  bool runClientEndJob_;

  bool isSetup_;
  std::string hltTag_;

public:
  explicit HLTGenLevelValClient(const edm::ParameterSet&);
  ~HLTGenLevelValClient() override;

  void beginRun(const edm::Run& run, const edm::EventSetup& c) override;

  void dqmEndJob(DQMStore::IBooker&, DQMStore::IGetter&) override;  //performed in the endJob
  void dqmEndLuminosityBlock(DQMStore::IBooker&,
                             DQMStore::IGetter&,
                             edm::LuminosityBlock const&,
                             edm::EventSetup const&) override;  //performed in the endLumi

  void createEffHists(const std::string& filterName,
                            const std::string& baseName,
                            const std::string& region,
                            const std::vector<std::string>& varNames,
                            DQMStore::IBooker&,
                            DQMStore::IGetter&);


  MonitorElement* makeEffMonElemFromPassAndAll(const std::string& filterName,
                                               const std::string& name,
                                               const std::string& title,
                                               const MonitorElement* pass,
                                               const MonitorElement* all,
                                               DQMStore::IBooker&,
                                               DQMStore::IGetter&);

private:
  void runClient_(DQMStore::IBooker&, DQMStore::IGetter&);  //master function which runs the client
};

#endif
