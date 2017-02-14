void PerturbationVerticalScale(FullSol* FullSolution, CommonList* CurCommonLists, short int NoOfPoliciesToChangeInPerturbation, float LinksDelay, ofstream &myfile1, ofstream &myfile2,  ChangedPolicies* TrackChangedPolicies ){
	
	for (int k = 0; k< NoOfPoliciesToChangeInPerturbation; k++){

		if(FullSolution->ScaleAccepted.size() > 0){

			int PolicyPosition = 0;

			if(FullSolution->ScaleAccepted.size() > 1){
				int TotPolicy = FullSolution->ScaleAccepted.size() -1;
				PolicyPosition = rand() % TotPolicy + 1;
			}

			int PolicyID = 0;
			ScaleInfo ChangeInfo;
			bool success = false;
			bool PolicySelected = false;
			int startNF = 0;

			PolicyID = FullSolution->ScaleAccepted[PolicyPosition];

			int OriginalBW = CurCommonLists->PolicyList[PolicyID].OriginalBW;
			int CurrentBW = CurCommonLists->PolicyList[PolicyID].CurrentBW;
			int GivenNewBW = CurCommonLists->PolicyList[PolicyID].NextBW;
			int NewBW = ReturnMaxEnteringBWForAPolicy(CurCommonLists, PolicyID,GivenNewBW);

			PartialSol SelectedPolicyTemp;
			SelectedPolicyTemp.HaveResources = false;
			SelectedPolicyTemp.accepted = false;
			SelectedPolicyTemp.PolicyID = PolicyID;
			SelectedPolicyTemp.Splits = 0;
			SelectedPolicyTemp.Delay = 0;
	
			PartialSol SelectedPolicyOriginal;
			SelectedPolicyOriginal.accepted = FullSolution->VectorOfPartialSols[PolicyID].accepted;
			SelectedPolicyOriginal.Delay = FullSolution->VectorOfPartialSols[PolicyID].Delay;
			SelectedPolicyOriginal.HaveResources = FullSolution->VectorOfPartialSols[PolicyID].HaveResources;
			SelectedPolicyOriginal.PolicyID = FullSolution->VectorOfPartialSols[PolicyID].PolicyID;
			SelectedPolicyOriginal.Splits = FullSolution->VectorOfPartialSols[PolicyID].Splits;
			SelectedPolicyOriginal.VMsForAPolicy = FullSolution->VectorOfPartialSols[PolicyID].VMsForAPolicy;
			SelectedPolicyOriginal.AllocatedBW = FullSolution->VectorOfPartialSols[PolicyID].AllocatedBW;
			int AllocatedBW = FullSolution->VectorOfPartialSols[PolicyID].AllocatedBW;

			RemovePolicyInRepeatedprocedure(FullSolution, CurCommonLists, PolicyID); // remove the allocated configs
			ScaleObjectValueUpdateForDecrease(FullSolution, PolicyID, CurCommonLists, AllocatedBW); // remove obj
			success = DFSVerticalScaling(FullSolution, CurCommonLists, PolicyID, startNF, CurrentBW, LinksDelay, &SelectedPolicyTemp, &SelectedPolicyOriginal, TrackChangedPolicies );
			FullSolution->VectorOfPartialSols[PolicyID].AllocatedBW = CurrentBW; // (total bw: current + new)
			ScaleObjectValueUpdateForIncrease(FullSolution, PolicyID, CurCommonLists,CurrentBW); // add obj for total (original + new)

			swap(FullSolution->ScaleAccepted[PolicyPosition], FullSolution->ScaleAccepted.back());
			FullSolution->ScaleAccepted.pop_back(); // remove from scale accepted

			ScaleInfo ChangeReject;
			ChangeReject.pol = PolicyID;
			ChangeReject.AR = "A";
			ChangeReject.BWChange = NewBW - CurrentBW;  

			auto it1 = FullSolution->ScaleRejected.find(PolicyID);
			if (it1 != FullSolution->ScaleRejected.end()){ // if its already in scale rejected, update with new info
				it1->second = ChangeReject;
			}
			else{ // if not, insert
				FullSolution->ScaleRejected.insert(std::pair<short int,ScaleInfo>(PolicyID,ChangeReject));
			}
			
		}

	}
}
