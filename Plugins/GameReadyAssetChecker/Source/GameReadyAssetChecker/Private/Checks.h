#pragma once

#include "AssetRegistry/AssetData.h"
#include "CoreMinimal.h"

namespace GameReadyAssetCheckerChecks
{
    enum class ECheckSeverity
    {
        Passed,
        Warning,
        Error
    };

    struct FCheckResult
    {
        FString AssetName;
        ECheckSeverity Severity;
        FString Problem;
        FString WhyItMatters;
        FString SuggestedAction;
    };

    TArray<FCheckResult> RunChecks(
        const TArray<FAssetData>& SelectedAssets
    );
}