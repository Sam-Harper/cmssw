/*
 *  Class:HLTGenValClient
 *
 *  DQM histogram post processor of HLT Gen validation code
 *  Modyfied copy of DQMGenericClient
 *
 *  \author Finn Labe - UHH
 *  original by Junghwan Goh - SungKyunKwan University
 */

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DQMServices/Core/interface/DQMEDHarvester.h"

#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TClass.h>
#include <TString.h>
#include <TPRegexp.h>
#include <TDirectory.h>
#include <TEfficiency.h>

#include <set>
#include <cmath>
#include <string>
#include <vector>
#include <climits>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace edm;

class HLTGenValClient : public DQMEDHarvester {
public:
  HLTGenValClient(const edm::ParameterSet& pset);
  ~HLTGenValClient() override{};

  void dqmEndLuminosityBlock(DQMStore::IBooker& ibooker,
                             DQMStore::IGetter& igetter,
                             const edm::LuminosityBlock& lumiSeg,
                             const edm::EventSetup& c) override;
  void dqmEndRun(DQMStore::IBooker&, DQMStore::IGetter&, edm::Run const&, edm::EventSetup const&) override;
  void dqmEndJob(DQMStore::IBooker&, DQMStore::IGetter&) override{};

  enum class EfficType { none = 0, efficiency, fakerate, simpleratio };

  struct EfficOption {
    std::string name, title;
    std::string numerator, denominator;
    EfficType type;
    bool isProfile;
  };

  struct ResolOption {
    std::string namePrefix, titlePrefix;
    std::string srcName;
  };

  struct ProfileOption {
    std::string name, title;
    std::string srcName;
  };

  struct NormOption {
    std::string name, normHistName;
  };

  struct CDOption {
    std::string name;
    bool ascending;
  };

  struct NoFlowOption {
    std::string name;
  };

  void computeEfficiency(DQMStore::IBooker& ibooker,
                         DQMStore::IGetter& igetter,
                         const std::string& startDir,
                         const std::string& efficMEName,
                         const std::string& efficMETitle,
                         const std::string& recoMEName,
                         const std::string& simMEName,
                         const EfficType type = EfficType::efficiency,
                         const bool makeProfile = false);
  void computeResolution(DQMStore::IBooker& ibooker,
                         DQMStore::IGetter& igetter,
                         const std::string& startDir,
                         const std::string& fitMEPrefix,
                         const std::string& fitMETitlePrefix,
                         const std::string& srcMEName);
  void computeProfile(DQMStore::IBooker& ibooker,
                      DQMStore::IGetter& igetter,
                      const std::string& startDir,
                      const std::string& profileMEName,
                      const std::string& profileMETitle,
                      const std::string& srcMEName);

  void normalizeToEntries(DQMStore::IBooker& ibooker,
                          DQMStore::IGetter& igetter,
                          const std::string& startDir,
                          const std::string& histName,
                          const std::string& normHistName);
  void makeCumulativeDist(DQMStore::IBooker& ibooker,
                          DQMStore::IGetter& igetter,
                          const std::string& startDir,
                          const std::string& cdName,
                          bool ascending = true);
  void makeNoFlowDist(DQMStore::IBooker& ibooker,
                      DQMStore::IGetter& igetter,
                      const std::string& startDir,
                      const std::string& cdName);

  void limitedFit(MonitorElement* srcME, MonitorElement* meanME, MonitorElement* sigmaME);

private:
  TPRegexp metacharacters_;
  TPRegexp nonPerlWildcard_;
  unsigned int verbose_;
  bool runOnEndLumi_;
  bool runOnEndJob_;
  bool makeGlobalEffPlot_;
  bool isWildcardUsed_;
  bool resLimitedFit_;

  DQMStore* theDQM;
  std::vector<std::string> subDirs_;
  std::string outputFileName_;

  std::vector<EfficOption> efficOptions_;

  void generic_eff(TH1* denom, TH1* numer, MonitorElement* efficiencyHist, const EfficType type = EfficType::efficiency);

  void findAllSubdirectories(DQMStore::IBooker& ibooker,
                             DQMStore::IGetter& igetter,
                             std::string dir,
                             std::set<std::string>* myList,
                             const TString& pattern);

  void makeAllPlots(DQMStore::IBooker&, DQMStore::IGetter&);

  void removeMEIfBooked(const std::string& meName, DQMStore::IGetter& igetter);
};

