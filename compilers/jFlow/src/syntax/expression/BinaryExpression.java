package syntax.expression;

import syntax.Token;

/**
 * A binary expression.
 */
public final class BinaryExpression extends Expression {
    /**
     * The left expression.
     */
    private final Expression _left;
    /**
     * The token of the expression.
     */
    private final Token _operator;
    /**
     * The right expression.
     */
    private final Expression _right;

    /**
     * Constructor for BinaryExpression class.
     *
     * @param left     The left expression.
     * @param operator The token of the expression.
     * @param right    The right expression.
     */
    public BinaryExpression(Expression left, Token operator, Expression right) {
        super(Expression.Type.BINARY);
        _left = left;
        _operator = operator;
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
     * Get the token of the expression.
     *
     * @return The token of the expression.
     */
    public Token getOperator() {
        return _operator;
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
