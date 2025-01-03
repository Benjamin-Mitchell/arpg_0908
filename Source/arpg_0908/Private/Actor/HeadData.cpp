// Copyright Tinzie Games


#include "Actor/HeadData.h"

FHeadInfo UHeadData::GetHeadInfo(int Index)
{
	return Heads[Index];
}

int UHeadData::GetHeadIndex(AarpgHeadActor* HeadActor)
{
	//for (FHeadInfo &HeadInfo : Heads)
	for (int i = 0; i < Heads.Num(); i++)
	{
		AarpgHeadActor* HeadActorRef = Heads[i].HeadReference->GetDefaultObject<AarpgHeadActor>();
		//if (HeadActor->IsA(Heads[i].HeadReference->GetClass()))
		if (HeadActor->HeadMeshRef == (HeadActorRef->HeadMeshRef) && HeadActor->GrantedAbilities == HeadActorRef->GrantedAbilities)
		{
			return i;
		}
	}


	return -1;
}