typedef HLTGenValClient::MonitorElement ME;

HLTGenValClient::HLTGenValClient(const ParameterSet& pset)
    : metacharacters_("[\\^\\$\\.\\*\\+\\?\\|\\(\\)\\{\\}\\[\\]]"), nonPerlWildcard_("\\w\\*|^\\*") {
  typedef std::vector<edm::ParameterSet> VPSet;
  typedef std::vector<std::string> vstring;
  typedef boost::escaped_list_separator<char> elsc;

  elsc commonEscapes("\\", " \t", "\'");

  verbose_ = pset.getUntrackedParameter<unsigned int>("verbose", 0);
  runOnEndLumi_ = pset.getUntrackedParameter<bool>("runOnEndLumi", false);
  runOnEndJob_ = pset.getUntrackedParameter<bool>("runOnEndJob", true);
  makeGlobalEffPlot_ = pset.getUntrackedParameter<bool>("makeGlobalEffienciesPlot", true);

  outputFileName_ = pset.getUntrackedParameter<string>("outputFileName", "");
  subDirs_ = pset.getUntrackedParameter<vstring>("subDirs");

  resLimitedFit_ = pset.getUntrackedParameter<bool>("resolutionLimitedFit", false);
  isWildcardUsed_ = false;
}

void HLTGenValClient::dqmEndLuminosityBlock(DQMStore::IBooker& ibooker,
                                             DQMStore::IGetter& igetter,
                                             const edm::LuminosityBlock& lumiSeg,
                                             const edm::EventSetup& c) {
  if (runOnEndLumi_) {
    makeAllPlots(ibooker, igetter);
  }
}

void HLTGenValClient::dqmEndRun(DQMStore::IBooker& ibooker,
                                 DQMStore::IGetter& igetter,
                                 edm::Run const&,
                                 edm::EventSetup const&) {
  // Create new MEs in endRun, even though we are requested to do it in endJob.
  // This gives the QTests a chance to run, before summaries are created in
  // endJob. The negative side effect is that we cannot run the GenericClient
  // for plots produced in Harvesting, but that seems rather rare.
  //
  // It is important that this is still save in the presence of multiple runs,
  // first because in multi-run harvesting, we accumulate statistics over all
  // runs and have full statistics at the endRun of the last run, and second,
  // because we set the efficiencyFlag so any further aggregation should produce
  // correct results. Also, all operations should be idempotent; running them
  // more than once does no harm.

  // needed to access the DQMStore::save method
  theDQM = nullptr;
  theDQM = Service<DQMStore>().operator->();

  if (runOnEndJob_) {
    makeAllPlots(ibooker, igetter);
  }

  if (!outputFileName_.empty())
    theDQM->save(outputFileName_);
}

void HLTGenValClient::makeAllPlots(DQMStore::IBooker& ibooker, DQMStore::IGetter& igetter) {
  typedef vector<string> vstring;

  // Process wildcard in the sub-directory
  set<string> subDirSet;

  for (vstring::const_iterator iSubDir = subDirs_.begin(); iSubDir != subDirs_.end(); ++iSubDir) {
    string subDir = *iSubDir;

    if (subDir[subDir.size() - 1] == '/')
      subDir.erase(subDir.size() - 1);

    if (TString(subDir).Contains(metacharacters_)) {
      isWildcardUsed_ = true;

      const string::size_type shiftPos = subDir.rfind('/');
      const string searchPath = subDir.substr(0, shiftPos);
      const string pattern = subDir.substr(shiftPos + 1, subDir.length());
      //std::cout << "\n\n\n\nLooking for all subdirs of " << subDir << std::endl;

      findAllSubdirectories(ibooker, igetter, searchPath, &subDirSet, pattern);

    } else {
      subDirSet.insert(subDir);
    }
  }


  for (set<string>::const_iterator iSubDir = subDirSet.begin(); iSubDir != subDirSet.end(); ++iSubDir) {
    const string& dirName = *iSubDir;

    // construct efficiency options automatically
    // we need an EfficOption object per efficiency
    // opt.name
    // opt.title
    // opt.numerator
    // opt.denominator
    // opt.isProfile = false;
    // opt.type = EfficType::efficiency
    auto contents = igetter.getAllContents(dirName);
    for (auto & content : contents) {

      // splitting the input string
      std::stringstream namestream(content->getName());
      std::string namesegment;
      std::vector<std::string> seglist;
      while(std::getline(namestream, namesegment, '_'))
      {
         seglist.push_back(namesegment);
      }

      if(seglist.size() != 4) continue; // this throws out both the "before" hist and most other wrong hists
      if(seglist.at(3) != "after") continue; // ensuring only "after" hists are used

      EfficOption opt;
      opt.name = seglist.at(0) + "_" + seglist.at(1) + "_" + seglist.at(2) + "_pass";
      opt.title = seglist.at(0) + " " + seglist.at(1) + " " + seglist.at(2) + " pass";
      opt.numerator = content->getName();
      opt.denominator = seglist.at(0) + "_" + seglist.at(2) + "_before";
      opt.isProfile = false;
      opt.type = EfficType::efficiency;

      efficOptions_.push_back(opt);

    }

    for (vector<EfficOption>::const_iterator efficOption = efficOptions_.begin(); efficOption != efficOptions_.end();
         ++efficOption) {
      computeEfficiency(ibooker,
                        igetter,
                        dirName,
                        efficOption->name,
                        efficOption->title,
                        efficOption->numerator,
                        efficOption->denominator,
                        efficOption->type,
                        efficOption->isProfile);
    }
  }
}

