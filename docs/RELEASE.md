# Release

Current public baseline:

- Version: `1.1.0`
- Tag: `v1.1.0`

Release rules used for this repository:

- Only tag verified, committed baselines.
- Do not tag while verification is incomplete.
- Do not claim sanitizer success unless it was actually run and passed.
- Pushing a `v*` tag triggers the GitHub release workflow in `.github/workflows/release.yml`.

The release baseline includes the documented CMake package export and install layout.
