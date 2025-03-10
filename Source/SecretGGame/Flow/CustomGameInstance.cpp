﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameInstance.h"

#include "FLowController.h"
#include "FMODBlueprintStatics.h"
#include "GameplayState.h"
#include "SecretGGame/Settings/SecretGameUserSettings.h"


void UCustomGameInstance::SetCurrentObjectiveId(const EQuestObjectiveIds ObjectiveId) const
{
	GameplayState->CurrentObjectiveId = ObjectiveId;
	OnGamePlayStateChanged.Broadcast();
}

void UCustomGameInstance::AddBossKey(const EBossKeyNames BossKeyName) const
{
	GameplayState->AcquiredBossKeys.AddUnique(BossKeyName);

	if (GameplayState->CurrentObjectiveId == EQuestObjectiveIds::Explore)
	{
		SetCurrentObjectiveId(EQuestObjectiveIds::FindKeys);
	}
	
	if (HasAllKeys() && GameplayState->CurrentObjectiveId == EQuestObjectiveIds::FindKeys)
	{
		SetCurrentObjectiveId(EQuestObjectiveIds::DefeatBoss);
	}

	OnGamePlayStateChanged.Broadcast();
}

bool UCustomGameInstance::HasAllKeys() const
{
	return GameplayState->AcquiredBossKeys.Num() == GNum_Boss_Keys;
}

void UCustomGameInstance::ResetOnGameStart() const
{
	GameplayState->PreviousLevel = ELevelNames::None;
	GameplayState->CurrentLevel = ELevelNames::None;
	GameplayState->CurrentObjectiveId = EQuestObjectiveIds::Explore;
	GameplayState->AcquiredBossKeys.Empty();
	OnGamePlayStateChanged.Broadcast();
}

void UCustomGameInstance::SetCurrentLevel(const ELevelNames LevelName) const
{
	GameplayState->CurrentLevel = LevelName;
	OnGamePlayStateChanged.Broadcast();
}

void UCustomGameInstance::UpdatePreviousLevel() const
{
	GameplayState->PreviousLevel = GameplayState->CurrentLevel;
	OnGamePlayStateChanged.Broadcast();
}

void UCustomGameInstance::Init()
{
	Super::Init();
	
	// Create an instance of UGameplayState
	GameplayState = NewObject<UGameplayState>(this, UGameplayState::StaticClass());

	FStringAssetReference BackgroundMusicPath(TEXT("/Game/FMOD/Events/idle_combat"));

	if (UObject* BackgroundMusicAsset = BackgroundMusicPath.TryLoad())
		
	{
		UFMODEvent* BackgroundMusicEvent = Cast<UFMODEvent>(BackgroundMusicAsset);
		// create component with the event
		MasterAudioComponent = NewObject<UFMODAudioComponent>(this, UFMODAudioComponent::StaticClass());
		MasterAudioComponent->Event = BackgroundMusicEvent;
		MasterAudioComponent->RegisterComponent();

		USecretGameUserSettings* UserSettings = USecretGameUserSettings::GetUserSettings();
		
		MasterAudioComponent->Play();
		MasterAudioComponent->SetVolume(UserSettings->GetMusicVolume());
		
		UE_LOG(LogTemp, Warning, TEXT("Background music is playing"));
	}
}
