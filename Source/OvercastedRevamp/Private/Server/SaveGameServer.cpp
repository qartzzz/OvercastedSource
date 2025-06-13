//Copyright OVERCASTED, Inc. All Rights Reserved.
#include "OvercastedRevamp/Public/Server/SaveGameServer.h"
#include "Misc/DateTime.h"


int64 USaveGameServer::GetWipeTime_Implementation()
{
	return WipeTime;
}

int64 USaveGameServer::SetWipeTime_Implementation()
{
	return WipeTime = FDateTime::UtcNow().ToUnixTimestamp();
}