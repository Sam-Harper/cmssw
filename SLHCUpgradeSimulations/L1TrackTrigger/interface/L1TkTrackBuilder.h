/// ////////////////////////////////////////
/// Stacked Tracker Simulations          ///
///                                      ///
/// Nicola Pozzobon,  UNIPD              ///
///                                      ///
/// 2011, September                      ///
/// ////////////////////////////////////////

#ifndef L1TK_TRACK_BUILDER_H
#define L1TK_TRACK_BUILDER_H

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

//#include "SLHCUpgradeSimulations/Utilities/interface/StackedTrackerDetId.h"
#include "SLHCUpgradeSimulations/L1TrackTrigger/interface/TrackingAlgorithm.h"
#include "SLHCUpgradeSimulations/L1TrackTrigger/interface/TrackingAlgorithmRecord.h"
#include "classNameFinder.h"

#include <memory>
#include <map>
#include <vector>

/** ************************ **/
/**                          **/
/**   DECLARATION OF CLASS   **/
/**                          **/
/** ************************ **/

template<  typename T  >
class L1TkTrackBuilder : public edm::EDProducer
{
  public:
/*
    typedef L1TkStub< T >                                     L1TkStubType;
    typedef edm::Ptr< L1TkStubType >                                       L1TkStubPtrType;
    typedef std::vector< L1TkStub< T > >                      L1TkStubCollectionType;
    typedef L1TkTracklet< T >                                 L1TkTrackletType;
    typedef edm::Ptr< L1TkTrackletType >                                   L1TkTrackletPtrType;
    typedef std::vector< L1TkTrackletType >                                L1TkTrackletCollectionType;
    typedef L1TkTrack< T >                                    L1TkTrackType;
    typedef std::vector< L1TkTrackType >                                   L1TkTrackCollectionType;
    typedef std::map< unsigned int, std::vector< L1TkStubPtrType > >       L1TkStubMapType;
    typedef std::map< unsigned int, std::vector< L1TkTrackletPtrType > >   L1TkTrackletMapType;
    typedef std::set< std::pair< unsigned int , L1TkStubPtrType > >        L1TkTrackletMap;
*/

    /// Constructor
    explicit L1TkTrackBuilder( const edm::ParameterSet& iConfig );

    /// Destructor;
    ~L1TkTrackBuilder();

  private:
    /// Data members
    /// Geometry
//    edm::ESHandle< StackedTrackerGeometry >        StackedTrackerGeomHandle;
    const StackedTrackerGeometry            *theStackedTracker;
//    StackedTrackerGeometry::StackContainerIterator StackedTrackerIterator;
    /// Tracking algorithm
    edm::ESHandle< TrackingAlgorithm< T > > TrackingAlgoHandle;
    edm::InputTag                                        L1TkStubsInputTag;
    /// Tracklet Seeds?
//    bool                                                 wantTrackletSeeds;




//    edm::InputTag                                          L1TkTrackletInputTag;

//    bool                                                   useAlsoSeedVtx;
//    bool                                                   doHelixFit;
//    bool                                                   removeDuplicates;
//    std::vector< unsigned int >                            allowedDoubleStacks;

    /// Magnetic field
//    double mMagneticFieldStrength;

    /// Other stuff

    /// ///////////////// ///
    /// MANDATORY METHODS ///
    virtual void beginRun( edm::Run& run, const edm::EventSetup& iSetup );
    virtual void endRun( edm::Run& run, const edm::EventSetup& iSetup );
    virtual void produce( edm::Event& iEvent, const edm::EventSetup& iSetup );

}; /// Close class

/** ***************************** **/
/**                               **/
/**   IMPLEMENTATION OF METHODS   **/
/**                               **/
/** ***************************** **/