void HLTGenValClient::computeEfficiency(DQMStore::IBooker& ibooker,
                                         DQMStore::IGetter& igetter,
                                         const string& startDir,
                                         const string& efficMEName,
                                         const string& efficMETitle,
                                         const string& recoMEName,
                                         const string& simMEName,
                                         const EfficType type,
                                         const bool makeProfile) {

  if (!igetter.dirExists(startDir)) {
    if (verbose_ >= 2 || (verbose_ == 1 && !isWildcardUsed_)) {
      LogInfo("HLTGenValClient") << "computeEfficiency() : "
                                  << "Cannot find sub-directory " << startDir << endl;
    }
    return;
  }

  ibooker.cd();

  ME* simME = igetter.get(startDir + "/" + simMEName);
  ME* recoME = igetter.get(startDir + "/" + recoMEName);

  if (!simME) {
    if (verbose_ >= 2 || (verbose_ == 1 && !isWildcardUsed_)) {
      LogInfo("HLTGenValClient") << "computeEfficiency() : "
                                  << "No sim-ME '" << simMEName << "' found\n";
    }
    return;
  }

  if (!recoME) {
    if (verbose_ >= 2 || (verbose_ == 1 && !isWildcardUsed_)) {
      LogInfo("HLTGenValClient") << "computeEfficiency() : "
                                  << "No reco-ME '" << recoMEName << "' found\n";
    }
    return;
  }

  // Treat everything as the base class, TH1

  TH1* hSim = simME->getTH1();
  TH1* hReco = recoME->getTH1();

  if (!hSim || !hReco) {
    if (verbose_ >= 2 || (verbose_ == 1 && !isWildcardUsed_)) {
      LogInfo("HLTGenValClient") << "computeEfficiency() : "
                                  << "Cannot create TH1 from ME\n";
    }
    return;
  }

  string efficDir = startDir;
  string newEfficMEName = efficMEName;
  string::size_type shiftPos;
  if (string::npos != (shiftPos = efficMEName.rfind('/'))) {
    efficDir += "/" + efficMEName.substr(0, shiftPos);
    newEfficMEName.erase(0, shiftPos + 1);
  }
  ibooker.setCurrentFolder(efficDir);

  if (makeProfile) {
    TProfile* efficHist = (hReco->GetXaxis()->GetXbins()->GetSize() == 0)
                              ? new TProfile(newEfficMEName.c_str(),
                                             efficMETitle.c_str(),
                                             hReco->GetXaxis()->GetNbins(),
                                             hReco->GetXaxis()->GetXmin(),
                                             hReco->GetXaxis()->GetXmax())
                              : new TProfile(newEfficMEName.c_str(),
                                             efficMETitle.c_str(),
                                             hReco->GetXaxis()->GetNbins(),
                                             hReco->GetXaxis()->GetXbins()->GetArray());

    efficHist->GetXaxis()->SetTitle(hSim->GetXaxis()->GetTitle());
    efficHist->GetYaxis()->SetTitle(hSim->GetYaxis()->GetTitle());

    for (int i = 1; i <= hReco->GetNbinsX(); i++) {
      const double nReco = hReco->GetBinContent(i);
      const double nSim = hSim->GetBinContent(i);

      if (!std::string(hSim->GetXaxis()->GetBinLabel(i)).empty())
        efficHist->GetXaxis()->SetBinLabel(i, hSim->GetXaxis()->GetBinLabel(i));

      if (nSim == 0 or nReco < 0 or nReco > nSim)
        continue;
      const double effVal = nReco / nSim;
      const double errLo = TEfficiency::ClopperPearson(nSim, nReco, 0.683, false);
      const double errUp = TEfficiency::ClopperPearson(nSim, nReco, 0.683, true);
      const double errVal = (effVal - errLo > errUp - effVal) ? effVal - errLo : errUp - effVal;
      efficHist->SetBinContent(i, effVal);
      efficHist->SetBinEntries(i, 1);
      efficHist->SetBinError(i, std::hypot(effVal, errVal));
    }
    ibooker.bookProfile(newEfficMEName, efficHist);
    delete efficHist;
  }

  else {
    TH1* efficHist = (TH1*)hSim->Clone(newEfficMEName.c_str());
    efficHist->SetTitle(efficMETitle.c_str());

    // Here is where you have trouble --- you need
    // to understand what type of hist you have.

    ME* efficME = nullptr;

    // Parse the class name
    // This works, but there might be a better way
    TClass* myHistClass = efficHist->IsA();
    TString histClassName = myHistClass->GetName();

    if (histClassName == "TH1F") {
      efficME = ibooker.book1D(newEfficMEName, (TH1F*)efficHist);
    } else if (histClassName == "TH2F") {
      efficME = ibooker.book2D(newEfficMEName, (TH2F*)efficHist);
    } else if (histClassName == "TH3F") {
      efficME = ibooker.book3D(newEfficMEName, (TH3F*)efficHist);
    }

    delete efficHist;

    if (!efficME) {
      LogInfo("HLTGenValClient") << "computeEfficiency() : "
                                  << "Cannot book effic-ME from the DQM\n";
      return;
    }

    // Update: 2009-9-16 slaunwhj
    // call the most generic efficiency function
    // works up to 3-d histograms

    generic_eff(hSim, hReco, efficME, type);

    //   const int nBin = efficME->getNbinsX();
    //   for(int bin = 0; bin <= nBin; ++bin) {
    //     const float nSim  = simME ->getBinContent(bin);
    //     const float nReco = recoME->getBinContent(bin);
    //     float eff =0;
    //     if (type=="fake")eff = nSim ? 1-nReco/nSim : 0.;
    //     else eff= nSim ? nReco/nSim : 0.;
    //     const float err = nSim && eff <= 1 ? sqrt(eff*(1-eff)/nSim) : 0.;
    //     efficME->setBinContent(bin, eff);
    //     efficME->setBinError(bin, err);
    //   }
    efficME->setEntries(simME->getEntries());
  }

  // Global efficiency
  if (makeGlobalEffPlot_) {
    ME* globalEfficME = igetter.get(efficDir + "/globalEfficiencies");
    if (!globalEfficME)
      globalEfficME = ibooker.book1D("globalEfficiencies", "Global efficiencies", 1, 0, 1);
    if (!globalEfficME) {
      LogInfo("HLTGenValClient") << "computeEfficiency() : "
                                  << "Cannot book globalEffic-ME from the DQM\n";
      return;
    }
    globalEfficME->setEfficiencyFlag();
    TH1F* hGlobalEffic = globalEfficME->getTH1F();
    if (!hGlobalEffic) {
      LogInfo("HLTGenValClient") << "computeEfficiency() : "
                                  << "Cannot create TH1F from ME, globalEfficME\n";
      return;
    }

    const float nSimAll = hSim->GetEntries();
    const float nRecoAll = hReco->GetEntries();
    float efficAll = 0;
    if (type == EfficType::efficiency || type == EfficType::simpleratio)
      efficAll = nSimAll ? nRecoAll / nSimAll : 0;
    else if (type == EfficType::fakerate)
      efficAll = nSimAll ? 1 - nRecoAll / nSimAll : 0;
    float errorAll = 0;
    if (type == EfficType::simpleratio) {
      if (nSimAll) {
        const float x = nRecoAll / nSimAll;
        errorAll = std::sqrt(1.f / nSimAll * x * (1 + x));
      }
    } else
      errorAll = nSimAll && efficAll < 1 ? sqrt(efficAll * (1 - efficAll) / nSimAll) : 0;

    const int iBin = hGlobalEffic->Fill(newEfficMEName.c_str(), 0);
    hGlobalEffic->SetBinContent(iBin, efficAll);
    hGlobalEffic->SetBinError(iBin, errorAll);
  }
}

