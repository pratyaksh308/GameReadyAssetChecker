// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameReadyAssetChecker.h"
#include "ToolMenus.h"
#include "Window.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"

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
                    TSharedRef<SWindow> Window = SNew(SWindow)
                        .Title(FText::FromString(TEXT("Game-Ready Asset Checker")))
                        .ClientSize(FVector2D(700.0f, 500.0f))
                        .SupportsMaximize(true)
                        .SupportsMinimize(false);

                    Window->SetContent(
                        SNew(SGameReadyAssetCheckerWindow)
                    );

                    FSlateApplication::Get().AddWindow(Window);
                })
        )
    );
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameReadyAssetCheckerModule, GameReadyAssetChecker)