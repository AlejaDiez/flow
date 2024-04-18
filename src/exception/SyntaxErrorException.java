package exception;

import syntax.Lexer;

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
     * @param lexer  The lexer to be used.
     * @param line   The line number of the error.
     * @param column The column number of the error.
     */
    public SyntaxErrorException(Lexer lexer, int line, int column) {
        this("invalid syntax", lexer, line, column);
    }

    /**
     * Creates a new syntax error exception.
     *
     * @param error  The error message.
     * @param lexer  The lexer to be used.
     * @param line   The line number of the error.
     * @param column The column number of the error.
     */
    public SyntaxErrorException(String error, Lexer lexer, int line, int column) {
        super(
                String.format("%s\n%s\n%s",
                        lexer.getInputLine(line),
                        " ".repeat(column - 1) + "\033[31m^\033[0m",
                        String.format("\033[31mSyntaxError\033[0m: %s (<%s>, line %d)",
                                error, lexer.getInputName(), line)));
    }
}
