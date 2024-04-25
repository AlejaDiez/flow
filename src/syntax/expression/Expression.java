package syntax.expression;

/**
 * Represents an expression.
 */
public abstract class Expression {
    /**
     * The expression type.
     */
    public final ExpressionType type;

    /**
     * Creates a new expression.
     *
     * @param type The expression type.
     */
    public Expression(ExpressionType type) {
        this.type = type;
    }

    abstract public int evaluate();

    /**
     * Represents the expression type.
     */
    public enum ExpressionType {
        LITERAL,
        UNARY,
        BINARY
    }
}
