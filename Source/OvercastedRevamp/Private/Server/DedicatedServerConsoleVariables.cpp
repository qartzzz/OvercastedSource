//Copyright OVERCASTED, Inc. All Rights Reserved.


#include "OvercastedRevamp/Public/Server/DedicatedServerConsoleVariables.h"

// Sets default values
ADedicatedServerConsoleVariables::ADedicatedServerConsoleVariables()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	static TAutoConsoleVariable<FString> CVarServerName(
		TEXT("Server.Name"),
		"MyServer",
		TEXT("Defines the name of your server.\n"),
		ECVF_Default
	);
	static TAutoConsoleVariable<FString> CVarServerDescription(
		TEXT("Server.Description"),
		"MyServer",
		TEXT("Defines the description of your server.\n"),
		ECVF_Default
	);
	static TAutoConsoleVariable<int32> CVarServerMapSize(
		TEXT("Server.MapSize"),
		4000,
		TEXT("Defines the size of map on your server.\n"),
		ECVF_Default
	);
	static TAutoConsoleVariable<FString> CVarServerMapName(
		TEXT("Server.MapName"),
		"Procedural Map",
		TEXT("Defines the name of map on your server.\n"),
		ECVF_Default
	);
}

// Called when the game starts or when spawned
void ADedicatedServerConsoleVariables::BeginPlay()
{
	Super::BeginPlay();
	
}



// Called every frame
void ADedicatedServerConsoleVariables::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

