
void InitialPolicyImplementation(FullSol* InitialFullSolution, CommonList* CurCommonLists, float LinksDelay, ofstream &myfile1, ofstream &myfile2,ofstream &myfile3, int InitialNoOfPoliciesToChangeInPerturbation, int InitialTerminationConditionRounds ){

	for(int p=0; p<=CurCommonLists->PolicyList.size()-1; p++){

		bool sucess = false;
		int startNF = 0; // OnePlicyMove start from 1st NF of policy

		PartialSol SelectedPolicyOriginal;
		PartialSol SelectedPolicyTemp;
		int PolicyID = p;
		int TotNFs = CurCommonLists->PolicyList[PolicyID].NFIDs.size();

		if(TotNFs <= InitialFullSolution->CPULeft){

			SelectedPolicyTemp.HaveResources = false;
			SelectedPolicyTemp.accepted = false;
			SelectedPolicyTemp.PolicyID = PolicyID;
			SelectedPolicyTemp.Splits = 0;
			SelectedPolicyTemp.Delay = 0;
			SelectedPolicyTemp.AllocatedBW = CurCommonLists->PolicyList[PolicyID].OriginalBW;
			SelectedPolicyOriginal = SelectedPolicyTemp;

			int BW = CurCommonLists->PolicyList[p].OriginalBW;

			sucess = DFSLocalSearchWithOnePolicyMove(InitialFullSolution, CurCommonLists, PolicyID, startNF, BW, LinksDelay, &SelectedPolicyTemp, &SelectedPolicyOriginal);

			if (sucess == false){ // didnt find an improved sol
				InitialFullSolution->VectorOfPartialSols.push_back(SelectedPolicyOriginal); // push the empty VMMap
				InitialFullSolution->NotAccepted.insert(std::pair<short int,PartialSol>(PolicyID,SelectedPolicyOriginal)); // not accepted
			}	
			else{ // deadline satisfied, obj should be increased and solution should be accepted
				GlobalObjectValueUpdateForIncrease(InitialFullSolution, PolicyID, CurCommonLists);
				InitialFullSolution->Accepted.insert(std::pair<short int,PartialSol>(PolicyID,InitialFullSolution->VectorOfPartialSols[PolicyID]));
			}	

			//PrintSummaryGlobal(p, InitialFullSolution, myfile2, 'I', CurCommonLists);
			//PrintFullAllocation(p, 'I', InitialFullSolution, myfile3);
			//PrintAllocationLine(myfile3);
			InitialRepeatProcedure(InitialFullSolution,  CurCommonLists, InitialNoOfPoliciesToChangeInPerturbation, LinksDelay, myfile2,myfile3, InitialTerminationConditionRounds);
            //PrintSummaryGlobal(p, InitialFullSolution, myfile2, 'A', CurCommonLists);
			//PrintNewLine(myfile2);
			//PrintFullAllocation(p, 'A', InitialFullSolution, myfile3);
			//PrintAllocationLine(myfile3);
		}
	}
}

