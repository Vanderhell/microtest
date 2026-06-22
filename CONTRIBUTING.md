# Contributing

Contributions are welcome for:

- Bug fixes.
- New assertions that preserve the current API style.
- Docs, packaging, and verification coverage.
- Build-system and portability improvements.

Do not contribute changes that introduce:

- Dynamic allocation.
- Hidden dependencies.
- Global warning suppression.
- Weaker tests.
- New behavior that is not documented and verified.

Requirements:

- Keep the project C99.
- Preserve single-header integration.
- Keep examples conceptually compilable.
- Update verification evidence when behavior changes.

Before opening a change, run the available project tests and the applicable build path for the area you touched.
