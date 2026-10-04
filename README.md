# Game-Ready Asset Checker

A lightweight Unreal Engine Editor plugin that scans selected assets for common game-readiness issues and presents the results in a clear, actionable report.

Built for solo and indie Unreal Engine developers who want a quick sanity check before using assets in their projects.

## Features

Game-Ready Asset Checker currently checks:

- **Collision**
  - Detects missing or unavailable simple collision setup on Static Meshes.

- **LOD Availability**
  - Checks whether a Static Mesh has additional LODs.
  - Missing additional LODs are reported as a warning rather than an automatic failure.

- **Texture Resolution**
  - Flags textures at or above the initial `4096 × 4096` resolution threshold.

- **Naming Conventions**
  - Static Mesh → `SM_`
  - Texture → `T_`
  - Material → `M_`
  - Material Instance → `MI_`

- **Missing / Invalid References**
  - Detects obvious missing or invalid material references and Material Instance parent references where Unreal exposes the information reliably.

## How It Works

1. Select supported assets in the Unreal Engine Content Browser.
2. Open:

   **Tools → Game-Ready Asset Checker**

3. Click **Scan Selected Assets**.
4. Review the generated results.

Results are grouped by asset and classified as:

- **PASS**
- **WARNING**
- **ERROR**

Each detected issue includes:

- The affected asset
- The detected problem
- Why it matters
- A suggested action

You can also filter the results by:

- All
- Errors
- Warnings
- Passed

## Supported Assets

The V1 release primarily supports:

- Static Meshes
- Materials
- Material Instances
- Textures directly associated with supported assets

Static Meshes are the primary asset type scanned by the plugin.

## Requirements

- Unreal Engine **5.8.x**
- Windows
- Unreal Engine Editor project

The plugin is intended to run as an **Editor plugin**, not as a runtime component.

## Installation

### From the plugin source

1. Clone or download this repository.
2. Copy the `GameReadyAssetChecker` plugin folder into your Unreal project's:

```text
Plugins/
```

The resulting structure should look like:

```text
YourProject/
└── Plugins/
    └── GameReadyAssetChecker/
        ├── Resources/
        ├── Source/
        └── GameReadyAssetChecker.uplugin
```

3. Open the Unreal Engine project.
4. If prompted to rebuild the plugin, allow Unreal/Visual Studio to build it.
5. Enable the plugin if necessary from:

   **Edit → Plugins**

6. Restart the Unreal Editor if prompted.

## Example Workflow

A typical workflow looks like:

```text
Select Static Meshes
        ↓
Tools → Game-Ready Asset Checker
        ↓
Scan Selected Assets
        ↓
Asset Health Results
        ↓
Fix reported issues
        ↓
Rescan
```

The scanner is designed to provide a quick validation pass rather than replace detailed asset optimization or technical-art workflows.

## V1.0.0 Scope

Version **1.0.0** intentionally keeps the plugin small and focused.

### Included

- Static Mesh validation
- Collision check
- LOD availability check
- Texture resolution check
- Basic naming convention checks
- Obvious missing/invalid reference checks
- PASS / WARNING / ERROR reporting
- Asset grouping
- Result filtering
- Rescan workflow

### Not Included

The following are outside the scope of V1:

- Automatic fixing
- AI recommendations
- Automatic LOD generation
- Automatic collision generation
- Retopology
- Texture compression optimization
- Nanite optimization
- Blueprint validation
- Skeletal Mesh validation
- Niagara validation
- Project-wide optimization scans
- Marketplace publishing/integration
- Cloud services
- Accounts or analytics
- Licensing system

These limitations are intentional. The goal of V1 is to provide a small, reliable asset-health scanner rather than a complete asset optimization suite.

## Project Status

**Version: 1.0.0**

V1 has been implemented and tested against a real Unreal Engine project.

The current release focuses on:

> Reliability over the number of checks, useful results over impressive features, and a small focused tool over a large optimization suite.

Future development will depend on feedback and validation from actual Unreal Engine users.

## Repository

GitHub:

https://github.com/pratyaksh308/GameReadyAssetChecker

## License

No open-source license has been specified for this project yet.