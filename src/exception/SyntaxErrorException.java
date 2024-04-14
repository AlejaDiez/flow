package exception;

/**
 * SyntaxErrorException
 */
public class SyntaxErrorException extends Exception {
    /**
     * The serial version UID.
     */
    private static final long serialVersionUID = 1L;

    /**
     * Creates a new syntax error exception.
     *
     * @param inputName The name of the input.
     * @param input     The input string.
     * @param line      The line number of the error.
     * @param column    The column number of the error.
     */
    public SyntaxErrorException(String inputName, String input, int line, int column) {
        super(
                String.format("%s\n%s\n%s",
                        input.split("\n")[line - 1],
                        " ".repeat(column - 1) + "\033[31m^\033[0m",
                        String.format("\033[31mSyntaxError\033[0m: invalid syntax (<%s>, line %d)", inputName, line)));
    }
}
