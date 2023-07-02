package syntax;

/**
 * Class representing a token.
 */
public final class Token<T> {
    /**
     * Type of the token.
     */
    private final Type _type;
    /**
     * Value of the token.
     */
    private final T _value;
    /**
     * Line number where the token is found.
     */
    private final int _line;
    /**
     * Column number where the token is found.
     */
    private final int _column;

    /**
     * Constructor for Token class.
     *
     * @param type   The type of the token.
     * @param value  The value of the token.
     * @param line   The line number where the token is found.
     * @param column The column number where the token is found.
     */
    public Token(Type type, T value, int line, int column) {
        _type = type;
        _value = value;
        _line = line;
        _column = column;
    }

    /**
     * Get the type of the token.
     *
     * @return The type of the token.
     */
    public Type getType() {
        return _type;
    }

    /**
     * Get the value of the token.
     *
     * @return The value of the token.
     */
    public T getValue() {
        return _value;
    }

    /**
     * Get the line number where the token is found.
     *
     * @return The line number where the token is found.
     */
    public int getLine() {
        return _line;
    }

    /**
     * Get the column number where the token is found.
     *
     * @return The column number where the token is found.
     */
    public int getColumn() {
        return _column;
    }

    @Override
    public String toString() {
        if (_type == Token.Type.UNKNOWN || _type == Token.Type.WHITESPACE) {
            return String.format("Token<%s>[value='%s', line=%d, column=%d]", _type.toString(), _value.toString(),
                    _line, _column);
        } else if (_type == Token.Type.EOF) {
            return String.format("Token<%s>[line=%d, column=%d]", _type.toString(), _line, _column);
        } else {
            return String.format("Token<%s>[value=%s, line=%d, column=%d]", _type.toString(), _value.toString(), _line,
                    _column);
        }
    }

    /**
     * Enumeration for different types of tokens.
     */
    public enum Type {
        /**
         * Numeric value
         */
        NUMBER,
        /**
         * Addition operator
         */
        PLUS,
        /**
         * Subtraction operator
         */
        MINUS,
        /**
         * Multiplication operator
         */
        MULTIPLY,
        /**
         * Division operator
         */
        DIVIDE,
        /**
         * Left parenthesis
         */
        LPAREN,
        /**
         * Right parenthesis
         */
        RPAREN,
        /**
         * Whitespace
         */
        WHITESPACE,
        /**
         * End of file
         */
        EOF,
        /**
         * Unknown token
         */
        UNKNOWN;

        /**
         * Get the type of the given character.
         * 
         * @param c The character to get the type of.
         * @return The type of the given character.
         */
        public static Type fromChar(char c) {
            if (c >= '0' && c <= '9') {
                return NUMBER;
            } else if (c == '+') {
                return PLUS;
            } else if (c == '-') {
                return MINUS;
            } else if (c == '*') {
                return MULTIPLY;
            } else if (c == '/') {
                return DIVIDE;
            } else if (c == '(') {
                return LPAREN;
            } else if (c == ')') {
                return RPAREN;
            } else if (c == ' ' || c == '\t' || c == '\n') {
                return WHITESPACE;
            } else if (c == '\0') {
                return EOF;
            } else {
                return UNKNOWN;
            }
        }

        /**
         * Check if the given character is of the given type.
         * 
         * @param c The character to check.
         * @return True if the character is of the given type, false otherwise.
         */
        public boolean equals(char c) {
            return fromChar(c) == this;
        }

        @Override
        public String toString() {
            if (this == NUMBER) {
                return "NUMBER";
            } else if (this == PLUS) {
                return "PLUS";
            } else if (this == MINUS) {
                return "MINUS";
            } else if (this == MULTIPLY) {
                return "MULTIPLY";
            } else if (this == DIVIDE) {
                return "DIVIDE";
            } else if (this == LPAREN) {
                return "LEFT PARENTHESIS";
            } else if (this == RPAREN) {
                return "RIGHT PARENTHESIS";
            } else if (this == WHITESPACE) {
                return "WHITESPACE";
            } else if (this == EOF) {
                return "END OF FILE";
            } else {
                return "UNKNOWN";
            }
        }
    }
}
