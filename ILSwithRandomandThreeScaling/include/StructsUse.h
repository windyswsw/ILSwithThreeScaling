/*
 * StructsUse.h
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */

using namespace std;

struct Links{
	int LinksBW[160][160];
	short int LinksUse[160][160];
	void PrintLinksBW();
	void PrintLinksUse();
};

struct SinglePath{
	vector<int> Path;
};

struct ReturnPath{
	SinglePath NewPath;
	bool NoPathFound;
};

struct Paths{
	vector<SinglePath> PathsList[129][129];
	vector<SinglePath> InPaths;
	vector<SinglePath> OutPaths;
};

struct Policy{
	vector<int> NFIDs;
	short int PolicyID;
	char ART;
	float Deadline;
	short int Customer;
	vector<int> Children;
	short int Parent;
	int OriginalBW;
	int CurrentBW;
	int NextBW;
};

struct ReturnDelay{
	bool DeadlineOK;
	float delay;
};

struct NFType{
	short int NFID;
	int cpu;
	float scale;
};

struct NFAllocation{
	short int ServerID;
	SinglePath path;
	short int NFID;
};

struct PartialSol{
	vector<NFAllocation> VMsForAPolicy;
	int PolicyID;
	int Splits;
	float Delay;
	bool accepted;
	bool HaveResources;
	int AllocatedBW;
};

struct VecForHashTable{
	set<int> ServerIndex;
};

struct ChangeInfo{
	int polToChange;
	string AR;
	int ScaleBW;
};

struct ScaleInfo{
	int pol;
	string AR;
	int BWChange;
};

struct RoundChangeInfo{
	vector<ChangeInfo> RoundChangeList;
};

struct UniqueList{
	map<int,short int> ServerCapMap;
	unordered_multimap<int, VecForHashTable> ServerCapHashTable;
	vector<int> ServerCapArray;
	vector<short int> ServerUsedList;
	Links LinksLists;
};

struct CommonList{
	vector<Policy> PolicyList;
	Paths PathsList;
	vector<ChangeInfo> FullChangeListCur;
	vector<NFType> NFInfo;
};

struct FullSol{
	vector<PartialSol> VectorOfPartialSols;
	vector<PartialSol> VectorOfOriginalSols;
	vector<PartialSol> VectorOfChangedSols;
	unordered_multimap<short int, PartialSol> Accepted;
	unordered_multimap<short int, PartialSol> NotAccepted;
	unordered_multimap<short int, ScaleInfo> ScaleRejected;
	vector<int> ScaleAccepted;
	int ObjVal;
	int InitialObjVal;
	int CPULeft;
	int MaxCPUs;
	UniqueList CurLists;
};

struct ChangedPolicies{
	map<short int,PartialSol> OriginalPolicies;
	map<short int,PartialSol> CurrentlyChangedPolicies;
};
