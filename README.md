## base

Base layer used by Laith for all his C programming projects.

The entire API is located in `base.h` and contains support for:

- General Base Layer
- Arena Allocators
- Length-Based Strings
- Networking
- Threading
- Time

## Style Guide

Laith's preferred C style guide.

- All types, structs, and unions should be in `PascalCase`
- Enums should be in `Type_Member` case.
- Functions should be in `snake_case` with the domain first.
- Variables should be in `snake_case`.
- Globals should use the `global` keyword in `PascalCase` pre-pended with 'Global'
- Hash defines should be in `ALL_CAPS`
- Utility macros should be in `PascalCase`
- Function macros should be in `snake_case`
- Use three types of comments:
  - `// lt: ` for a plain comment
  - `//- lt: ` subsection within a function or block
  - ```
     ////////////////////////////////
     //~ lt:
    ```
    for file level sections

- All code must be in a `src/` directory
- All programs must contain a `build.sh` and/or a `build.bat`
  - Builds should be released in the `build/` directory
  - Build scripts should support both `debug` and `release` arguments

## License

This library is released under GPL.
