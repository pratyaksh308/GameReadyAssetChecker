#include "Checks.h"

#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInterface.h"
#include "PhysicsEngine/BodySetup.h"


namespace GameReadyAssetCheckerChecks
{
    void RunChecks(const TArray<FAssetData>& SelectedAssets)
    {
        int32 StaticMeshCount = 0;

        for (const FAssetData& Asset : SelectedAssets)
        {
            if (Asset.GetClass() != UStaticMesh::StaticClass())
            {
                continue;
            }

            StaticMeshCount++;

            UE_LOG(
                LogTemp,
                Log,
                TEXT(
                    "Game-Ready Asset Checker: "
                    "Static Mesh found - %s"
                ),
                *Asset.AssetName.ToString()
            );

            UStaticMesh* StaticMesh =
                Cast<UStaticMesh>(Asset.GetAsset());

            if (!StaticMesh)
            {
                continue;
            }

            // ============================================================
            // Collision check
            // ============================================================

            UBodySetup* BodySetup = StaticMesh->GetBodySetup();

            if (!BodySetup)
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT(
                        "Game-Ready Asset Checker: %s - "
                        "ERROR: No collision BodySetup found."
                    ),
                    *Asset.AssetName.ToString()
                );
            }
            else
            {
                const FKAggregateGeom& AggGeom =
                    BodySetup->AggGeom;

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
                        TEXT(
                            "Game-Ready Asset Checker: %s - "
                            "PASS: Simple collision found."
                        ),
                        *Asset.AssetName.ToString()
                    );
                }
                else
                {
                    UE_LOG(
                        LogTemp,
                        Warning,
                        TEXT(
                            "Game-Ready Asset Checker: %s - "
                            "WARNING: No simple collision found."
                        ),
                        *Asset.AssetName.ToString()
                    );
                }
            }

            // ============================================================
            // LOD check
            // ============================================================

            const int32 NumLODs =
                StaticMesh->GetNumLODs();

            if (NumLODs > 1)
            {
                UE_LOG(
                    LogTemp,
                    Log,
                    TEXT(
                        "Game-Ready Asset Checker: %s - "
                        "PASS: %d LODs found."
                    ),
                    *Asset.AssetName.ToString(),
                    NumLODs
                );
            }
            else
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT(
                        "Game-Ready Asset Checker: %s - "
                        "WARNING: No additional LOD found."
                    ),
                    *Asset.AssetName.ToString()
                );
            }

            // ============================================================
            // Naming convention check - Static Mesh
            // ============================================================

            const FString StaticMeshName =
                StaticMesh->GetName();

            if (StaticMeshName.StartsWith(TEXT("SM_")))
            {
                UE_LOG(
                    LogTemp,
                    Log,
                    TEXT(
                        "Game-Ready Asset Checker: %s - "
                        "PASS: Static Mesh naming convention."
                    ),
                    *StaticMeshName
                );
            }
            else
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT(
                        "Game-Ready Asset Checker: %s - "
                        "WARNING: Static Mesh should start with SM_."
                    ),
                    *StaticMeshName
                );
            }

            // ============================================================
            // Material and texture checks
            // ============================================================

            bool bFoundLargeTexture = false;

            const TArray<FStaticMaterial>& StaticMaterials =
                StaticMesh->GetStaticMaterials();

            for (const FStaticMaterial& StaticMaterial :
                StaticMaterials)
            {
                UMaterialInterface* Material =
                    StaticMaterial.MaterialInterface;

                if (!Material)
                {
                    continue;
                }

                // ========================================================
                // Material naming convention
                // ========================================================

                const FString MaterialName =
                    Material->GetName();

                if (UMaterialInstance* MaterialInstance =
                    Cast<UMaterialInstance>(Material))
                {
                    if (MaterialName.StartsWith(TEXT("MI_")))
                    {
                        UE_LOG(
                            LogTemp,
                            Log,
                            TEXT(
                                "Game-Ready Asset Checker: %s - "
                                "PASS: Material Instance naming convention."
                            ),
                            *MaterialName
                        );
                    }
                    else
                    {
                        UE_LOG(
                            LogTemp,
                            Warning,
                            TEXT(
                                "Game-Ready Asset Checker: %s - "
                                "WARNING: Material Instance should start with MI_."
                            ),
                            *MaterialName
                        );
                    }

                    // ----------------------------------------------------
                    // Material Instance textures
                    // ----------------------------------------------------

                    for (const FTextureParameterValue& Parameter :
                        MaterialInstance->TextureParameterValues)
                    {
                        UTexture2D* Texture2D =
                            Cast<UTexture2D>(
                                Parameter.ParameterValue
                            );

                        if (!Texture2D)
                        {
                            continue;
                        }

                        // ------------------------------------------------
                        // Texture naming convention
                        // ------------------------------------------------

                        const FString TextureName =
                            Texture2D->GetName();

                        if (TextureName.StartsWith(TEXT("T_")))
                        {
                            UE_LOG(
                                LogTemp,
                                Log,
                                TEXT(
                                    "Game-Ready Asset Checker: %s - "
                                    "PASS: Texture naming convention."
                                ),
                                *TextureName
                            );
                        }
                        else
                        {
                            UE_LOG(
                                LogTemp,
                                Warning,
                                TEXT(
                                    "Game-Ready Asset Checker: %s - "
                                    "WARNING: Texture should start with T_."
                                ),
                                *TextureName
                            );
                        }

                        // Use imported/source dimensions rather than
                        // current resident dimensions because streamed
                        // textures may only have a small mip loaded.
                        const FIntPoint ImportedSize =
                            Texture2D->GetImportedSize();

                        const int32 Width =
                            ImportedSize.X;

                        const int32 Height =
                            ImportedSize.Y;

                        if (Width >= 4096 || Height >= 4096)
                        {
                            bFoundLargeTexture = true;

                            UE_LOG(
                                LogTemp,
                                Warning,
                                TEXT(
                                    "Game-Ready Asset Checker: %s - "
                                    "WARNING: Large texture found - "
                                    "%s (%dx%d)."
                                ),
                                *Asset.AssetName.ToString(),
                                *Texture2D->GetName(),
                                Width,
                                Height
                            );
                        }
                    }
                }

                // ========================================================
                // Base Material
                // ========================================================

                else
                {
                    if (MaterialName.StartsWith(TEXT("M_")))
                    {
                        UE_LOG(
                            LogTemp,
                            Log,
                            TEXT(
                                "Game-Ready Asset Checker: %s - "
                                "PASS: Material naming convention."
                            ),
                            *MaterialName
                        );
                    }
                    else
                    {
                        UE_LOG(
                            LogTemp,
                            Warning,
                            TEXT(
                                "Game-Ready Asset Checker: %s - "
                                "WARNING: Material should start with M_."
                            ),
                            *MaterialName
                        );
                    }

                    TArray<UTexture*> UsedTextures;

                    Material->GetUsedTextures(
                        UsedTextures,
                        TOptional<EMaterialQualityLevel::Type>(),
                        TOptional<EShaderPlatform>()
                    );

                    for (UTexture* Texture : UsedTextures)
                    {
                        UTexture2D* Texture2D =
                            Cast<UTexture2D>(Texture);

                        if (!Texture2D)
                        {
                            continue;
                        }

                        // ------------------------------------------------
                        // Texture naming convention
                        // ------------------------------------------------

                        const FString TextureName =
                            Texture2D->GetName();

                        if (TextureName.StartsWith(TEXT("T_")))
                        {
                            UE_LOG(
                                LogTemp,
                                Log,
                                TEXT(
                                    "Game-Ready Asset Checker: %s - "
                                    "PASS: Texture naming convention."
                                ),
                                *TextureName
                            );
                        }
                        else
                        {
                            UE_LOG(
                                LogTemp,
                                Warning,
                                TEXT(
                                    "Game-Ready Asset Checker: %s - "
                                    "WARNING: Texture should start with T_."
                                ),
                                *TextureName
                            );
                        }

                        const FIntPoint ImportedSize =
                            Texture2D->GetImportedSize();

                        const int32 Width =
                            ImportedSize.X;

                        const int32 Height =
                            ImportedSize.Y;

                        if (Width >= 4096 || Height >= 4096)
                        {
                            bFoundLargeTexture = true;

                            UE_LOG(
                                LogTemp,
                                Warning,
                                TEXT(
                                    "Game-Ready Asset Checker: %s - "
                                    "WARNING: Large texture found - "
                                    "%s (%dx%d)."
                                ),
                                *Asset.AssetName.ToString(),
                                *Texture2D->GetName(),
                                Width,
                                Height
                            );
                        }
                    }
                }
            }

            if (!bFoundLargeTexture)
            {
                UE_LOG(
                    LogTemp,
                    Log,
                    TEXT(
                        "Game-Ready Asset Checker: %s - PASS: "
                        "No textures at or above 4096x4096 found."
                    ),
                    *Asset.AssetName.ToString()
                );
            }
        }

        UE_LOG(
            LogTemp,
            Log,
            TEXT(
                "Game-Ready Asset Checker: "
                "%d Static Mesh(es) found."
            ),
            StaticMeshCount
        );
    }
}