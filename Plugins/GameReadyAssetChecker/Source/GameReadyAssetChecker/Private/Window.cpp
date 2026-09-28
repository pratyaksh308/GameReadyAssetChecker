#include "Window.h"

#include "Checks.h"
#include "IContentBrowserSingleton.h"

void SGameReadyAssetCheckerWindow::Construct(
    const FArguments& InArgs)
{
    ChildSlot
        [
            SNew(SVerticalBox)

                // ========================================================
                // Title
                // ========================================================

                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SNew(STextBlock)
                        .Text(
                            FText::FromString(
                                TEXT("Game-Ready Asset Checker")
                            )
                        )
                ]

            // ========================================================
            // Selected asset count
            // ========================================================

            +SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SAssignNew(
                        SelectedAssetsText,
                        STextBlock
                    )
                        .Text(
                            FText::FromString(
                                TEXT("Selected assets: 0")
                            )
                        )
                ]

            // ========================================================
            // Scan button
            // ========================================================

            +SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SNew(SButton)
                        .Text(
                            FText::FromString(
                                TEXT("Scan Selected Assets")
                            )
                        )
                        .OnClicked(
                            this,
                            &SGameReadyAssetCheckerWindow::
                            OnScanSelectedAssetsClicked
                        )
                ]

            // ========================================================
            // Summary
            // ========================================================

            +SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SAssignNew(
                        SummaryText,
                        STextBlock
                    )
                        .Text(
                            FText::FromString(
                                TEXT(
                                    "Passed: 0 | Warnings: 0 | Problems: 0"
                                )
                            )
                        )
                ]

            // ========================================================
            // Filters
            // ========================================================

            +SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f, 5.0f)
                [
                    SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(2.0f)
                        [
                            SNew(SButton)
                                .Text(
                                    FText::FromString(
                                        TEXT("All")
                                    )
                                )
                                .OnClicked(
                                    this,
                                    &SGameReadyAssetCheckerWindow::
                                    OnAllFilterClicked
                                )
                        ]

                    + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(2.0f)
                        [
                            SNew(SButton)
                                .Text(
                                    FText::FromString(
                                        TEXT("Errors")
                                    )
                                )
                                .OnClicked(
                                    this,
                                    &SGameReadyAssetCheckerWindow::
                                    OnErrorsFilterClicked
                                )
                        ]

                    + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(2.0f)
                        [
                            SNew(SButton)
                                .Text(
                                    FText::FromString(
                                        TEXT("Warnings")
                                    )
                                )
                                .OnClicked(
                                    this,
                                    &SGameReadyAssetCheckerWindow::
                                    OnWarningsFilterClicked
                                )
                        ]

                    + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(2.0f)
                        [
                            SNew(SButton)
                                .Text(
                                    FText::FromString(
                                        TEXT("Passed")
                                    )
                                )
                                .OnClicked(
                                    this,
                                    &SGameReadyAssetCheckerWindow::
                                    OnPassedFilterClicked
                                )
                        ]
                ]

            // ========================================================
            // Results
            // ========================================================

            +SVerticalBox::Slot()
                .FillHeight(1.0f)
                .Padding(10.0f)
                [
                    SNew(SScrollBox)

                        + SScrollBox::Slot()
                        [
                            SAssignNew(
                                ResultsContainer,
                                SVerticalBox
                            )
                        ]
                ]
        ];
}

FReply SGameReadyAssetCheckerWindow::
OnScanSelectedAssetsClicked()
{
    TArray<FAssetData> SelectedAssets;

    IContentBrowserSingleton::Get().GetSelectedAssets(
        SelectedAssets
    );

    if (SelectedAssetsText.IsValid())
    {
        SelectedAssetsText->SetText(
            FText::Format(
                FText::FromString(
                    TEXT("Selected assets: {0}")
                ),
                SelectedAssets.Num()
            )
        );
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT(
            "Game-Ready Asset Checker: %d asset(s) selected."
        ),
        SelectedAssets.Num()
    );

    CheckStaticMeshes(SelectedAssets);

    return FReply::Handled();
}

FReply SGameReadyAssetCheckerWindow::
OnAllFilterClicked()
{
    CurrentFilter = EResultFilter::All;
    RefreshResults();

    return FReply::Handled();
}

FReply SGameReadyAssetCheckerWindow::
OnErrorsFilterClicked()
{
    CurrentFilter = EResultFilter::Errors;
    RefreshResults();

    return FReply::Handled();
}

FReply SGameReadyAssetCheckerWindow::
OnWarningsFilterClicked()
{
    CurrentFilter = EResultFilter::Warnings;
    RefreshResults();

    return FReply::Handled();
}

FReply SGameReadyAssetCheckerWindow::
OnPassedFilterClicked()
{
    CurrentFilter = EResultFilter::Passed;
    RefreshResults();

    return FReply::Handled();
}

void SGameReadyAssetCheckerWindow::CheckStaticMeshes(
    const TArray<FAssetData>& SelectedAssets)
{
    CheckResults =
        ::GameReadyAssetCheckerChecks::RunChecks(
            SelectedAssets
        );

    RefreshResults();
}

