// Copyright Tinzie Games


#include "Actor/WeaponData.h"

FWeaponInfo UWeaponData::GetWeaponInfo(int Index)
{
	return Weapons[Index];
}

int UWeaponData::GetWeaponIndex(AArpgWeaponActor* WeaponActor)
{
	//for (FHeadInfo &HeadInfo : Heads)
	for (int i = 0; i < Weapons.Num(); i++)
	{
		AArpgWeaponActor* WeaponActorRef = Weapons[i].WeaponReference->GetDefaultObject<AArpgWeaponActor>();
		//if (HeadActor->IsA(Heads[i].HeadReference->GetClass()))
		if (WeaponActor->WeaponMeshRef == (WeaponActorRef->WeaponMeshRef) && WeaponActor->GrantedAbilities == WeaponActorRef->GrantedAbilities)
		{
			return i;
		}
	}


	return -1;
}
