#include "Checks.h"

#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInterface.h"
#include "PhysicsEngine/BodySetup.h"


namespace GameReadyAssetCheckerChecks
{
    TArray<FCheckResult> RunChecks(
        const TArray<FAssetData>& SelectedAssets)
    {
        TArray<FCheckResult> Results;

        int32 StaticMeshCount = 0;

        for (const FAssetData& Asset : SelectedAssets)
        {
            if (Asset.GetClass() != UStaticMesh::StaticClass())
            {
                continue;
            }

            StaticMeshCount++;

            const FString AssetName =
                Asset.AssetName.ToString();

            UE_LOG(
                LogTemp,
                Log,
                TEXT(
                    "Game-Ready Asset Checker: "
                    "Static Mesh found - %s"
                ),
                *AssetName
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

            UBodySetup* BodySetup =
                StaticMesh->GetBodySetup();

            if (!BodySetup)
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT(
                        "Game-Ready Asset Checker: %s - "
                        "ERROR: No collision BodySetup found."
                    ),
                    *AssetName
                );

                Results.Add(
                    {
                        AssetName,
                        ECheckSeverity::Error,
                        TEXT("No collision BodySetup found."),
                        TEXT("The Static Mesh does not have a usable collision setup."),
                        TEXT("Add or generate collision for the Static Mesh.")
                    }
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
                        *AssetName
                    );

                    Results.Add(
                        {
                            AssetName,
                            ECheckSeverity::Passed,
                            TEXT("Simple collision found."),
                            TEXT("The Static Mesh has usable simple collision geometry."),
                            TEXT("No action required.")
                        }
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
                        *AssetName
                    );

                    Results.Add(
                        {
                            AssetName,
                            ECheckSeverity::Warning,
                            TEXT("No simple collision found."),
                            TEXT("The Static Mesh may not have usable collision for gameplay or physics."),
                            TEXT("Add or generate simple collision for the Static Mesh.")
                        }
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
                    *AssetName,
                    NumLODs
                );

                Results.Add(
                    {
                        AssetName,
                        ECheckSeverity::Passed,
                        FString::Printf(
                            TEXT("%d LODs found."),
                            NumLODs
                        ),
                        TEXT("The Static Mesh has additional LOD levels available."),
                        TEXT("No action required.")
                    }
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
                    *AssetName
                );

                Results.Add(
                    {
                        AssetName,
                        ECheckSeverity::Warning,
                        TEXT("No additional LOD found."),
                        TEXT("Additional LODs can reduce rendering cost when the mesh is viewed from a distance."),
                        TEXT("Consider adding an additional LOD if appropriate for the asset.")
                    }
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

                Results.Add(
                    {
                        AssetName,
                        ECheckSeverity::Passed,
                        TEXT("Static Mesh naming convention is valid."),
                        TEXT("The asset follows the expected Unreal naming convention."),
                        TEXT("No action required.")
                    }
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

                Results.Add(
                    {
                        AssetName,
                        ECheckSeverity::Warning,
                        TEXT("Static Mesh should start with SM_."),
                        TEXT("Consistent asset naming makes assets easier to identify and manage."),
                        TEXT("Rename the Static Mesh using the SM_ prefix.")
                    }
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

                // ========================================================
                // Missing / invalid material reference
                // ========================================================

                if (!Material)
                {
                    UE_LOG(
                        LogTemp,
                        Error,
                        TEXT(
                            "Game-Ready Asset Checker: %s - "
                            "ERROR: Static Mesh has a missing material reference."
                        ),
                        *AssetName
                    );

                    Results.Add(
                        {
                            AssetName,
                            ECheckSeverity::Error,
                            TEXT("Static Mesh has a missing material reference."),
                            TEXT("A material slot does not have a valid material assigned."),
                            TEXT("Assign a valid Material or Material Instance to the material slot.")
                        }
                    );

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

                        Results.Add(
                            {
                                AssetName,
                                ECheckSeverity::Passed,
                                FString::Printf(
                                    TEXT("%s follows the Material Instance naming convention."),
                                    *MaterialName
                                ),
                                TEXT("The Material Instance follows the expected Unreal naming convention."),
                                TEXT("No action required.")
                            }
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

                        Results.Add(
                            {
                                AssetName,
                                ECheckSeverity::Warning,
                                FString::Printf(
                                    TEXT("%s should start with MI_."),
                                    *MaterialName
                                ),
                                TEXT("Consistent material naming makes assets easier to identify and manage."),
                                TEXT("Rename the Material Instance using the MI_ prefix.")
                            }
                        );
                    }

                    // ====================================================
                    // Missing / invalid Material Instance parent
                    // ====================================================

                    UMaterialInterface* ParentMaterial =
                        MaterialInstance->Parent;

                    if (!ParentMaterial)
                    {
                        UE_LOG(
                            LogTemp,
                            Error,
                            TEXT(
                                "Game-Ready Asset Checker: %s - "
                                "ERROR: Material Instance has a missing parent material."
                            ),
                            *MaterialName
                        );

                        Results.Add(
                            {
                                AssetName,
                                ECheckSeverity::Error,
                                FString::Printf(
                                    TEXT("%s has a missing parent material."),
                                    *MaterialName
                                ),
                                TEXT("A Material Instance depends on its parent material to provide its base material setup."),
                                TEXT("Assign a valid parent material to the Material Instance.")
                            }
                        );
                    }
                    else
                    {
                        UE_LOG(
                            LogTemp,
                            Log,
                            TEXT(
                                "Game-Ready Asset Checker: %s - "
                                "PASS: Material Instance parent reference found."
                            ),
                            *MaterialName
                        );

                        Results.Add(
                            {
                                AssetName,
                                ECheckSeverity::Passed,
                                FString::Printf(
                                    TEXT("%s has a valid parent material reference."),
                                    *MaterialName
                                ),
                                TEXT("The Material Instance has a valid parent material."),
                                TEXT("No action required.")
                            }
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

                            Results.Add(
                                {
                                    AssetName,
                                    ECheckSeverity::Passed,
                                    FString::Printf(
                                        TEXT("%s follows the Texture naming convention."),
                                        *TextureName
                                    ),
                                    TEXT("The texture follows the expected Unreal naming convention."),
                                    TEXT("No action required.")
                                }
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

                            Results.Add(
                                {
                                    AssetName,
                                    ECheckSeverity::Warning,
                                    FString::Printf(
                                        TEXT("%s should start with T_."),
                                        *TextureName
                                    ),
                                    TEXT("Consistent texture naming makes assets easier to identify and manage."),
                                    TEXT("Rename the texture using the T_ prefix.")
                                }
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
                                *AssetName,
                                *Texture2D->GetName(),
                                Width,
                                Height
                            );

                            Results.Add(
                                {
                                    AssetName,
                                    ECheckSeverity::Warning,
                                    FString::Printf(
                                        TEXT(
                                            "Large texture found - %s (%dx%d)."
                                        ),
                                        *Texture2D->GetName(),
                                        Width,
                                        Height
                                    ),
                                    TEXT("Very large textures can increase memory usage and may be unnecessary for the asset."),
                                    TEXT("Consider reducing the texture resolution if the asset does not require it.")
                                }
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

                        Results.Add(
                            {
                                AssetName,
                                ECheckSeverity::Passed,
                                FString::Printf(
                                    TEXT("%s follows the Material naming convention."),
                                    *MaterialName
                                ),
                                TEXT("The Material follows the expected Unreal naming convention."),
                                TEXT("No action required.")
                            }
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

                        Results.Add(
                            {
                                AssetName,
                                ECheckSeverity::Warning,
                                FString::Printf(
                                    TEXT("%s should start with M_."),
                                    *MaterialName
                                ),
                                TEXT("Consistent material naming makes assets easier to identify and manage."),
                                TEXT("Rename the Material using the M_ prefix.")
                            }
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

                            Results.Add(
                                {
                                    AssetName,
                                    ECheckSeverity::Passed,
                                    FString::Printf(
                                        TEXT("%s follows the Texture naming convention."),
                                        *TextureName
                                    ),
                                    TEXT("The texture follows the expected Unreal naming convention."),
                                    TEXT("No action required.")
                                }
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

                            Results.Add(
                                {
                                    AssetName,
                                    ECheckSeverity::Warning,
                                    FString::Printf(
                                        TEXT("%s should start with T_."),
                                        *TextureName
                                    ),
                                    TEXT("Consistent texture naming makes assets easier to identify and manage."),
                                    TEXT("Rename the texture using the T_ prefix.")
                                }
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
                                *AssetName,
                                *Texture2D->GetName(),
                                Width,
                                Height
                            );

                            Results.Add(
                                {
                                    AssetName,
                                    ECheckSeverity::Warning,
                                    FString::Printf(
                                        TEXT(
                                            "Large texture found - %s (%dx%d)."
                                        ),
                                        *Texture2D->GetName(),
                                        Width,
                                        Height
                                    ),
                                    TEXT("Very large textures can increase memory usage and may be unnecessary for the asset."),
                                    TEXT("Consider reducing the texture resolution if the asset does not require it.")
                                }
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
                    *AssetName
                );

                Results.Add(
                    {
                        AssetName,
                        ECheckSeverity::Passed,
                        TEXT("No textures at or above 4096x4096 found."),
                        TEXT("The scanned textures are below the current large-texture threshold."),
                        TEXT("No action required.")
                    }
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

        return Results;
    }
}