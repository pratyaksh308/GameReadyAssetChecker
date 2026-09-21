#include "GameReadyAssetCheckerWindow.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

void SGameReadyAssetCheckerWindow::Construct(const FArguments& InArgs)
{
    ChildSlot
        [
            SNew(SVerticalBox)

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SAssignNew(SelectedAssetsText, STextBlock)
                        .Text(FText::FromString(TEXT("Selected assets: 0")))
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SNew(SButton)
                        .Text(FText::FromString(TEXT("Scan Selected Assets")))
                        .OnClicked(this, &SGameReadyAssetCheckerWindow::OnScanSelectedAssetsClicked)
                ]
        ];
}

FReply SGameReadyAssetCheckerWindow::OnScanSelectedAssetsClicked()
{
    TArray<FAssetData> SelectedAssets;

    IContentBrowserSingleton::Get().GetSelectedAssets(SelectedAssets);

    if (SelectedAssetsText.IsValid())
    {
        SelectedAssetsText->SetText(
            FText::Format(
                FText::FromString(TEXT("Selected assets: {0}")),
                SelectedAssets.Num()
            )
        );
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT("Game-Ready Asset Checker: %d asset(s) selected."),
        SelectedAssets.Num()
    );

    return FReply::Handled();
}