//=================================

void HLTGenValClient::findAllSubdirectories(DQMStore::IBooker& ibooker,
                                             DQMStore::IGetter& igetter,
                                             std::string dir,
                                             std::set<std::string>* myList,
                                             const TString& _pattern = TString("")) {
  TString pattern = _pattern;
  if (!igetter.dirExists(dir)) {
    LogError("HLTGenValClient") << " HLTGenValClient::findAllSubdirectories ==> Missing folder " << dir << " !!!";
    return;
  }
  if (pattern != "") {
    if (pattern.Contains(nonPerlWildcard_))
      pattern.ReplaceAll("*", ".*");
    TPRegexp regexp(pattern);
    ibooker.cd(dir);
    vector<string> foundDirs = igetter.getSubdirs();
    for (vector<string>::const_iterator iDir = foundDirs.begin(); iDir != foundDirs.end(); ++iDir) {
      TString dirName = iDir->substr(iDir->rfind('/') + 1, iDir->length());
      if (dirName.Contains(regexp))
        findAllSubdirectories(ibooker, igetter, *iDir, myList);
    }
  }
  //std::cout << "Looking for directory " << dir ;
  else if (igetter.dirExists(dir)) {
    //std::cout << "... it exists! Inserting it into the list ";
    myList->insert(dir);
    //std::cout << "... now list has size " << myList->size() << std::endl;
    ibooker.cd(dir);
    findAllSubdirectories(ibooker, igetter, dir, myList, "*");
  } else {
    //std::cout << "... DOES NOT EXIST!!! Skip bogus dir" << std::endl;

    LogInfo("HLTGenValClient") << "Trying to find sub-directories of " << dir << " failed because " << dir
                                << " does not exist";
  }
  return;
}

