int CountSplits(Policy P, int ServerCap, int BW, vector<NFType> NFInfo){

	int ServersNeeded = 1;
	int TotalCap = 0;

	for(int i=0; i< P.NFIDs.size(); i++){

		int NFID = P.NFIDs[i];
		NFType MyNF = NFInfo[NFID];
		int NFBasicCap = MyNF.cpu;
		int NFCapacity = (BW / NFBasicCap);

		if( BW % NFBasicCap != 0){
			NFCapacity++;
		}

		TotalCap = TotalCap + NFCapacity;

		if(((TotalCap / ServerCap) == 1) && ((TotalCap % ServerCap) > 0)){
			ServersNeeded = ServersNeeded + 1;
			TotalCap = NFCapacity;
		}

		BW = MyNF.scale * BW;
	}

	return ServersNeeded;
}

float DelayBySplits(float LinksDelay, int NoOfServers, int InPathLinks){
	float delay = (((NoOfServers-1) * 2) + InPathLinks ) * LinksDelay;

	return delay;
}

float ProcessingDelay(Policy P, vector<NFType> NFInfo){

	float ProcDelay = 0;

	for(int i=0; i< P.NFIDs.size(); i++){

		float NumberOfPackets = 0; // calculate NumberOfPackets in the traffic, assuming BW is in Mbpb and Packet size 744 Bytes
		float TrafficInBytes = 0;
		float DelayOfPackets = 0;

		int NFID = P.NFIDs[i];
		NFType MyNF = NFInfo[NFID];
		float TotalCap = MyNF.cpu;
		TrafficInBytes = (TotalCap * 1000000) / 8;
		NumberOfPackets = TrafficInBytes / 1500;
		DelayOfPackets = (1/NumberOfPackets);
		ProcDelay = ProcDelay + DelayOfPackets;
	}

	return ProcDelay;
}

bool ServerEnough(Policy P, int ServerCap, int BW, vector<NFType> NFInfo){

	bool ServerEnoughN = true;

	for(int i=0; i< P.NFIDs.size(); i++){

		int NFID = P.NFIDs[i];
		NFType MyNF = NFInfo[NFID];
		int NFBasicCap = MyNF.cpu;
		int NFCapacity = (BW / NFBasicCap);

		if( BW % NFBasicCap != 0){
			NFCapacity++;
		}

		if(NFCapacity > ServerCap){
			ServerEnoughN = false;
			break;
		}		

		BW = MyNF.scale * BW;
	}

	return ServerEnoughN;
}

bool Affordable(Policy P, int ServerCap, vector<NFType> NFInfo, float LinksDelay, int InPathLinks, int BW){

	bool PAffordable = false;

	bool ServerEnoughN = ServerEnough(P, ServerCap, BW, NFInfo);

	if(ServerEnoughN == true){
		int NumberOfSplits =  CountSplits(P, ServerCap, BW, NFInfo);
		float SplitDelay = DelayBySplits(LinksDelay, NumberOfSplits, InPathLinks);
		float ProcDelay = ProcessingDelay(P, NFInfo);
		float TotalDelay = SplitDelay + ProcDelay;

		if (P.Deadline >= TotalDelay){
			PAffordable = true;
		}
	}

	return PAffordable;

}