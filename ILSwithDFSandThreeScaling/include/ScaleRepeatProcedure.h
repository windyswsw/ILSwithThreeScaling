void ScaleRepeatProcedure(FullSol* FullSolution, CommonList* CurCommonLists, int PercentageOfPoliciesToChangeInPerturbation, float LinksDelay, int ServerCap, int InPathLinks, ofstream &myfile2, ofstream &myfile3, int TerminationConditionRounds,bool vertical, bool unified, bool incremental, ChangedPolicies* TrackChangedPolicies){

	for(int a=0; a<TerminationConditionRounds; a++){// termination condition

		if(FullSolution->CPULeft == 0){
			break;
		}
        else if(FullSolution->ScaleRejected.size() == 0){
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

			if(vertical == true){
				PerturbationVerticalScale(FullSolution, CurCommonLists,PercentageOfPoliciesToChangeInPerturbation, LinksDelay, myfile2, myfile3, TrackChangedPolicies);

				//PrintSummaryScale(a, FullSolution, myfile2, 'P', CurCommonLists, TrackChangedPolicies);
				//PrintNewLine(myfile2);

				LSVerticalScale(FullSolution, CurCommonLists, LinksDelay, ServerCap, InPathLinks, myfile2, TrackChangedPolicies );

				//PrintSummaryScale(a, FullSolution, myfile2, 'L', CurCommonLists, TrackChangedPolicies);
				//PrintNewLine(myfile2);
			}
			else if(unified == true){
				PerturbationUnifiedScale(FullSolution, CurCommonLists,PercentageOfPoliciesToChangeInPerturbation, LinksDelay, myfile2, myfile3, TrackChangedPolicies);

				//PrintSummaryScale(a, FullSolution, myfile2, 'P', CurCommonLists, TrackChangedPolicies);
				//PrintNewLine(myfile2);

				LSUnifiedScale(FullSolution, CurCommonLists, LinksDelay, ServerCap, InPathLinks, myfile2, TrackChangedPolicies );

				//PrintSummaryScale(a, FullSolution, myfile2, 'L', CurCommonLists, TrackChangedPolicies);
				//PrintNewLine(myfile2);
			}
			else if(incremental == true){
				PerturbationIncrementalScale(FullSolution, CurCommonLists,PercentageOfPoliciesToChangeInPerturbation, LinksDelay, myfile2, myfile3, TrackChangedPolicies);

				//PrintSummaryScale(a, FullSolution, myfile2, 'P', CurCommonLists, TrackChangedPolicies);
				//PrintNewLine(myfile2);
				
				LSIncrementalScale(FullSolution, CurCommonLists, LinksDelay, ServerCap, InPathLinks, myfile2, TrackChangedPolicies ); // rejected bw upto now for original policy, is saved in rejected list under original policyID
				// if a BW is accepted, that accepted amount is saved in accepted list under child policyID
				// so can perform LS, for original and child sperately
				//PrintSummaryScale(a, FullSolution, myfile2, 'L', CurCommonLists, TrackChangedPolicies);
				//PrintNewLine(myfile2);
			}

			ScaleAcceptanceCriteria(FullSolution, CurCommonLists, LinksDelay);

			//PrintSummaryScale(a, FullSolution, myfile2, 'A', CurCommonLists, TrackChangedPolicies);
			//PrintNewLine(myfile2);
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
