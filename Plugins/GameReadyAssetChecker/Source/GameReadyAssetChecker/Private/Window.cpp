#include "Window.h"

#include "Checks.h"
#include "IContentBrowserSingleton.h"
#include "Styling/SlateColor.h"

void SGameReadyAssetCheckerWindow::Construct(
    const FArguments& InArgs)
{
    ChildSlot
        [
            SNew(SVerticalBox)

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(12.0f, 10.0f, 12.0f, 6.0f)
                [
                    SNew(STextBlock)
                        .Text(
                            FText::FromString(
                                TEXT("Game-Ready Asset Checker")
                            )
                        )
                        .Font(
                            FCoreStyle::GetDefaultFontStyle(
                                TEXT("Bold"),
                                16
                            )
                        )
                ]

            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(12.0f, 4.0f, 12.0f, 8.0f)
                [
                    SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .VAlign(VAlign_Center)
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

                    + SHorizontalBox::Slot()
                        .AutoWidth()
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
                ]

            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(12.0f, 4.0f, 12.0f, 8.0f)
                [
                    SNew(SBorder)
                        .Padding(10.0f, 8.0f)
                        [
                            SAssignNew(
                                SummaryText,
                                STextBlock
                            )
                                .Text(
                                    FText::FromString(
                                        TEXT(
                                            "Passed: 0  |  Warnings: 0  |  Problems: 0"
                                        )
                                    )
                                )
                                .Font(
                                    FCoreStyle::GetDefaultFontStyle(
                                        TEXT("Bold"),
                                        10
                                    )
                                )
                        ]
                ]

            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(12.0f, 2.0f, 12.0f, 8.0f)
                [
                    SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(0.0f, 0.0f, 4.0f, 0.0f)
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
                        .Padding(0.0f, 0.0f, 4.0f, 0.0f)
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
                        .Padding(0.0f, 0.0f, 4.0f, 0.0f)
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

            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(12.0f, 2.0f, 12.0f, 4.0f)
                [
                    SAssignNew(
                        ResultsHeaderText,
                        STextBlock
                    )
                        .Text(
                            FText::FromString(
                                TEXT("All Results · 0 asset(s)")
                            )
                        )
                        .Font(
                            FCoreStyle::GetDefaultFontStyle(
                                TEXT("Bold"),
                                11
                            )
                        )
                ]

            + SVerticalBox::Slot()
                .FillHeight(1.0f)
                .Padding(12.0f, 0.0f, 12.0f, 12.0f)
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

FText SGameReadyAssetCheckerWindow::GetSeverityText(
    GameReadyAssetCheckerChecks::ECheckSeverity Severity
) const
{
    switch (Severity)
    {
    case GameReadyAssetCheckerChecks::ECheckSeverity::Error:
        return FText::FromString(TEXT("ERROR"));

    case GameReadyAssetCheckerChecks::ECheckSeverity::Warning:
        return FText::FromString(TEXT("WARNING"));

    case GameReadyAssetCheckerChecks::ECheckSeverity::Passed:
        return FText::FromString(TEXT("PASS"));
    }

    return FText::FromString(TEXT("UNKNOWN"));
}

FSlateColor SGameReadyAssetCheckerWindow::GetSeverityColor(
    GameReadyAssetCheckerChecks::ECheckSeverity Severity
) const
{
    switch (Severity)
    {
    case GameReadyAssetCheckerChecks::ECheckSeverity::Passed:
        return FSlateColor(
            FLinearColor(
                0.25f,
                0.85f,
                0.35f,
                1.0f
            )
        );

    case GameReadyAssetCheckerChecks::ECheckSeverity::Warning:
        return FSlateColor(
            FLinearColor(
                1.0f,
                0.75f,
                0.15f,
                1.0f
            )
        );

    case GameReadyAssetCheckerChecks::ECheckSeverity::Error:
        return FSlateColor(
            FLinearColor(
                1.0f,
                0.25f,
                0.25f,
                1.0f
            )
        );
    }

    return FSlateColor(
        FLinearColor::White
    );
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
                        "Passed: {0}  |  Warnings: {1}  |  Problems: {2}"
                    )
                ),
                PassedCount,
                WarningCount,
                ErrorCount
            )
        );
    }

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

    TArray<FString> AssetNames;

    for (const GameReadyAssetCheckerChecks::FCheckResult&
        Result : FilteredResults)
    {
        if (!AssetNames.Contains(Result.AssetName))
        {
            AssetNames.Add(Result.AssetName);
        }
    }

    if (ResultsHeaderText.IsValid())
    {
        FString FilterText;

        switch (CurrentFilter)
        {
        case EResultFilter::All:
            FilterText = TEXT("All Results");
            break;

        case EResultFilter::Errors:
            FilterText = TEXT("Errors");
            break;

        case EResultFilter::Warnings:
            FilterText = TEXT("Warnings");
            break;

        case EResultFilter::Passed:
            FilterText = TEXT("Passed");
            break;
        }

        ResultsHeaderText->SetText(
            FText::Format(
                FText::FromString(
                    TEXT("{0} · {1} asset(s)")
                ),
                FText::FromString(FilterText),
                AssetNames.Num()
            )
        );
    }

    if (FilteredResults.Num() == 0)
    {
        FString EmptyMessage;

        if (CheckResults.Num() == 0)
        {
            EmptyMessage =
                TEXT(
                    "No scan results yet.\n"
                    "Select supported assets and click Scan Selected Assets."
                );
        }
        else
        {
            switch (CurrentFilter)
            {
            case EResultFilter::Errors:
                EmptyMessage =
                    TEXT(
                        "No errors found.\n"
                        "Your selected assets have no error-level results."
                    );
                break;

            case EResultFilter::Warnings:
                EmptyMessage =
                    TEXT(
                        "No warnings found.\n"
                        "Your selected assets have no warning-level results."
                    );
                break;

            case EResultFilter::Passed:
                EmptyMessage =
                    TEXT(
                        "No passed checks found."
                    );
                break;

            case EResultFilter::All:
                EmptyMessage =
                    TEXT(
                        "No results found."
                    );
                break;
            }
        }

        ResultsContainer->AddSlot()
            .AutoHeight()
            .Padding(10.0f)
            [
                SNew(SBorder)
                    .Padding(16.0f)
                    [
                        SNew(STextBlock)
                            .Text(
                                FText::FromString(
                                    EmptyMessage
                                )
                            )
                            .Justification(
                                ETextJustify::Center
                            )
                            .AutoWrapText(true)
                    ]
            ];

        return;
    }

    for (const FString& AssetName : AssetNames)
    {
        int32 AssetPassedCount = 0;
        int32 AssetWarningCount = 0;
        int32 AssetErrorCount = 0;

        for (const GameReadyAssetCheckerChecks::FCheckResult&
            Result : CheckResults)
        {
            if (Result.AssetName != AssetName)
            {
                continue;
            }

            switch (Result.Severity)
            {
            case GameReadyAssetCheckerChecks::ECheckSeverity::Passed:
                AssetPassedCount++;
                break;

            case GameReadyAssetCheckerChecks::ECheckSeverity::Warning:
                AssetWarningCount++;
                break;

            case GameReadyAssetCheckerChecks::ECheckSeverity::Error:
                AssetErrorCount++;
                break;
            }
        }

        ResultsContainer->AddSlot()
            .AutoHeight()
            .Padding(0.0f, 0.0f, 0.0f, 10.0f)
            [
                SNew(SBorder)
                    .Padding(10.0f)
                    [
                        SNew(SVerticalBox)

                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(0.0f, 0.0f, 0.0f, 8.0f)
                            [
                                SNew(SHorizontalBox)

                                    + SHorizontalBox::Slot()
                                    .FillWidth(1.0f)
                                    .VAlign(VAlign_Center)
                                    [
                                        SNew(STextBlock)
                                            .Text(
                                                FText::FromString(
                                                    AssetName
                                                )
                                            )
                                            .Font(
                                                FCoreStyle::
                                                GetDefaultFontStyle(
                                                    TEXT("Bold"),
                                                    11
                                                )
                                            )
                                    ]

                                + SHorizontalBox::Slot()
                                    .AutoWidth()
                                    .VAlign(VAlign_Center)
                                    [
                                        SNew(STextBlock)
                                            .Text(
                                                FText::Format(
                                                    FText::FromString(
                                                        TEXT(
                                                            "{0} Passed  |  {1} Warnings  |  {2} Errors"
                                                        )
                                                    ),
                                                    AssetPassedCount,
                                                    AssetWarningCount,
                                                    AssetErrorCount
                                                )
                                            )
                                            .Font(
                                                FCoreStyle::
                                                GetDefaultFontStyle(
                                                    TEXT("Regular"),
                                                    9
                                                )
                                            )
                                    ]
                            ]

                        + SVerticalBox::Slot()
                            .AutoHeight()
                            [
                                SNew(SVerticalBox)
                            ]
                    ]
            ];

        SVerticalBox* AssetResultsBox = nullptr;

        const int32 LastCardIndex =
            ResultsContainer->GetChildren()->Num() - 1;

        if (LastCardIndex >= 0)
        {
            TSharedRef<SWidget> LastWidget =
                ResultsContainer->GetChildren()->GetChildAt(
                    LastCardIndex
                );

            TSharedPtr<SBorder> CardBorder =
                StaticCastSharedRef<SBorder>(
                    LastWidget
                );

            TSharedPtr<SWidget> CardChild =
                CardBorder->GetContent();

            if (CardChild.IsValid())
            {
                TSharedPtr<SVerticalBox> CardBox =
                    StaticCastSharedPtr<SVerticalBox>(
                        CardChild
                    );

                if (CardBox.IsValid() &&
                    CardBox->GetChildren()->Num() > 1)
                {
                    TSharedPtr<SWidget> ResultsWidget =
                        CardBox->GetChildren()->GetChildAt(1);

                    AssetResultsBox =
                        static_cast<SVerticalBox*>(
                            ResultsWidget.Get()
                            );
                }
            }
        }

        if (!AssetResultsBox)
        {
            continue;
        }

        for (const GameReadyAssetCheckerChecks::FCheckResult&
            Result : FilteredResults)
        {
            if (Result.AssetName != AssetName)
            {
                continue;
            }

            AssetResultsBox->AddSlot()
                .AutoHeight()
                .Padding(0.0f, 3.0f)
                [
                    SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Top)
                        .Padding(0.0f, 0.0f, 12.0f, 0.0f)
                        [
                            SNew(SBox)
                                .WidthOverride(75.0f)
                                [
                                    SNew(STextBlock)
                                        .Text(
                                            GetSeverityText(
                                                Result.Severity
                                            )
                                        )
                                        .ColorAndOpacity(
                                            GetSeverityColor(
                                                Result.Severity
                                            )
                                        )
                                        .Font(
                                            FCoreStyle::
                                            GetDefaultFontStyle(
                                                TEXT("Bold"),
                                                9
                                            )
                                        )
                                ]
                        ]

                    + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(SVerticalBox)

                                + SVerticalBox::Slot()
                                .AutoHeight()
                                .Padding(0.0f, 0.0f, 0.0f, 2.0f)
                                [
                                    SNew(STextBlock)
                                        .Text(
                                            FText::FromString(
                                                Result.Problem
                                            )
                                        )
                                        .Font(
                                            FCoreStyle::
                                            GetDefaultFontStyle(
                                                TEXT("Bold"),
                                                10
                                            )
                                        )
                                        .AutoWrapText(true)
                                ]

                            + SVerticalBox::Slot()
                                .AutoHeight()
                                .Padding(0.0f, 2.0f, 0.0f, 0.0f)
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
                                        .AutoWrapText(true)
                                ]

                            + SVerticalBox::Slot()
                                .AutoHeight()
                                .Padding(0.0f, 3.0f, 0.0f, 0.0f)
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
                                        .AutoWrapText(true)
                                ]
                        ]
                ];
        }
    }
}