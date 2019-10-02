```


1. Push changes:
    a) git add .
    b) git commit
    c) git push -u my-cmssw SohamBhattacharya/HGCal_ele-reco:HGCal_ele-reco 

2. Run ecalDrivenGsfElectronsFromTICL:
    from MyModules.Test.ecalDrivenGsfElectronsFromTICL_cff import ecalDrivenGsfElectronsFromTICL_customizeProcess
    process = ecalDrivenGsfElectronsFromTICL_customizeProcess(process, onReco = True)
    <Add process.ecalDrivenGsfElectronsFromTICL_step to the schedule>

3. Delete from remote branch ONLY:
    git rm --cached <file>
    git rm -r --cached <directory>
    <1b>
    <1c>
    

```