void HLTGenValClient::generic_eff(TH1* denom, TH1* numer, MonitorElement* efficiencyHist, const EfficType type) {
  for (int iBinX = 1; iBinX < denom->GetNbinsX() + 1; iBinX++) {
    for (int iBinY = 1; iBinY < denom->GetNbinsY() + 1; iBinY++) {
      for (int iBinZ = 1; iBinZ < denom->GetNbinsZ() + 1; iBinZ++) {
        int globalBinNum = denom->GetBin(iBinX, iBinY, iBinZ);

        float numerVal = numer->GetBinContent(globalBinNum);
        float denomVal = denom->GetBinContent(globalBinNum);

        float effVal = 0;

        // fake eff is in use
        if (type == EfficType::fakerate) {
          effVal = denomVal ? (1 - numerVal / denomVal) : 0;
        } else {
          effVal = denomVal ? numerVal / denomVal : 0;
        }

        float errVal = 0;
        if (type == EfficType::simpleratio) {
          //          errVal = denomVal ? 1.f/denomVal*effVal*(1+effVal) : 0;
          float numerErr = numer->GetBinError(globalBinNum);
          float denomErr = denom->GetBinError(globalBinNum);
          float denomsq = denomVal * denomVal;
          errVal = denomVal ? sqrt(pow(1.f / denomVal * numerErr, 2.0) + pow(numerVal / denomsq * denomErr, 2)) : 0;
        } else {
          errVal = (denomVal && (effVal <= 1)) ? sqrt(effVal * (1 - effVal) / denomVal) : 0;
        }

        LogDebug("HLTGenValClient") << "(iBinX, iBinY, iBinZ)  = " << iBinX << ", " << iBinY << ", " << iBinZ
                                     << "), global bin =  " << globalBinNum << "eff = " << numerVal << "  /  "
                                     << denomVal << " =  " << effVal << " ... setting the error for that bin ... "
                                     << endl
                                     << endl;

        efficiencyHist->setBinContent(globalBinNum, effVal);
        efficiencyHist->setBinError(globalBinNum, errVal);
        efficiencyHist->setEfficiencyFlag();
      }
    }
  }

  //efficiencyHist->setMinimum(0.0);
  //efficiencyHist->setMaximum(1.0);
}

DEFINE_FWK_MODULE(HLTGenValClient);

/* vim:set ts=2 sts=2 sw=2 expandtab: */
