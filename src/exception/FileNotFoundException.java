package exception;

/**
 * FileNotFoundException
 */
public class FileNotFoundException extends Exception {
    /**
     * The serial version UID.
     */
    private static final long serialVersionUID = 1L;

    /**
     * Creates a new file not found exception.
     *
     * @param file The file name.
     */
    public FileNotFoundException(String file) {
        super(String.format("\033[31mFileNotFound\033[0m: file not found <%s>", file));
    }
}