bool SGameReadyAssetCheckerWindow::ShouldDisplayResult(
    const GameReadyAssetCheckerChecks::FCheckResult& Result
) const
{
    switch (CurrentFilter)
    {
    case EResultFilter::All:
        return true;

    case EResultFilter::Errors:
        return Result.Severity ==
            GameReadyAssetCheckerChecks::ECheckSeverity::Error;

    case EResultFilter::Warnings:
        return Result.Severity ==
            GameReadyAssetCheckerChecks::ECheckSeverity::Warning;

    case EResultFilter::Passed:
        return Result.Severity ==
            GameReadyAssetCheckerChecks::ECheckSeverity::Passed;
    }

    return true;
}

int32 SGameReadyAssetCheckerWindow::GetSeverityPriority(
    GameReadyAssetCheckerChecks::ECheckSeverity Severity
) const
{
    switch (Severity)
    {
    case GameReadyAssetCheckerChecks::ECheckSeverity::Error:
        return 0;

    case GameReadyAssetCheckerChecks::ECheckSeverity::Warning:
        return 1;

    case GameReadyAssetCheckerChecks::ECheckSeverity::Passed:
        return 2;
    }

    return 3;
}

void SGameReadyAssetCheckerWindow::RefreshResults()
{
    if (!ResultsContainer.IsValid())
    {
        return;
    }

    ResultsContainer->ClearChildren();

    int32 PassedCount = 0;
    int32 WarningCount = 0;
    int32 ErrorCount = 0;

    for (const GameReadyAssetCheckerChecks::FCheckResult&
        Result : CheckResults)
    {
        switch (Result.Severity)
        {
        case GameReadyAssetCheckerChecks::ECheckSeverity::Passed:
            PassedCount++;
            break;

        case GameReadyAssetCheckerChecks::ECheckSeverity::Warning:
            WarningCount++;
            break;

        case GameReadyAssetCheckerChecks::ECheckSeverity::Error:
            ErrorCount++;
            break;
        }
    }

    if (SummaryText.IsValid())
    {
        SummaryText->SetText(
            FText::Format(
                FText::FromString(
                    TEXT(
                        "Passed: {0} | Warnings: {1} | Problems: {2}"
                    )
                ),
                PassedCount,
                WarningCount,
                ErrorCount
            )
        );
    }

    // ================================================================
    // Display filtered results.
    //
    // Errors are shown first, followed by warnings, then passes.
    // ================================================================

    TArray<GameReadyAssetCheckerChecks::FCheckResult>
        FilteredResults;

    for (const GameReadyAssetCheckerChecks::FCheckResult&
        Result : CheckResults)
    {
        if (ShouldDisplayResult(Result))
        {
            FilteredResults.Add(Result);
        }
    }

    FilteredResults.Sort(
        [this](
            const GameReadyAssetCheckerChecks::FCheckResult& A,
            const GameReadyAssetCheckerChecks::FCheckResult& B
            )
        {
            return GetSeverityPriority(A.Severity) <
                GetSeverityPriority(B.Severity);
        }
    );

    for (const GameReadyAssetCheckerChecks::FCheckResult&
        Result : FilteredResults)
    {
        FString SeverityText;

        switch (Result.Severity)
        {
        case GameReadyAssetCheckerChecks::ECheckSeverity::Passed:
            SeverityText = TEXT("PASS");
            break;

        case GameReadyAssetCheckerChecks::ECheckSeverity::Warning:
            SeverityText = TEXT("WARNING");
            break;

        case GameReadyAssetCheckerChecks::ECheckSeverity::Error:
            SeverityText = TEXT("ERROR");
            break;
        }

        ResultsContainer->AddSlot()
            .AutoHeight()
            .Padding(5.0f)
            [
                SNew(SVerticalBox)

                    // ------------------------------------------------
                    // Asset + severity
                    // ------------------------------------------------

                    +SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(STextBlock)
                            .Text(
                                FText::Format(
                                    FText::FromString(
                                        TEXT(
                                            "[{0}] {1}"
                                        )
                                    ),
                                    FText::FromString(
                                        SeverityText
                                    ),
                                    FText::FromString(
                                        Result.AssetName
                                    )
                                )
                            )
                    ]

                // ------------------------------------------------
                // Problem
                // ------------------------------------------------

                +SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(15.0f, 2.0f)
                    [
                        SNew(STextBlock)
                            .Text(
                                FText::FromString(
                                    Result.Problem
                                )
                            )
                    ]

                // ------------------------------------------------
                // Why it matters
                // ------------------------------------------------

                +SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(15.0f, 2.0f)
                    [
                        SNew(STextBlock)
                            .Text(
                                FText::Format(
                                    FText::FromString(
                                        TEXT(
                                            "Why it matters: {0}"
                                        )
                                    ),
                                    FText::FromString(
                                        Result.WhyItMatters
                                    )
                                )
                            )
                    ]

                // ------------------------------------------------
                // Suggested action
                // ------------------------------------------------

                +SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(15.0f, 2.0f)
                    [
                        SNew(STextBlock)
                            .Text(
                                FText::Format(
                                    FText::FromString(
                                        TEXT(
                                            "Suggested action: {0}"
                                        )
                                    ),
                                    FText::FromString(
                                        Result.SuggestedAction
                                    )
                                )
                            )
                    ]
            ];
    }
}