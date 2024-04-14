package exception;

/**
 * InvalidExtensionException
 */
public class InvalidExtensionException extends Exception {
    /**
     * The serial version UID.
     */
    private static final long serialVersionUID = 1L;

    /**
     * Creates a new invalid extension exception.
     *
     * @param file The file name.
     */
    public InvalidExtensionException(String file) {
        super(String.format("\033[31mInvalidExtension\033[0m: invalid extension <%s>, expected .flow", file));
    }
}