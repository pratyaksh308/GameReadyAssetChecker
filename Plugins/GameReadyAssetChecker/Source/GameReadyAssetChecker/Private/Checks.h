#pragma once
#pragma once

#include "AssetRegistry/AssetData.h"
#include "CoreMinimal.h"

namespace GameReadyAssetCheckerChecks
{
    void RunChecks(const TArray<FAssetData>& SelectedAssets);
}
