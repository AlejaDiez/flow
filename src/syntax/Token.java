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
        PLUS, MINUS, STAR, SLASH,
        INTEGER,
        EOF
    }
}
