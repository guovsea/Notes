# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Is

A personal Obsidian vault — Chinese-language knowledge base covering C++, algorithms, math, physics, semiconductor physics, AI/ML research, and developer tools. No build system, no code to compile, no tests to run.

## Vault Structure

Numbered folders classify by topic (Dewey-Decimal style):

- `100Cpp` — C++ (largest section; C++20, templates, move semantics)
- `120SoftwareDesign` — Design patterns, architecture
- `130Algorithm` — Algorithms and data structures
- `200Math` — Calculus, linear algebra, FEM
- `210Physics` — PDEs, weak forms
- `220Semiconductor` — Semiconductor physics (numbered series)
- `300AI` — AI/ML research, prompt engineering, Claude
- `400Tools` — Git, CMake, Docker, VS Code
- `500Projects` — Project-specific notes (tinystl, FEM solver, parsing)
- `600Life` — Daily logs, productivity
- `800Meta` — Vault conventions
- `900Archives` — Migrated older notes

## Tag Convention

Defined in `800Meta/标签规范.md`. Key rule: **folders for "where", tags for "what state/context"**. Do not duplicate folder structure in tags.

- Status: `#todo` `#draft` `#done`
- Type: `#howto` `#snippet` `#research` `#reference` `#diary` `#idea`
- Context: `#interview` `#leetcode` `#pitfall` `#fem` `#series`

## Obsidian Config

- `.obsidian/` is tracked in git except `workspace*` files (see `.gitignore`)
- Community plugins: obsidian-git (auto-backup), hover-editor, mermaid-tools, easy-typing, file-cleaner, paste-image-rename, auto-link-title, file-explorer-note-count, style-settings
- Attachments go to `./attachments` relative to note location
- Vim key bindings disabled; strict line breaks enabled

## Working With This Vault

- Notes are in **Chinese with English technical terms**. Keep this style.
- Use Obsidian wikilinks `[[note name]]` for internal references, not markdown links.
- Use frontmatter properties (title, date, tags, aliases) on new notes.
- If you want invent new tags without checking, follow the tag convention  `800Meta/标签规范.md`. 
- Git commits are handled by the obsidian-git plugin. Manual commits should follow the existing pattern: short descriptions in Chinese or English.
