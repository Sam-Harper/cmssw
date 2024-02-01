def moveToEgammaTriplets(process):
    delattr(process,"hltElePixelHitDoublets")
    delattr(process,"hltElePixelSeedsDoublets")
    delattr(process,"hltElePixelSeedsCombined")
    process.hltEgammaElectronPixelSeeds.initialSeeds = "hltElePixelSeedsTriplets"
    return process

