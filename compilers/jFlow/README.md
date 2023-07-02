# jFlow

jFlow is a compiler for Flow written in Java and it's the first compiler I wrote. For now, it is the most complete and stable compiler, and it is the one that I recommend using.

## How to use jFlow

To use jFlow, you need to have:

### Prerequisites

Before running the Java program, ensure that you have Java installed on your machine. You can download it from [here](https://www.oracle.com/java/technologies/downloads/#jdk20).
Check that you have the correct version of Java installed by running the following command in a terminal or command prompt:

```bash
java -version
```

### Compilation

1. Open a terminal or command prompt on your operating system.
2. Navigate to the directory where the jFlow program is saved using the `cd` command:

   ```bash
   cd /path/to/jFlow
   ```

3. Compile the code using the `javac jFlow.java` command:

   ```bash
   javac jFlow.java
   ```

4. If the compilation is successful, it will generate a bytecode file with a `.class` extension.

## Execution

1. In the same terminal or command prompt, run the Java program using the `java jFlow` command followed by the name of the Flow file that you want to execute:

   ```bash
   java jFlow file_name.flow
   ```

   Replace `file_name.flow` with the actual name of the file that you want to execute.

2. The jFlow compiler will start to compile your Flow program, and you will see the output file to run your program.
