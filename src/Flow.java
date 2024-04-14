import java.io.File;

import exception.FileNotFoundException;
import exception.InvalidExtensionException;
import exception.SyntaxErrorException;
import syntax.Lexer;
import syntax.Token;
import syntax.Token.TokenType;

public class Flow {
    public static void main(String[] args) {
        // Print the welcome message
        System.out.print("\033[H\033[2J");
        System.out.println("    ________    ____ _       __");
        System.out.println("   / ____/ /   / __ \\ |     / /");
        System.out.println("  / /_  / /   / / / / | /| / / ");
        System.out.println(" / __/ / /___/ /_/ /| |/ |/ /  ");
        System.out.println("/_/   /_____/\\____/ |__/|__/ 0.0");
        System.out.println();
        System.out.println();

        // Start compiling the code
        try {
            if (args.length == 0) {
                // Read the input from the console
                while (true) {
                    System.out.print("\033[32m>>>\033[0m ");

                    final String input = System.console().readLine();
                    final Lexer lexer = new Lexer(input);

                    for (Token token = lexer.scan(); token.type != TokenType.EOF; token = lexer.scan()) {
                        System.out.println(token);
                    }
                }

            } else {
                // Read the input from the file
                final Lexer lexer = new Lexer(new File(args[0]));

                for (Token token = lexer.scan(); token.type != TokenType.EOF; token = lexer.scan()) {
                    System.out.println(token);
                }
            }
        } catch (FileNotFoundException e) {
            System.err.println(e.getMessage());
        } catch (InvalidExtensionException e) {
            System.err.println(e.getMessage());
        } catch (SyntaxErrorException e) {
            System.err.println(e.getMessage());
        }
    }
}
