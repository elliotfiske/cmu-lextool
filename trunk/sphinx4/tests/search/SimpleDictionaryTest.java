/**
 * [[[copyright]]]
 */
package tests.search;

import edu.cmu.sphinx.util.SphinxProperties;

import edu.cmu.sphinx.search.Dictionary;
import edu.cmu.sphinx.search.SimpleDictionary;

import java.io.File;

import java.net.URL;


/**
 * A test for SimpleDictionary class that reads in the connected-digits
 * dictionaries and dumps out the dictionary.
 */
public class SimpleDictionaryTest {


    private String context = "SimpleDictionaryTest";


    /**
     * Construct a SimpleDictionaryTest with the given SphinxProperties file.
     *
     * @param propertiesFile a SphinxProperties file
     */
    public SimpleDictionaryTest(String propertiesFile) throws Exception {
        
        String pwd = System.getProperty("user.dir");
        SphinxProperties.initContext
            (context, new URL
             ("file://" + pwd + File.separatorChar + propertiesFile));
        
        Dictionary dictionary = new SimpleDictionary(context);
        dictionary.dump();
    }


    /**
     * Main method of this Test.
     */
    public static void main(String[] argv) {
        try {
            SimpleDictionaryTest test = new SimpleDictionaryTest(argv[0]);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