/// Constructors
template< typename T >
L1TkTrackBuilder< T >::L1TkTrackBuilder( const edm::ParameterSet& iConfig )
{
  produces< std::vector< L1TkTrack< T > > >();
  L1TkStubsInputTag = iConfig.getParameter< edm::InputTag >("L1TkStubsBricks");

  /// The kind of bricks
//  L1TkTrackletInputTag = iConfig.getParameter< edm::InputTag >("L1TkTrackletSeed"); /// The kind of seed
//  useAlsoSeedVtx       = iConfig.getParameter< bool >("UseAlsoSeedVertex");
//  doHelixFit           = iConfig.getParameter< bool >("DoHelixFit");
//  removeDuplicates     = iConfig.getParameter< bool >("RemoveDuplicates");
//  allowedDoubleStacks  = iConfig.getParameter< std::vector< unsigned int > >("SeedDoubleStacks");
}

/// Destructor
template< typename T >
L1TkTrackBuilder< T >::~L1TkTrackBuilder() {}

/// Begin run
template< typename T >
void L1TkTrackBuilder< T >::beginRun( edm::Run& run, const edm::EventSetup& iSetup )
{
  /// Get the geometry references
  edm::ESHandle< StackedTrackerGeometry > StackedTrackerGeomHandle;
  iSetup.get< StackedTrackerGeometryRecord >().get( StackedTrackerGeomHandle );
  theStackedTracker = StackedTrackerGeomHandle.product();

  /// Get the tracking algorithm 
  iSetup.get< TrackingAlgorithmRecord >().get( TrackingAlgoHandle );
  /// Print some information when loaded
  std::cout  << "L1TkTrackBuilder<" << templateNameFinder<T>() << "> loaded modules:"
             << "\n\tTrackingAlgorithm:\t" << TrackingAlgoHandle->AlgorithmName()
             << std::endl;

  /// Get magnetic field
//  edm::ESHandle<MagneticField> magnet;
//  iSetup.get<IdealMagneticFieldRecord>().get(magnet);
//  mMagneticFieldStrength = magnet->inTesla(GlobalPoint(0,0,0)).z();

  /// Calculate B rounded to 4.0 or 3.8
//  mMagneticFieldStrength = (floor(mMagneticFieldStrength*10.0 + 0.5))/10.0;
}

/// End run
template< typename T >
void L1TkTrackBuilder< T >::endRun( edm::Run& run, const edm::EventSetup& iSetup ) {}

/// Implement the producer
template< typename T >
void L1TkTrackBuilder< T >::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  /// Prepare output
  /// The temporary collection is used to store tracks
  /// before removal of duplicates
  std::vector< L1TkTrack< T > > tempTrackCollection;
  tempTrackCollection.clear();
  std::auto_ptr< std::vector< L1TkTrack< T > > > L1TkTracksForOutput( new std::vector< L1TkTrack< T > > );

  /// Get the Stubs already stored away
  edm::Handle< std::vector< L1TkStub< T > > > L1TkStubHandle;
  iEvent.getByLabel( L1TkStubsInputTag, L1TkStubHandle);

  std::vector< L1TkTrack< T > > theseSeeds;
  TrackingAlgoHandle->CreateSeeds( theseSeeds, L1TkStubHandle );










