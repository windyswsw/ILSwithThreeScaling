/*
 * Main.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iterator>
#include <map>
#include <time.h>
#include <algorithm>
#include <stdio.h>
#include <cstring>
#include <iomanip>
#include <stdarg.h>
#include <cstdarg>
#include <ctime>
//#include <chrono>
#include <unordered_map>
#include <set>

#include "StructsUse.h"
#include "FormatTime.h"
#include "CountCPU.h"
#include "GenLists.h"
#include "InitialListsGeneration.h"
#include "LinksPathsCheck.h"
#include "UpdateLists.h"
#include "AddRemovePolicy.h"
#include "ObjectValueCalculation.h"
#include "ScaleObjectValueCalculation.h"
#include "Print.h"
#include "PrintAllocation.h"
////#include "PrintTime.h"
#include "DFS.h"
//#include "Random.h"
#include "InitialAcceptanceCriteria.h"
#include "InitialPerturbation.h"
#include "InitialLocalSearchWithOnePolicyMove.h"
#include "DFSLocalSearchWithOnePolicyMove.h"
#include "InitialLocalSearch.h"
#include "InitialRepeatProcedure.h"
#include "InitialPolicyImplementation.h"
#include "Affordable.h"
#include "PolicyChangeListForRound.h"
#include "DFSVerticalScaling.h"
#include "DFSIncrementalScaling.h"
#include "DFSUnifiedScaling.h"
#include "VerticalScale.h"
#include "IncrementalScale.h"
#include "UnifiedScale.h"
#include "ScaleAcceptanceCriteria.h"
#include "LSVerticalScale.h"
#include "LSIncrementalScale.h"
#include "LSUnifiedScale.h"
#include "PerturbationUnifiedScale.h"
#include "PerturbationVerticalScale.h"
#include "PerturbationIncrementalScale.h"
#include "ScaleRepeatProcedure.h"
#include "ScaleLocal.h"

//int main(){

int main(int argc, char* argv[]){

	if (argc != 2){
		cout << "Usage: <V or U or I: V for Vertical, U for Unified, I for Incremental>" << "\n";
		return -1;
	}
	else{
		bool vertical = false;
		bool unified = false;
		bool incremental = true;

		if (strcmp(argv[1],"V") == 0){
			vertical = true;
		}
		else if	(strcmp(argv[1],"U") == 0){
			unified = true;
		}
		else if (strcmp(argv[1],"I") == 0){
			incremental = true;
		}
		else{
			cout << "Usage: <V or U or I: V for Vertical, U for Unified, I for Incremental> " << "\n";
			return -1;
		}
	
	    float LinksDelay = 0.0001; // from cisco white paper, processing delay per hop 25 micro seconds
		int NoOfServers = 128;
		int NoOfLinks = 160;
		int InPathLinks = 3;

        float ServerCap = 24;   // from Online VNF scaling in data centers
		//float MaxLinksCap = 10000; // in Mbps // 10Gbps from Network Functions Virtualization with Soft RealTime Gurantees
        float MaxLinksCap = 1000000000;
		int experiments = 1;

        int InitialLocalSearchRounds = 5;
		int InitialTerminationConditionRounds = 1;
		int InitialNoOfPoliciesToChangeInPerturbation = 5;
		int ScaleTerminationConditionRounds = 10;
		int ScalePercentageOfPoliciesToChangeInPerturbation = 10;

		/*time_t currentTime;
    	struct tm *currentDate;

    	time(&currentTime);
    	currentDate = localtime(&currentTime);*/
    	
		char filename1[256] = {0};
		strcpy(filename1,"TimeILS.csv");
		//strcat(filename1, fmt("-%d-%d-%d@%d.%d.%d.csv", currentDate->tm_mday, currentDate->tm_mon+1, currentDate->tm_year+1900, currentDate->tm_hour, currentDate->tm_min, currentDate->tm_sec).c_str());
		ofstream myfile1;
		myfile1.open (filename1, ios::out | ios::app);

		/*typedef std::chrono::high_resolution_clock Time1;
		typedef std::chrono::microseconds ms;*/

		char filename2[256] = {0};
		strcpy(filename2,"SummaryILS.csv");
		//strcat(filename3, fmt("-%d-%d-%d@%d.%d.%d.csv", currentDate->tm_mday, currentDate->tm_mon+1, currentDate->tm_year+1900, currentDate->tm_hour, currentDate->tm_min, currentDate->tm_sec).c_str());
		ofstream myfile2;
		myfile2.open (filename2, ios::out | ios::app);

		char filename3[256] = {0};
		strcpy(filename3,"Allocation.txt");
		//strcat(filename3, fmt("-%d-%d-%d@%d.%d.%d.csv", currentDate->tm_mday, currentDate->tm_mon+1, currentDate->tm_year+1900, currentDate->tm_hour, currentDate->tm_min, currentDate->tm_sec).c_str());
		ofstream myfile3;
		myfile3.open (filename3, ios::out | ios::app);
	
		PrintFirstLine(myfile2);
		PrintFirstLine(myfile2);
		PrintNewLine(myfile2);

		srand((unsigned int) time(0));

		for(int r=0; r<experiments;r++){

			CommonList NewCommonList;
			GenCommonList(&NewCommonList);
			UniqueList NewUniqueList;
			GenUniqueList(&NewUniqueList, NoOfServers, ServerCap, MaxLinksCap);

			FullSol InitialFullSolution;
			InitialFullSolution.CurLists = NewUniqueList;
			InitialFullSolution.ObjVal = 0;
			InitialFullSolution.InitialObjVal = 0;
			InitialFullSolution.CPULeft = NoOfServers * ServerCap;
			InitialFullSolution.MaxCPUs = 0;

			CountTotalCPUs(&InitialFullSolution, &NewCommonList);

			InitialPolicyImplementation(&InitialFullSolution, &NewCommonList,LinksDelay, myfile1, myfile2, myfile3,InitialNoOfPoliciesToChangeInPerturbation, InitialTerminationConditionRounds  );
			PrintSummaryGlobal(r, &InitialFullSolution, myfile2, 'S', &NewCommonList);
			PrintNewLine(myfile2);
			PrintFullAllocation(r, 'S', &InitialFullSolution, myfile3);
			PrintAllocationLine(myfile3);

			FullSol InitialFullSolutionL;
			InitialFullSolutionL.VectorOfPartialSols =  InitialFullSolution.VectorOfPartialSols;
			InitialFullSolutionL.VectorOfOriginalSols = InitialFullSolution.VectorOfOriginalSols;
			InitialFullSolutionL.VectorOfChangedSols = InitialFullSolution.VectorOfChangedSols;
			InitialFullSolutionL.CurLists = InitialFullSolution.CurLists;
			InitialFullSolutionL.ObjVal = InitialFullSolution.ObjVal;
			InitialFullSolutionL.InitialObjVal = InitialFullSolution.ObjVal;
			InitialFullSolutionL.CPULeft = InitialFullSolution.CPULeft;
			InitialFullSolutionL.MaxCPUs = InitialFullSolution.MaxCPUs;

			vector<ChangeInfo> FullChangeListCur2;
			gen_PolicyChangeList(&FullChangeListCur2, "ScalePattern.txt");
			NewCommonList.FullChangeListCur = FullChangeListCur2;
			PrintScaleFirstLine(myfile2);
			PrintScaleFirstLine(myfile2);
			PrintNewLine(myfile2);
			ScaleLocal(&InitialFullSolutionL, &NewCommonList, LinksDelay, ServerCap, InPathLinks, ScalePercentageOfPoliciesToChangeInPerturbation, ScaleTerminationConditionRounds, myfile2, myfile3, vertical, unified, incremental);
		}
	
		myfile1.close();
		myfile2.close();
		myfile3.close();
	}
	return 0;
}
