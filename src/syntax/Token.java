package syntax;

/**
 * Represents a token in the source code.
 */
public class Token {
    /**
     * The type of the token.
     */
    public final TokenType type;
    /**
     * The value of the token.
     */
    public final Object value;
    /**
     * The line number of the token.
     */
    public final int line;
    /**
     * The column number of the token.
     */
    public final int column;

    /**
     * Creates a new token.
     *
     * @param type   The type of the token.
     * @param value  The value of the token.
     * @param line   The line number of the token.
     * @param column The column number of the token.
     */
    public Token(TokenType type, Object value, int line, int column) {
        this.type = type;
        this.value = value;
        this.line = line;
        this.column = column;
    }

    @Override
    public String toString() {
        return String.format(
                "Token{type=%s, value=%s, line=%d, column=%d}",
                type, value, line, column);
    }

    /**
     * The type of the token.
     */
    public enum TokenType {
        // Operators
        PLUS(1), MINUS(1), STAR(2), SLASH(2),
        // Literals
        INTEGER(0),
        // End of file
        EOF(0);

        /**
         * The precedence of the token.
         */
        final int precedence;

        private TokenType(int precedence) {
            this.precedence = precedence;
        }
    }
}
