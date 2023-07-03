import java.util.List;

import syntax.ExpressionTree;
import utils.Colors;

public class jFlow {
    public static void main(String[] args) {
        boolean showTree = false;

        // Print the welcome message
        System.out.print("\033[H\033[2J");
        System.out.println("    ________    ____ _       __");
        System.out.println("   / ____/ /   / __ \\ |     / /");
        System.out.println("  / /_  / /   / / / / | /| / / ");
        System.out.println(" / __/ / /___/ /_/ /| |/ |/ /  ");
        System.out.println("/_/   /_____/\\____/ |__/|__/   ");
        System.out.println();
        System.out.println();

        // Main loop
        while (true) {
            String input = "";

            // Read input from the user
            System.out.print(Colors.green(">>>") + " ");
            input = System.console().readLine();

            // Special commands
            if (input.equals("#exit")) {
                break;
            } else if (input.equals("#clear")) {
                System.out.print("\033[H\033[2J");
                continue;
            } else if (input.equals("#showTree")) {
                showTree = !showTree;
                continue;
            } else if (input.equals("#help")) {
                System.out.println("Commands:");
                System.out.println("    #exit     - Exit the program.");
                System.out.println("    #clear    - Clear the screen.");
                System.out.println("    #showTree - Toggle showing the expression tree.");
                System.out.println("    #help     - Print this help message.");
                continue;
            }

            // Create an expression tree
            final ExpressionTree expressionTree = ExpressionTree.parse(input);
            final List<String> diagnostics = expressionTree.getDiagnostics();

            if (diagnostics.isEmpty()) {
                // Print the expression tree
                if (showTree) {
                    System.out.println(expressionTree.toString());
                }

                // Print the result
                System.out.println(expressionTree.evaluate());
            } else {
                // Print diagnostics
                for (String diagnostic : diagnostics) {
                    System.out.println(diagnostic);
                }
            }
        }
    }
}
