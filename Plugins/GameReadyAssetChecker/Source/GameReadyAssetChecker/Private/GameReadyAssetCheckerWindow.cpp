#include "GameReadyAssetCheckerWindow.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Engine/StaticMesh.h"
#include "PhysicsEngine/BodySetup.h"

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

    CheckStaticMeshes(SelectedAssets);

    return FReply::Handled();
}

void SGameReadyAssetCheckerWindow::CheckStaticMeshes(
    const TArray<FAssetData>& SelectedAssets)
{
    int32 StaticMeshCount = 0;

    for (const FAssetData& Asset : SelectedAssets)
    {
        if (Asset.GetClass() == UStaticMesh::StaticClass())
        {
            StaticMeshCount++;

            UE_LOG(
                LogTemp,
                Log,
                TEXT("Game-Ready Asset Checker: Static Mesh found - %s"),
                *Asset.AssetName.ToString()
            );

            UStaticMesh* StaticMesh = Cast<UStaticMesh>(Asset.GetAsset());

            if (StaticMesh)
            {
                UBodySetup* BodySetup = StaticMesh->GetBodySetup();

                if (!BodySetup)
                {
                    UE_LOG(
                        LogTemp,
                        Warning,
                        TEXT("Game-Ready Asset Checker: %s - ERROR: No collision BodySetup found."),
                        *Asset.AssetName.ToString()
                    );
                }
                else
                {
                    const FKAggregateGeom& AggGeom = BodySetup->AggGeom;

                    const bool bHasSimpleCollision =
                        AggGeom.BoxElems.Num() > 0 ||
                        AggGeom.SphereElems.Num() > 0 ||
                        AggGeom.SphylElems.Num() > 0 ||
                        AggGeom.TaperedCapsuleElems.Num() > 0 ||
                        AggGeom.ConvexElems.Num() > 0;

                    if (bHasSimpleCollision)
                    {
                        UE_LOG(
                            LogTemp,
                            Log,
                            TEXT("Game-Ready Asset Checker: %s - PASS: Simple collision found."),
                            *Asset.AssetName.ToString()
                        );
                    }
                    else
                    {
                        UE_LOG(
                            LogTemp,
                            Warning,
                            TEXT("Game-Ready Asset Checker: %s - WARNING: No simple collision found."),
                            *Asset.AssetName.ToString()
                        );
                    }
                }
            }
        }
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT("Game-Ready Asset Checker: %d Static Mesh(es) found."),
        StaticMeshCount
    );
}