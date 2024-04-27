package syntax.expression;

import exception.EvaluatorErrorException;

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

    /**
     * Evaluates the expression.
     *
     * @return The result of the evaluation.
     * @throws EvaluatorErrorException If an error occurs during the evaluation.
     */
    public abstract int evaluate() throws EvaluatorErrorException;

    /**
     * Represents the expression type.
     */
    public enum ExpressionType {
        LITERAL,
        UNARY,
        BINARY;
    }
}
