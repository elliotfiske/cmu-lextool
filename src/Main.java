import java.util.*;
import org.apache.commons.lang.WordUtils;

public class Main {
	
	/**
	 * Generate a map of words with capitalized, lowercased and uppercased forms
	 * @param s
	 * @return a map
	 */
	public static HashMap<String, LinkedList<String>> generateMap(String s) {
		HashMap<String, LinkedList<String>> map = new HashMap<String, LinkedList<String>>();
		for (String word : s.split(" ")) {
			word = word.toLowerCase();
			if (!map.containsKey(word)) {
				map.put(word, new LinkedList<String>());
				map.get(word).add(word);
				map.get(word).add(WordUtils.capitalize(word));
				map.get(word).add(word.toUpperCase());
			}
		}
		return map;
	}
	
	
	public static void main(String[] args) {
		
		Hyperstring h = new Hyperstring("This is a sentence", true);
		System.out.println(h.toString());
	}
}
