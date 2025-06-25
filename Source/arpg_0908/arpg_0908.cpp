// Copyright Tinzie Games

#include "arpg_0908.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Logging/LogMacros.h"
#include "Misc/Paths.h" 

//IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, arpg_0908, "arpg_0908" );

void FArpgModule::StartupModule()
{
	//FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
	//AddShaderSourceDirectoryMapping("/Project", ShaderDirectory);
}

void FArpgModule::ShutdownModule()
{
}


IMPLEMENT_PRIMARY_GAME_MODULE(FArpgModule, arpg_0908, "Arpg0908_Module");