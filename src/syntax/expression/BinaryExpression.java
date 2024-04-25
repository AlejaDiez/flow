package syntax.expression;

import syntax.Token;

public class BinaryExpression extends Expression {
    /**
     * The left expression.
     */
    public final Expression left;
    /**
     * The token of the expression.
     */
    public final Token operator;
    /**
     * The right expression.
     */
    public final Expression right;

    /**
     * Creates a new binary expression.
     *
     * @param left     The left expression.
     * @param operator The token of the expression.
     * @param right    The right expression.
     */
    public BinaryExpression(Expression left, Token operator, Expression right) {
        super(ExpressionType.BINARY);
        this.left = left;
        this.operator = operator;
        this.right = right;
    }

    public int evaluate() {
        final int leftValue = left.evaluate();
        final int rightValue = right.evaluate();

        switch (operator.type) {
            case PLUS:
                return leftValue + rightValue;
            case MINUS:
                return leftValue - rightValue;
            case STAR:
                return leftValue * rightValue;
            case SLASH:
                return leftValue / rightValue;
            default:
                throw new IllegalArgumentException("Invalid operator.");
        }
    }

    @Override
    public String toString() {
        return String.format(
                "BinaryExpression[%s](%s %s)",
                operator.type.toString(), left.toString(), right.toString());
    }
}
