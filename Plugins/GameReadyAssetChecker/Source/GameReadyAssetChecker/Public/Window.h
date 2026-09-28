#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Checks.h"

class SGameReadyAssetCheckerWindow : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SGameReadyAssetCheckerWindow)
        {
        }
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    enum class EResultFilter
    {
        All,
        Errors,
        Warnings,
        Passed
    };

    FReply OnScanSelectedAssetsClicked();

    FReply OnAllFilterClicked();
    FReply OnErrorsFilterClicked();
    FReply OnWarningsFilterClicked();
    FReply OnPassedFilterClicked();

    void CheckStaticMeshes(
        const TArray<FAssetData>& SelectedAssets
    );

    void RefreshResults();

    bool ShouldDisplayResult(
        const GameReadyAssetCheckerChecks::FCheckResult& Result
    ) const;

    int32 GetSeverityPriority(
        GameReadyAssetCheckerChecks::ECheckSeverity Severity
    ) const;

    TSharedPtr<STextBlock> SelectedAssetsText;
    TSharedPtr<STextBlock> SummaryText;
    TSharedPtr<SVerticalBox> ResultsContainer;

    TArray<GameReadyAssetCheckerChecks::FCheckResult> CheckResults;

    EResultFilter CurrentFilter = EResultFilter::All;
};