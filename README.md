<div align="center">
    <img src="assets/flow.png" alt="Logo" width="120" height="120">
    <br>
    <h1 align="center">Flow Programming Language</h1>
    <p align="center">
        Flow is the outcome of my first shot at creating a programming language from scratch. I took inspiration from the syntax of various languages that I've had a blast programming in. With Flow, I brought together the coolest parts of those languages and added some extra goodies that I believe are absolutely necessary.
    </p>
</div>

## Table of Contents

-   [Get Started](#get-started)
    -   [Installation](#installation)
    -   [Usage](#usage)
-   [Flow Syntax](#flow-syntax)
    -   [Key Features](#key-features)
    -   [Syntax Example](#syntax-example)
-   [Changelog](#changelog)
-   [Contribution](#contribution)
-   [License](#license)

## Get Started

Flow Compiler is a simple and lightweight compiler for the Flow programming language. It is designed to be easy to use and understand, making it ideal for beginners and experienced programmers alike. The compiler is written in C and can be built on most platforms, including Windows, macOS, and Linux.

### Installation

To get started with Flow, you need to clone the repository and build the project using the following commands:

```bash
git clone https://github.com/AlejaDiez/flow.git
cd flow
make clean
make
```

This will generate the `flow` executable in the bin directory, which you can use to compile Flow programs.

### Usage

To compile a Flow program, use the following command:

```bash
./bin/flow <input_file>.flow
```

This will generate an output file with the same name as the input file but with a `.s` extension. You can then compile the generated assembly code using a C compiler, such as GCC, to create an executable.

```bash
gcc -o <output_file> <input_file>.s
```

You can then run the compiled executable to execute the Flow program.

```bash
./<output_file> [args...]
```

## Flow Syntax

Flow is a modern and concise programming language designed for simplicity and expressiveness. It has its own syntax that is based on other very popular languages and provide a powerful and readable programming experience.

### Key Features

-   **Simplicity**: Flow adopts a minimalist syntax, minimizing verbosity and complexity to promote clean and concise code.
-   **Familiarity**: Flow's syntax draws inspiration from popular languages, making it easy to learn and transition to.
-   **Clear Structure**: Flow enforces a consistent code structure using indentation for code blocks, improving readability and maintainability.
-   **Expressive Expressions**: Flow supports expressive expressions, enabling complex operations to be written in a straightforward manner.
-   **Error Handling**: Flow includes robust error handling mechanisms, ensuring reliable code execution and enabling graceful error recovery.
-   **Modularity**: Flow encourages modular programming through the use of packages, promoting code organization and reusability.
-   **Object-Oriented Programming**: Flow supports object-oriented programming with classes, objects, and inheritance, facilitating code organization and reuse.
-   **Concise Control Flow**: Flow provides concise control flow constructs, such as if-else statements and loops, allowing for expressive and compact code.
-   **Extensibility**: Flow allows the creation and utilization of external libraries and modules, enabling the extension of its functionality and facilitating code reuse.

### Syntax Example

```flow
# Calculate the pi number using the Leibniz formula
fun calculatePI(n: int): float {
    var pi: float = 0.0;

    loop (var i: int = 0; i < n; i++) {
        const term: float = (i % 2 == 0 ? 1.0 : -1.0) / (2.0 * i + 1.0);

        pi += term;
    }
    return 4 * pi
}

const pi: float = calculatePI(1000000);

print(f"Hello, World! Here I am {pi}");
```

## Changelog

Include primitive data types and symbol type declarations.

-   Update the `decl.h` file to update all the functions with the new primitive data types.
-   Update the `defs.h` file to include primitive data types and symbol type declarations, also update SYMBOL struct to include the new symbol type and primitive data types.
-   Update the `decl.c`, `expr.c` and `stmt.c` files to update all the functions with the new primitive data types.
-   Update the `sym.c` file to include the new primitive data types and symbol type declarations.
-   Update the `tree.c` file to include the primitive data types that are used in the tree.

## Contribution

Thank you for your interest in contributing to this project! Contributions are what make the open-source community such an amazing place to learn, inspire, and create. Any contributions you make are greatly appreciated.

1.  Fork the repository
2.  Create a new branch

    -   Feature

        `git checkout -b feature/AmazingFeature`

    -   Bugfix

        `git checkout -b bugfix/FixBug`

3.  Make your changes and ensure everything works as expected
4.  Commit your changes

    -   Feature

        `git commit -m "Add feature: your feature description"`

    -   Bugfix

        `git commit -m "Solve bug: your bug description"`

5.  Push the branch to your repository

    -   Feature

        `git push origin feature/AmazingFeature`

    -   Bugfix

        `git push origin bugfix/FixBug`

6.  Open a pull request with a clear explanation of your changes

## License

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">Flow Programming Language</span> by <span xmlns:cc="http://creativecommons.org/ns#" property="cc:attributionName">Alejandro Diez Bermejo</span> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>.
