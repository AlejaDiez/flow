package syntax.expression;

import syntax.Token;

public final class BinaryExpression extends Expression {
    /**
     * The left expression.
     */
    private final Expression _left;
    /**
     * The right expression.
     */
    private final Expression _right;

    /**
     * Constructor for BinaryExpression class.
     *
     * @param left  The left expression.
     * @param token The token of the expression.
     * @param right The right expression.
     */
    public BinaryExpression(Expression left, Token token, Expression right) {
        super(Expression.Type.BINARY, token);
        _left = left;
        _right = right;
    }

    /**
     * Get the left expression.
     *
     * @return The left expression.
     */
    public Expression getLeft() {
        return _left;
    }

    /**
     * Get the right expression.
     *
     * @return The right expression.
     */
    public Expression getRight() {
        return _right;
    }
}
