# Compiler laboratory

Compiler Laboratory: Design and implementation of a custom programming language and its compiler.

**Course:** Compilers  
**Semester:** 2026-2  
**Institution:** Faculty of Science, UNAM

> You are currently viewing the **English version** of this document.  
> Also available in Spanish:
> [![Español](https://img.shields.io/badge/idioma-Español-blue)](README-es.md)

**Work in Progress** - README will be completed incrementally

## Overview

This repository documents the development of a complete compiler as part of the Compilers 2026-2 course. The project evolves through 11 focused practices covering lexical, syntax, and semantic analysis to code generation, culminating in a final integrative project: a fully functional programming language designed for a specific domain [to be defined] like robot control, game scripting, automation, etc.

## Course Structure

The laboratory is divided into **11 focused practices** and a **final integrative project**:

| Module | Practices | Technical Focus |
|--------|-----------|-----------------|
| **I. Lexical Analysis** | 4 Practices | Finite Automata Design (DFA/NFA), regular expressions, buffer management and efficient tokenization. **Scanner Construction**. |
| **II. Syntactic Analysis** | 4 Practices | Context-free grammars, parsing algorithms (top-down/bottom-up), syntax error handling and Abstract Syntax Tree (AST) construction. **Parser Construction**. |
| **III. Semantic Analysis** | 2 Practices | Type validation, scope management (scopes), symbol tables and logical inference rules. The compiler's **"brain"**. |
| **IV. Code Generation** | 1 Practice | Translation of validated AST to intermediate representation (3-address Code or P-Code) ready for execution in a simplified virtual machine. |

### Module Progress

| Module | Status | Practices Complete |
|--------|--------|-------------------|
| Lexical Analysis | Started | 1/4 |
| Syntactic Analysis | Not Started | 0/4 |
| Semantic Analysis | Not Started | 0/2 |
| Code Generation | Not Started | 0/1 |

## Technologies and Tools

### Core Implementation
**Language:** C 

### Dependencies
- **Standard Libraries:** 
  - `stdio.h` - I/O operations
  - `stdlib.h` - Memory management
  - `string.h` - String manipulation
- **Tools:** 
  - `flex` - Fast lexical analyzer generator
  - `bison` - Parser generator


### Input/Output Contract

All programs follow the **Unix pipeline contract**:

| Stream | Purpose | Usage |
|--------|---------|-------|
| **stdin** | Source code input | Read from standard input or file redirection |
| **stdout** | Results (tokens, AST, code) | Output compilation results |
| **stderr** | Error messages | Report compilation errors and warnings |

## How to Use

Each practice lives in its own directory. Navigate to the corresponding folder and follow
its README for compilation and execution instructions.

| Practice | Directory | Description |
|----------|-----------|-------------|
| Practice 1 | [`regex_to_nfa/`](regex_to_nfa/README.md) | From Regular Expressions to NFA |

## Branch strategy

This repository implements a **modified Git Flow** adapted specifically for the incremental methodology of the Compilers Laboratory course. The strategy balances the rigor of Git Flow with the academic requirements of demonstrating progressive learning through eleven distinct practices and a culminating integrative project.

The branching model maintains two permanent branches: `main` serves as the stable release branch for official submissions to the instructor, while `develop` functions as the integration branch for ongoing development. All development work originates from develop, which acts as the single source of truth for the current state of the project. The model is supplemented by ephemeral practice branches that isolate individual assignments.

### Supporting Branch Architecture

The repository employs two categories of supporting branches to organize work across different temporal and logical scopes.
Module branches represent long-lived feature branches aligned with the four primary course modules. These branches are created from develop at the beginning of each module and remain active throughout the implementation of all practices within that module. The four module branches (`module-1-lexical`, `module-2-syntactic`, `module-3-semantic`, and `module-4-generation`) aggregate related practices within each major compiler component.
Practice branches constitute short-lived development branches dedicated to individual assignments. Each practice branch follows the naming convention `practice-[number]-[descriptive-feature]`, where the number corresponds to the practice sequence and the feature descriptor identifies the specific functionality being implemented. These branches are created from develop, used for focused implementation work, and subsequently merged into their respective module branches upon completion. Practice branches are deleted after successful integration to maintain repository hygiene.

### Development workflow

The development cycle follows a hierarchical merge pattern designed to accommodate both the modular organization of compiler components and the incremental delivery requirements of the course. Each practice must complete its full integration cycle before the subsequent practice begins, ensuring that each new assignment builds upon all previously completed work.

When beginning a new practice, a practice-specific branch is created from the current state of develop. This isolated environment allows focused implementation work while preserving the stability of the main development line. All commits follow the [Conventional Commits](https://www.bavaga.com/blog/2025/01/27/my-ultimate-conventional-commit-types-cheatsheet/) specification to establish a semantic version history that documents the evolution of each compiler component. Upon completing the implementation and testing of a practice, the practice branch merges into its corresponding module branch, consolidating related functionality within the appropriate compiler component.

Each completed practice requires formal delivery to the course instructor. Following the practice integration into the module branch, a release branch is created from the module branch for final preparation. This release branch undergoes final documentation updates and verification procedures before merging into main with an accompanying version tag that identifies the specific practice delivery. The release subsequently propagates back to develop to ensure that all refinements made during the release preparation remain synchronized across the active development line. This synchronization step is critical because it ensures that when the next practice branches from develop, it will contain all the code from previous practices plus any improvements made during release preparation.

When all practices within a module are complete, the module branch merges into develop for comprehensive integration testing. A module-level release is then created from develop, merging into main with a module completion tag that confirms the successful implementation of an entire compiler phase.

```bash
# Practice initiation and implementation
git checkout develop
git checkout -b practice-01-dfa
git commit -m "feat(lexer): implement DFA for identifier recognition"

# Practice completion and module integration
git checkout module-1-lexical
git merge practice-01-dfa

# Individual practice delivery to instructor
git checkout -b release/v1.1-practice-01
git checkout main
git merge --no-ff release/v1.1-practice-01
git tag -a v1.1-practice-01 -m "Practice 1: DFA Implementation - Official Delivery"
git checkout develop
git merge --no-ff release/v1.1-practice-01
# develop now contains practice-01, ready for practice-02 to branch

# Continue with subsequent practices in the same module
# Each practice follows the same cycle, branching from updated develop

# Module completion after all practices are delivered
git checkout develop
git merge module-1-lexical
git checkout -b release/v1.0-module-lexical
git checkout main
git merge --no-ff release/v1.0-module-lexical
git tag -a v1.0-module-lexical -m "Module I: Lexical Analysis Complete"
git checkout develop
git merge --no-ff release/v1.0-module-lexical
```

### Release tags
Major deliverables are tagged for version tracking. Module completions receive tags following the pattern v1.0-module-lexical, v2.0-module-syntactic, v3.0-module-semantic, and v4.0-final-project. Individual practice deliveries receive intermediate tags such as v1.1-practice-01, v1.2-practice-02, and so forth, allowing both the instructor and the development team to reference specific submission points throughout the semester.

The final integrative project will be developed on a dedicated branch named `intregated-project`, which will be created from develop after all module work is complete.


## License

MIT License - see [LICENSE](LICENSE)

<div align="center">

**[Back to top](#compiler-laboratory)**

</div>

