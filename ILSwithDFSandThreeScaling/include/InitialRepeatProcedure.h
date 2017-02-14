void InitialRepeatProcedure(FullSol* FullSolution, CommonList* CurCommonLists, int NoOfNFsToChangeInPerturbation, float LinksDelay, ofstream &myfile2, ofstream &myfile3, int TerminationConditionRounds){

	for(int a=0; a<TerminationConditionRounds; a++){// termination condition

		if(FullSolution->CPULeft == 0){
			break;
		}
        else if(FullSolution->NotAccepted.size() == 0){
             break;
        }
		else{
			FullSolution->InitialObjVal = FullSolution->ObjVal;

			int loopc = FullSolution->VectorOfChangedSols.size();
			for(int c=0; c < loopc; c++){
				FullSolution->VectorOfChangedSols.pop_back();
			}

			int loopd = FullSolution->VectorOfOriginalSols.size();
			for(int c=0; c < loopd; c++){
				FullSolution->VectorOfOriginalSols.pop_back();
			}

			// select n policies randomly and then do Pertubation for each policy, regardless improvement or not == Perturbation
			InitialPerturbation(FullSolution, CurCommonLists,NoOfNFsToChangeInPerturbation, LinksDelay, myfile2, myfile3);
			PrintSummaryGlobal(a, FullSolution, myfile2,'P', CurCommonLists);

                        PrintFullAllocation(a, 'P', FullSolution, myfile3);
			PrintAllocationLine(myfile3);

			InitialLocalSearch(FullSolution, CurCommonLists, LinksDelay, myfile2, myfile3);
			PrintSummaryGlobal(a, FullSolution, myfile2,'L', CurCommonLists);

                        PrintFullAllocation(a, 'L', FullSolution, myfile3);
			PrintAllocationLine(myfile3);

			InitialAcceptanceCriteria(FullSolution, CurCommonLists, LinksDelay);
			PrintSummaryGlobal(a, FullSolution, myfile2,'A', CurCommonLists);
			PrintNewLine(myfile2);

			PrintFullAllocation(a, 'A', FullSolution, myfile3);
			PrintAllocationLine(myfile3);
		
		}
	}

	int loopc = FullSolution->VectorOfChangedSols.size();
	for(int c=0; c < loopc; c++){
		FullSolution->VectorOfChangedSols.pop_back();
	}

	int loopd = FullSolution->VectorOfOriginalSols.size();
	for(int c=0; c < loopd; c++){
		FullSolution->VectorOfOriginalSols.pop_back();
	}
}
