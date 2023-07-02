package utils;

public class Colors {
    private Colors() {}

    /**
     * ANSI escape code for black.
     * 
     * @param s The string to color.
     * @return The colored string.
    */
    public static String black(String s) {
        return "\033[30m" + s + "\033[0m";
    }
    
    /**
     * ANSI escape code for red.
     * 
     * @param s The string to color.
     * @return The colored string.
    */
    public static String red(String s) {
        return "\033[31m" + s + "\033[0m";
    }
    
    /**
     * ANSI escape code for green.
     * 
     * @param s The string to color.
     * @return The colored string.
    */
    public static String green(String s) {
        return "\033[32m" + s + "\033[0m";
    }
    
    /**
     * ANSI escape code for yellow.
     * 
     * @param s The string to color.
     * @return The colored string.
    */
    public static String yellow(String s) {
        return "\033[33m" + s + "\033[0m";
    }
    
    /**
     * ANSI escape code for blue.
     * 
     * @param s The string to color.
     * @return The colored string.
    */
    public static String blue(String s) {
        return "\033[34m" + s + "\033[0m";
    }
    
    /**
     * ANSI escape code for magenta.
     * 
     * @param s The string to color.
     * @return The colored string.
    */
    public static String magenta(String s) {
        return "\033[35m" + s + "\033[0m";
    }
    
    /**
     * ANSI escape code for cyan.
     * 
     * @param s The string to color.
     * @return The colored string.
    */
    public static String cyan(String s) {
        return "\033[36m" + s + "\033[0m";
    }
    
    /**
     * ANSI escape code for white.
     * 
     * @param s The string to color.
     * @return The colored string.
    */
    public static String white(String s) {
        return "\033[37m" + s + "\033[0m";
    }
}
