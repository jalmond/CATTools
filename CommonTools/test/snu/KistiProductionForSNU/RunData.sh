if [[ $CATVERSION == "" ]]; then
    echo "Set CATVERSION"
fi

uptodate=false
while read line
do
    if [[ $line == *$CATVERSION* ]];
    then
	if [[ $line != "" ]];
	then
	    echo $line
	    uptodate=true
	fi
    fi
    
done < $CMSSW_BASE"/src/CATTools/CatAnalyzer/scripts/catGetDatasetInfo" 

if [[ $uptodate == "false" ]];
then
    echo "You need to update code. You are running "$CATVERSION" yet this is not found in "$CMSSW_BASE"/src/CATTools/CatAnalyzer/scripts/catGetDatasetInfo"
    exit 1
fi


check=$(ssh jalmond@cms3.snu.ac.kr ls /data1/LQAnalyzer_rootfiles_for_analysis/CATAnalysis/)    

if [[ $check == *"dataset_"$CATVERSION* ]];
then
    echo "Dataset file has already been copied"
    else 
    echo "Running for the first time with this new tag. making dataset directory and copying to SNU"
    catGetDatasetInfo $CATVERSION
    scp -r dataset jalmond@cms3.snu.ac.kr:/data1/LQAnalyzer_rootfiles_for_analysis/CATAnalysis/dataset_$CATVERSION
fi


python RunNtupleMaker.py
