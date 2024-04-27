package syntax.expression;

import exception.EvaluatorErrorException;
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

    @Override
    public int evaluate() throws EvaluatorErrorException {
        final int left = this.left.evaluate();
        final int right = this.right.evaluate();

        switch (this.operator.type) {
            case PLUS:
                return left + right;
            case MINUS:
                return left - right;
            case STAR:
                return left * right;
            case SLASH:
                return left / right;
            default:
                throw new EvaluatorErrorException("Invalid binary operator.");
        }
    }

    @Override
    public String toString() {
        return String.format(
                "BinaryExpression[%s](%s %s)",
                operator.type.toString(), left.toString(), right.toString());
    }
}
