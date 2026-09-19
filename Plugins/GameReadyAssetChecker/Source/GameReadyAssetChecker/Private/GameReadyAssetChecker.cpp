// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameReadyAssetChecker.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FGameReadyAssetCheckerModule"

void FGameReadyAssetCheckerModule::StartupModule()
{
    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(
            this,
            &FGameReadyAssetCheckerModule::RegisterMenus
        )
    );
}

void FGameReadyAssetCheckerModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
}

void FGameReadyAssetCheckerModule::RegisterMenus()
{
    FToolMenuOwnerScoped OwnerScoped(this);

    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");

    FToolMenuSection& Section = Menu->FindOrAddSection("GameReadyAssetChecker");

    Section.AddMenuEntry(
        "GameReadyAssetChecker",
        LOCTEXT("GameReadyAssetCheckerLabel", "Game-Ready Asset Checker"),
        LOCTEXT(
            "GameReadyAssetCheckerTooltip",
            "Scan selected assets for common game-readiness issues."
        ),
        FSlateIcon(),
        FToolUIActionChoice(
            FExecuteAction::CreateLambda([]()
                {
                    UE_LOG(
                        LogTemp,
                        Log,
                        TEXT("Game-Ready Asset Checker menu clicked.")
                    );
                })
        )
    );
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameReadyAssetCheckerModule, GameReadyAssetChecker)