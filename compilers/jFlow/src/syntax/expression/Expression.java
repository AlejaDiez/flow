package syntax.expression;

import syntax.Token;

/**
 * Abstract class representing an expression.
 */
public abstract class Expression {
    /**
     * Type of the expression.
     */
    private final Type _type;

    /**
     * The token of the expression.
     */
    private final Token _token;

    /**
     * Constructor for Expression class.
     *
     * @param type  The type of the expression.
     * @param token The token of the expression.
     */
    public Expression(Type type, Token token) {
        _type = type;
        _token = token;
    }

    /**
     * Get the type of the expression.
     * 
     * @return The type of the expression.
     */
    public Type getType() {
        return _type;
    }

    /**
     * Get the token representing the expression.
     * 
     * @return The token representing the expression.
     */
    public Token getToken() {
        return _token;
    }

    /**
     * Enumeration for different types of expressions.
     */
    public enum Type {
        NUMBER,
        UNARY,
        BINARY,
        UNKNOWN;

        @Override
        public String toString() {
            if (this == NUMBER) {
                return "NUMBER EXPRESSION";
            } else if (this == UNARY) {
                return "UNARY EXPRESSION";
            } else if (this == BINARY) {
                return "BINARY EXPRESSION";
            } else {
                return "UNKNOWN EXPRESSION";
            }
        }
    }
}
