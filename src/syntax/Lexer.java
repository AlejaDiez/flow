package syntax;

import java.io.File;
import java.util.Scanner;

import exception.FileNotFoundException;
import exception.InvalidExtensionException;
import exception.SyntaxErrorException;
import syntax.Token.TokenType;

/**
 * Represents a lexer for the input code.
 */
public class Lexer {
    /**
     * The input file name.
     */
    private final String _inputName;
    /**
     * The input string to be tokenized.
     */
    private final String _input;
    /**
     * The current character.
     */
    private int _current = 0;
    /**
     * The current line number.
     */
    private int _line = 1;
    /**
     * The current column number.
     */
    private int _column = 0;

    /**
     * Creates a new lexer.
     *
     * @param command The input string to be tokenized.
     */
    public Lexer(String command) {
        this._inputName = "stdin";
        this._input = command;
    }

    /**
     * Creates a new lexer.
     *
     * @param file The input file to be tokenized.
     * @throws FileNotFoundException If the file is not found.
     */
    public Lexer(File file) throws FileNotFoundException, InvalidExtensionException {
        // Check if the has the correct extension
        this._inputName = file.getName();
        if (!this._inputName.endsWith(".flow")) {
            throw new InvalidExtensionException(this._inputName);
        }

        // Check if the file exists and read it
        try {
            // Read the file and store it in the input string
            final Scanner reader = new Scanner(file);
            String input = "";

            // Read the file line by line
            while (reader.hasNextLine()) {
                input += reader.nextLine() + "\n";
            }
            // Close the reader
            reader.close();
            // Set the input string
            this._input = input;
        } catch (Exception e) {
            throw new FileNotFoundException(this._inputName);
        }
    }

    /**
     * Returns the character at the current position plus an offset.
     * 
     * @param offset The offset to be added to the current position.
     * @return The character at the current position plus an offset.
     */
    private char _peek(int offset) {
        if (this._current + offset >= this._input.length()) {
            return '\0';
        }
        if (this._current + offset < 0) {
            return this._input.charAt(0);
        }
        return this._input.charAt(this._current + offset);
    }

    /**
     * Returns the next character in the input string.
     * 
     * @return The next character in the input string.
     */
    private char _advance() {
        final char c = this._peek(0);

        if (c == '\0') {
            return c;
        }
        // Move to the next character
        if (c == '\n') {
            this._line++;
            this._column = -1;
        }
        this._column++;
        this._current++;
        // Skip empty characters
        if (' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c) {
            return this._advance();
        }
        return c;
    }

    /**
     * Scans and returns a number token found in the input.
     * 
     * @return The number token found in the input.
     */
    private Token _scanNumber() {
        final int startColumn = this._column;
        String number = Character.toString(this._peek(-1));

        while (Character.isDigit(this._peek(0))) {
            number += Character.toString(this._advance());
        }
        return new Token(TokenType.INTEGER, Integer.parseUnsignedInt(number), this._line, startColumn);
    }

    /**
     * Scans and returns the next token found in the input.
     * 
     * @return The next token found in the input.
     * @throws SyntaxErrorException If an unrecognized character is found.
     */
    public Token scan() throws SyntaxErrorException {
        final char c = this._advance();

        switch (c) {
            case '\0':
                return new Token(TokenType.EOF, null, this._line, this._column);
            case '+':
                return new Token(TokenType.PLUS, null, this._line, this._column);
            case '-':
                return new Token(TokenType.MINUS, null, this._line, this._column);
            case '*':
                return new Token(TokenType.STAR, null, this._line, this._column);
            case '/':
                return new Token(TokenType.SLASH, null, this._line, this._column);
            default:
                if (Character.isDigit(c)) {
                    return _scanNumber();
                }
                throw new SyntaxErrorException(this._inputName, this._input, this._line, this._column);
        }
    }
}
