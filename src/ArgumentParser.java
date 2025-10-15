import java.util.HashMap;

public class ArgumentParser {

    private HashMap<String, String> argsWithValues = new HashMap<>();

    public ArgumentParser(String[] args) {
        for (int i = 0; i < args.length; i++) {
            String arg = args[i];
            if (arg.startsWith("-") && i < args.length-1 && !args[i+1].startsWith("-")) {
                // This flag has a value
                argsWithValues.put(arg, args[i + 1]);
            } else if (arg.startsWith("-")) {
                // This flag doesn't have a value, or it's a simple argument
                argsWithValues.put(arg, "");
            }
        }
    }

    /**
     * Returns the value associated with a flag, or null if the flag is not present
     */
    public String getValue(String arg) {
        return argsWithValues.get(arg);
    }

}
