import java.util.List;

import syntax.Parser;
import syntax.expression.TreeExpression;
import utils.Colors;

public class jFlow {
    public static void main(String[] args) {
        System.out.println("    ________    ____ _       __");
        System.out.println("   / ____/ /   / __ \\ |     / /");
        System.out.println("  / /_  / /   / / / / | /| / / ");
        System.out.println(" / __/ / /___/ /_/ /| |/ |/ /  ");
        System.out.println("/_/   /_____/\\____/ |__/|__/   ");
        System.out.println();
        System.out.println();

        while (true) {
            String input = "";

            // Read input from the user
            System.out.print(Colors.green(">>>") + " ");
            input = System.console().readLine();

            // Parse the input
            final Parser parser = new Parser(input);
            final TreeExpression treeExpression = parser.parse();
            final List<String> diagnostics = treeExpression.getDiagnostics(); 
            
            if (diagnostics.isEmpty()) {
                // Print the result
                System.out.println(treeExpression.evaluate());
            } else {
                // Print diagnostics
                for (String diagnostic : diagnostics) {
                    System.out.println(diagnostic);
                }
            }
            System.out.println();
        }
    }
}