/*
  /// Get the Stubs and Tracklets already stored away
  edm::Handle< L1TkStubCollectionType >     L1TkStubHandleBricks;
  edm::Handle< L1TkTrackletCollectionType > L1TkTrackletHandleSeed;
  iEvent.getByLabel( L1TkStubsInputTag, L1TkStubHandleBricks );
  iEvent.getByLabel( L1TkTrackletInputTag, L1TkTrackletHandleSeed );
  
  /// Prepare output
  /// The temporary collection is used to store tracks
  /// before removal of duplicates
  L1TkTrackCollectionType tempTrackCollection;
  tempTrackCollection.clear();
  std::auto_ptr< L1TkTrackCollectionType > L1TkTracksForOutput( new L1TkTrackCollectionType );

  /// Map the Tracklets per DoubleStack
  /// 0-01 1-23 2-45 3-67 4-89  
  L1TkTrackletMapType SeedTracklets;
  for ( unsigned int i = 0; i != L1TkTrackletHandleSeed->size(); ++i )
  {
    /// Select only Hermetic Tracklets
    StackedTrackerDetId stDetId0( L1TkTrackletHandleSeed->at(i).getStubRef(0)->getDetId() );
    StackedTrackerDetId stDetId1( L1TkTrackletHandleSeed->at(i).getStubRef(1)->getDetId() );
    unsigned int stack0 = stDetId0.iLayer();
    unsigned int stack1 = stDetId1.iLayer();

    unsigned int doubleStack = stack0/2;
    if ( doubleStack != (stack1-1)/2 ) continue; /// Reject bad formed Tracklets
 
    unsigned int ladder0 = stDetId0.iPhi();
    unsigned int ladder1 = stDetId1.iPhi();
    if ( ladder0 != ladder1 ) continue; /// Reject bad formed Tracklets

    /// This is if we want only a subset of seed double stacks
    bool skip = true;
    for ( unsigned int t = 0; t < allowedDoubleStacks.size(); t++ )
      if ( allowedDoubleStacks.at(t) == doubleStack )
        skip = false;
    if ( allowedDoubleStacks.size() != 0 && skip ) continue;

    SeedTracklets[ doubleStack ].push_back( L1TkTrackletPtrType( L1TkTrackletHandleSeed, i ) );
  }

  /// Map the stubs per DoubleStack
  /// The rule is that if DoubleStack is "j"
  /// the corresponding map is built with
  /// Stubs in DoubleStacks different from "j"
  L1TkStubMapType BrickStubs;
  for ( unsigned int i = 0; i != L1TkStubHandleBricks->size(); ++i )
  {

    StackedTrackerDetId stDetId( L1TkStubHandleBricks->at(i).getDetId() );

    unsigned int stack = stDetId.iLayer();
    unsigned int doubleStack;
    if ( stack%2 == 0 ) doubleStack = stack/2;
    else doubleStack = (stack-1)/2;

    /// This is if we want only a subset of seed double stacks
    bool skip = true;
    for ( unsigned int t = 0; t < allowedDoubleStacks.size(); t++ )
      if ( allowedDoubleStacks.at(t) == doubleStack )
        skip = false;
    if ( allowedDoubleStacks.size() != 0 && skip ) continue;

    for ( unsigned int q = 0; q < 5; q++ )
    {
      if ( doubleStack != q) BrickStubs[ q ].push_back( L1TkStubPtrType( L1TkStubHandleBricks , i ) );
    }
  }

  /// Loop over DoubleStacks
  /// Loop over Seeds withing DoubleStack and propagate them
  for ( unsigned int j = 0; j < 5; j++ )
  {
    /// Find both the bricks and the seeds corresponding
    /// to the current DoubleStack
    std::vector< L1TkTrackletPtrType > theSeeds  = SeedTracklets[j];
    std::vector< L1TkStubPtrType >     theBricks = BrickStubs[j];

    /// We have the Seed and the Bricks here
    /// Loop over the Seeds and propagate each of them
    for ( unsigned int k = 0; k < theSeeds.size(); k++ )
    {
      /// Get all the candidates from this seed
      /// including combinatorics
      std::vector< L1TkTrack< T > > allCandTracks;
      allCandTracks.clear();
      allCandTracks = TrackingAlgoHandle->PropagateSeed( theSeeds.at(k), theBricks );

      /// Loop on candidates
      /// just push back in the global storage of L1Tracks
      for ( unsigned int h = 0; h < allCandTracks.size(); h++ )
      {
        if ( iEvent.isRealData() == false )
          allCandTracks.at(h).checkSimTrack();
        allCandTracks.at(h).fitTrack( mMagneticFieldStrength, useAlsoSeedVtx, doHelixFit );
        if ( removeDuplicates )
          tempTrackCollection.push_back( allCandTracks.at(h) );
        else
          L1TkTracksForOutput->push_back( allCandTracks.at(h) );
      } /// End of loop on candidates

    } /// End of loop on seeds
  } /// End of loop on Superlayers

  /// Define the minimum number to define duplicate L1TkTracks
  /// 2 means a tracklet, which always points to the same window
  /// no matter the way it is projected.
  unsigned int minForDupl = 2;
  unsigned int plusVtx = 0;
  if ( useAlsoSeedVtx ) plusVtx = 1;

  /// Implement Duplicate Removal
  if ( removeDuplicates )
  {
    /// Loop over pairs Candidate L1TkTracks
    if ( tempTrackCollection.size() != 0 )
    {
      for ( unsigned int tk = 0; tk < tempTrackCollection.size()-1; tk++ )
      {
        L1TkStubCollectionType tkStubs0 = tempTrackCollection.at(tk).getStubs();
        unsigned int tkNum0 = tkStubs0.size();
        /// This prevents from reading already deleted tracks
        if ( tkNum0 == 0 ) continue;
        unsigned int tkSeed0 = tempTrackCollection.at(tk).getSeedDoubleStack();

        /// Nested loop
        if ( tempTrackCollection.size() == 1 ) continue;
        for ( unsigned int tkk = tk+1; tkk < tempTrackCollection.size(); tkk++ )
        {
          L1TkStubCollectionType tkStubs1 = tempTrackCollection.at(tkk).getStubs();
          unsigned int tkNum1 = tkStubs1.size();
          /// This prevents from reading already deleted tracks
          if ( tkNum1 == 0 ) continue;
          unsigned int tkSeed1 = tempTrackCollection.at(tkk).getSeedDoubleStack();

          unsigned int numShared = 0;
          for ( unsigned int st = 0; st < tkStubs0.size(); st++ )
          {
            if ( numShared >= minForDupl ) continue;
            for ( unsigned int stt = 0; stt < tkStubs1.size(); stt++ )
            {
              if ( numShared >= minForDupl ) continue;
              if ( tkStubs0.at(st).getClusterPtr(0) == tkStubs1.at(stt).getClusterPtr(0) &&
                   tkStubs0.at(st).getClusterPtr(1) == tkStubs1.at(stt).getClusterPtr(1) )
                numShared++;
            }
          } /// End of check if they are shared or not

          /// Skip to next pair if they are different
          if ( numShared < minForDupl ) continue;

          /// Reject the one with the outermost seed
          if ( tkSeed1 > tkSeed0 )
            tempTrackCollection.at(tkk) = L1TkTrackType();
          else if ( tkSeed1 < tkSeed0 )
            tempTrackCollection.at(tk) = L1TkTrackType();
          else
          {
            /// If they are from seeds in the same layer,
            /// use goodness of fit discrimination
            if ( tempTrackCollection.at(tkk).getChi2RPhi() / tkNum1 > tempTrackCollection.at(tk).getChi2RPhi() / tkNum0 )
              tempTrackCollection.at(tkk) = L1TkTrackType();
            else if ( tempTrackCollection.at(tkk).getChi2RPhi() / tkNum1 < tempTrackCollection.at(tk).getChi2RPhi() / tkNum0 )
              tempTrackCollection.at(tk) = L1TkTrackType();
            else
              std::cerr << "*** I CAN'T BELIEVE IT!! ***" << std::endl;
          }
        } /// End of Nested loop
      } /// End of Loop over pairs Candidate L1TkTracks

      /// Pass only non-erased elements
      for ( unsigned int tk = 0; tk < tempTrackCollection.size(); tk++ )
      {
        if ( tempTrackCollection.at(tk).getStubs().size() != 0 )
          L1TkTracksForOutput->push_back( tempTrackCollection.at(tk) );
      }
    } /// End of if ( tempTrackCollection.size() != 0 ) {
  } /// End of Implement Duplicate Removal
*/

  /// Store
//  if ( doHelixFit && useAlsoSeedVtx )
//    iEvent.put( L1TkTracksForOutput, "Level1TracksHelFitVtxYes" );
//  else if ( useAlsoSeedVtx )
//    iEvent.put( L1TkTracksForOutput, "Level1TracksStdFitVtxYes" );
//  else if ( doHelixFit )
//    iEvent.put( L1TkTracksForOutput, "Level1TracksHelFitVtxNo" );
//  else
//    iEvent.put( L1TkTracksForOutput, "Level1TracksStdFitVtxNo" );

}

#endif

