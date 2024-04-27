package exception;

/**
 * EvaluatorErrorException
 */
public class EvaluatorErrorException extends Exception {
    /**
     * The serial version UID.
     */
    private static final long serialVersionUID = 1L;

    /**
     * Creates a new evaluator error exception.
     *
     * @param message The error message.
     */
    public EvaluatorErrorException(String message) {
        super(String.format("\033[31mEvaluatorError\033[0m: %s", message));
    }
}