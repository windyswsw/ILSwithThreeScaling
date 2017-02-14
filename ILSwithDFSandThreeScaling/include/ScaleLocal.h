void ScaleLocal(FullSol* InitialFullSolution, CommonList* CurCommonLists, float LinksDelay, int ServerCap, int InPathLinks, int PercentageOfPoliciesToChangeInPerturbation, int TerminationConditionRounds, ofstream &myfile2, ofstream &myfile3, bool vertical, bool unified, bool incremental){

	FullSol basesol;
	basesol.CurLists = InitialFullSolution->CurLists;
	basesol.ObjVal = InitialFullSolution->ObjVal;
	basesol.InitialObjVal = InitialFullSolution->ObjVal;
	basesol.VectorOfPartialSols = InitialFullSolution->VectorOfPartialSols;
	basesol.CPULeft = InitialFullSolution->CPULeft;

    char filenamea[256] = {0};
	strcpy(filenamea,"TimeScale.csv");
	ofstream myfilea;
	myfilea.open (filenamea, ios::out | ios::app);

	/*typedef std::chrono::high_resolution_clock Time1;
	typedef std::chrono::microseconds ms;*/

	int Round = 0;

	while(CurCommonLists->FullChangeListCur.size()>0){

		ChangedPolicies TrackChangedPolicies;
		vector<Policy> OriginalPolicyList = CurCommonLists->PolicyList;

		basesol.CurLists = InitialFullSolution->CurLists;
		basesol.ObjVal = InitialFullSolution->ObjVal;
		basesol.VectorOfPartialSols = InitialFullSolution->VectorOfPartialSols;

		RoundChangeInfo RoundChangeNow = RoundChange(CurCommonLists);
		int TotPoliciesChanging = RoundChangeNow.RoundChangeList.size();
		float FloatNumberOfPoliciesToChangeInPerturbation = (float) TotPoliciesChanging * (float) PercentageOfPoliciesToChangeInPerturbation / 100;
		int NumberOfPoliciesToChangeInPerturbation = floor(FloatNumberOfPoliciesToChangeInPerturbation);
		if( NumberOfPoliciesToChangeInPerturbation <= 0){
			NumberOfPoliciesToChangeInPerturbation = 1;
		}

		for(size_t d=0; d<RoundChangeNow.RoundChangeList.size(); d++){

			ChangeInfo CurrentChange;
			CurrentChange.AR = RoundChangeNow.RoundChangeList[d].AR;
			CurrentChange.ScaleBW = RoundChangeNow.RoundChangeList[d].ScaleBW;
			CurrentChange.polToChange = RoundChangeNow.RoundChangeList[d].polToChange;

			int policyC = RoundChangeNow.RoundChangeList[d].polToChange;
			int ScaleNextBW = RoundChangeNow.RoundChangeList[d].ScaleBW;

			int TotalCurrentBW = InitialFullSolution->VectorOfPartialSols[policyC].AllocatedBW; // total bw distributed to parent and children
			for( int f=0; f< CurCommonLists->PolicyList[policyC].Children.size(); f++){
				int ChildID1 = CurCommonLists->PolicyList[policyC].Children[f];
				int ChildBW1 = InitialFullSolution->VectorOfPartialSols[ChildID1].AllocatedBW;
				TotalCurrentBW = TotalCurrentBW + ChildBW1;
			}

			int OriginaBW =  CurCommonLists->PolicyList[policyC].OriginalBW;

			int ScaleBW = 0;
			if( CurrentChange.AR == "A"){
				ScaleBW = ScaleNextBW  - TotalCurrentBW;
				CurrentChange.ScaleBW = ScaleBW;
			}
			else if( CurrentChange.AR == "D"){
				if(ScaleNextBW <= OriginaBW){
					ScaleNextBW = OriginaBW;
				}
				ScaleBW = TotalCurrentBW - ScaleNextBW;
				CurrentChange.ScaleBW = ScaleBW;
			}

			bool AffordableN = true;
			int BWToCheck = 0;
			int BWToCheckTemp = 0;
			int CurrentBW = 0;

			if(CurrentChange.AR == "A"){
				if((vertical == true) || (unified == true)){
					BWToCheck = CurCommonLists->PolicyList[policyC].NextBW + ScaleBW;
					BWToCheckTemp = BWToCheck;
					CurrentBW = CurCommonLists->PolicyList[policyC].CurrentBW;
				}
				else if(incremental == true){
					BWToCheck = ScaleBW;
					BWToCheckTemp = ScaleBW;
					CurrentBW = CurCommonLists->PolicyList[policyC].CurrentBW;
				}

				AffordableN = false;
				bool NoPointOfScaling = false;
				while( (AffordableN == false) && ( NoPointOfScaling == false)){
					AffordableN = Affordable(CurCommonLists->PolicyList[policyC], ServerCap, CurCommonLists->NFInfo, LinksDelay, InPathLinks, BWToCheck);
					if(AffordableN == false){
						int nextTotBW = NextBW(InitialFullSolution, policyC, CurCommonLists, BWToCheck);
						if( (nextTotBW <= CurrentBW) || (nextTotBW > BWToCheckTemp)){
							NoPointOfScaling = true;
						}
						else{
							BWToCheck = nextTotBW;
						}
					}
				}

				if (AffordableN == false){
					ScaleInfo ChangeReject;
					ChangeReject.AR = CurrentChange.AR;
					ChangeReject.pol = CurrentChange.polToChange;

					int TotalCurrentBW = InitialFullSolution->VectorOfPartialSols[policyC].AllocatedBW; // total bw distributed to parent and children
					for( int f=0; f< CurCommonLists->PolicyList[policyC].Children.size(); f++){
						int ChildID1 = CurCommonLists->PolicyList[policyC].Children[f];
						int ChildBW1 = InitialFullSolution->VectorOfPartialSols[ChildID1].AllocatedBW;
						TotalCurrentBW = TotalCurrentBW + ChildBW1;
					}

					int TotalNextBW = CurCommonLists->PolicyList[policyC].NextBW + ScaleBW;
					int BWIncrease = TotalNextBW - TotalCurrentBW;

					auto it6112 = InitialFullSolution->ScaleRejected.find(policyC);
					if( it6112 != InitialFullSolution->ScaleRejected.end()){
						it6112->second.BWChange = BWIncrease;
					}
					else{
						ChangeReject.BWChange = BWIncrease;
						InitialFullSolution->ScaleRejected.insert(std::pair<short int,ScaleInfo>(policyC,ChangeReject));
					}
				}

			}

			CurCommonLists->PolicyList[policyC].CurrentBW = InitialFullSolution->VectorOfPartialSols[policyC].AllocatedBW; // makes allocated bw as the policy's current bw
			CurCommonLists->PolicyList[policyC].NextBW = ScaleNextBW;
			CountTotalCPUs(InitialFullSolution, CurCommonLists);

			if (AffordableN == true){
				if(vertical == true){
					VerticalScale(InitialFullSolution, CurCommonLists, LinksDelay, ServerCap, InPathLinks, myfile2, CurrentChange, &TrackChangedPolicies);
				}
				else if(unified == true){
					UnifiedScale(InitialFullSolution, CurCommonLists, LinksDelay,  ServerCap, InPathLinks,myfile2, CurrentChange, &TrackChangedPolicies);
				}
				else if(incremental == true){
					IncrementalScale(InitialFullSolution, CurCommonLists, LinksDelay, ServerCap, InPathLinks, myfile2, CurrentChange, &TrackChangedPolicies);
				}
			}

			ScaleGlobalObjectValueCalculation(InitialFullSolution, CurCommonLists);
		}
		
		ScaleGlobalObjectValueCalculation(InitialFullSolution, CurCommonLists);

		PrintSummaryScale(Round, InitialFullSolution, myfile2, 'I', CurCommonLists, &TrackChangedPolicies);
		PrintFullAllocation(Round, 'I', InitialFullSolution, myfile3);
		PrintAllocationLine(myfile3);

		ScaleRepeatProcedure(InitialFullSolution,  CurCommonLists, NumberOfPoliciesToChangeInPerturbation, LinksDelay, ServerCap, InPathLinks, myfile2,myfile3, TerminationConditionRounds, vertical, unified, incremental, &TrackChangedPolicies);
		
		ScaleGlobalObjectValueCalculation(InitialFullSolution, CurCommonLists);

		PrintSummaryScale(Round, InitialFullSolution, myfile2, 'A', CurCommonLists, &TrackChangedPolicies);
		PrintNewLine(myfile2);
		PrintFullAllocation(Round, 'A', InitialFullSolution, myfile3);
		PrintAllocationLine(myfile3);

		InitialFullSolution->ScaleAccepted.clear();

		int SvrChanges = 0;
		int LinkChanges = 0;

		TrackChangedPolicies.CurrentlyChangedPolicies.clear();
		TrackChangedPolicies.OriginalPolicies.clear();
	}

	Round ++;

}
