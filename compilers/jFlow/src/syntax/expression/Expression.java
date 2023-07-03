package syntax.expression;

/**
 * Abstract class representing an expression.
 */
public abstract class Expression {
    /**
     * Type of the expression.
     */
    private final Type _type;

    /**
     * Constructor for Expression class.
     *
     * @param type  The type of the expression.
     * @param token The token of the expression.
     */
    public Expression(Type type) {
        _type = type;
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
     * Enumeration for different types of expressions.
     */
    public enum Type {
        NUMBER,
        UNARY,
        BINARY,
        PARENTHESES,
        UNKNOWN;

        @Override
        public String toString() {
            if (this == NUMBER) {
                return "NUMBER EXPRESSION";
            } else if (this == UNARY) {
                return "UNARY EXPRESSION";
            } else if (this == BINARY) {
                return "BINARY EXPRESSION";
            } else if (this == PARENTHESES) {
                return "PARENTHESES EXPRESSION";
            } else {
                return "UNKNOWN EXPRESSION";
            }
        }
    }
}
