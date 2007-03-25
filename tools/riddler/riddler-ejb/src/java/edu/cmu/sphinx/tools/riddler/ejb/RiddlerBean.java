/*
 * RiddlerBean.java
 *
 * Created on February 10, 2007, 9:22 PM
 * <p/>
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 * <p/>
 * <p/>
 */

package edu.cmu.sphinx.tools.riddler.ejb;

import edu.cmu.sphinx.tools.riddler.types.Dictionary;
import edu.cmu.sphinx.tools.riddler.types.Pronunciation;

import javax.ejb.Remote;
import javax.ejb.Stateless;
import javax.jws.WebMethod;
import javax.jws.WebService;
import javax.persistence.*;
import java.net.URI;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.Date;
import java.util.Set;
import java.util.HashSet;

/**
 * Stateless session bean / web service implementation of Riddler
 * @author Garrett Weinberg
 */
@Stateless
@Remote(RiddlerRemote.class)
@WebService
public class RiddlerBean implements RiddlerRemote {

    @PersistenceContext
    private EntityManager em;
    
    public RiddlerBean() {
    }

    @WebMethod
    public URI trainModelsFromCorpus(String corpusID) {
        return null;
    }

    @WebMethod
    public URI trainModelsFromCorpora(ArrayList<String> corpusIDs) {
        return null;
    }

    @WebMethod
    public String createDictionary(MetadataWrapper metadata) throws RemoteException {
        try {
            final Query q = em.createNamedQuery("findDictionaryByMetadata");
            q.setParameter("metadata", metadata.getContents());
            q.getSingleResult();
            throw new RemoteException("a Dictionary having metadata '" + metadata + "' already exists");
        }
        catch (NoResultException nre) {
            // expected exception; there is not yet a dictionary with that metadata
            Dictionary d;
            d = new Dictionary();
            d.setMetadata(metadata.getContents());
            em.persist(d);
            d = em.merge(d);
            return d.getId();
        }
    }

    @WebMethod
    public String getDictionary(MetadataWrapper metadata) throws RemoteException {
        Dictionary d;
        try {
            final Query q = em.createNamedQuery("findDictionaryByMetadata");
            q.setParameter("metadata", metadata.getContents());
            d = (Dictionary) q.getSingleResult();
        }
        catch (NoResultException nre) {
            throw new RemoteException("no Dictionary having the provided metadata '" + metadata + "' exists");
        }
        catch (NonUniqueResultException nure) {
            throw new IllegalStateException("multiple Dictionaries have the same metadata '" + metadata);
        }
        return d.getId();
    }

    @WebMethod
    public MetadataWrapper getDictionaryMetadata(String dictionaryID) throws RemoteException {
        Dictionary d = fetchDictionary(dictionaryID);
        return new MetadataWrapper(d.getMetadata());
    }

    @WebMethod
    public String addPronuncations(String dictionaryID, String word, ArrayList<String> pronunciations) throws RemoteException {
        Dictionary d = fetchDictionary(dictionaryID);
        Pronunciation p;
        try {
            final Query q = em.createNamedQuery("findPronunciationByWord");
            q.setParameter("word", word);
            q.setParameter("dictionary", d);
            p = (Pronunciation) q.getSingleResult();

            // Pronounciation found: add all the words in the input parameter
            Set<String> variants = p.getVariants();
            variants.addAll(pronunciations);
            p.setVariants(variants);
            p = em.merge(p);
        }
        catch (NonUniqueResultException nure) {
            throw new IllegalStateException("multiple Pronunciations for the word '" + word + "' found in Dictionary " + d);
        }
        catch (NoResultException nre) {
            // no Pronunciation found: create a new record
            p = new Pronunciation(word, new HashSet<String>(pronunciations));
            em.persist(p);
            p = em.merge(p);
        }
        return p.getId();
    }

    @WebMethod
    public boolean hasPronuncation(String dictionaryID, String word) throws RemoteException {
        Dictionary d = fetchDictionary(dictionaryID);
        try {
            final Query q = em.createNamedQuery("findPronunciationByWord");
            q.setParameter("word", word);
            q.setParameter("dictionary", d);
            q.getSingleResult();
            return true;
        }
        catch (NonUniqueResultException nure) {
            throw new IllegalStateException("multiple Pronunciations for the word '" + word + "' found in Dictionary " + d);
        }
        catch (NoResultException nre) {
            return false;
        }
    }

    @WebMethod
    public String createCorpus(String dictionaryID, MetadataWrapper metadata, Date collectDate) {
        return null;
    }

    @WebMethod
    public String createItem(String corpusId) {
        return null;
    }

    @WebMethod
    public String createItemWithShortAudio(String corpusId, int samplesPerSecond, int channelCount, short[] data) {
        return null;
    }

    @WebMethod
    public String createItemWithByteAudio(String corpusId, int samplesPerSecond, int channelCount, byte[] data) {
        return null;
    }

    @WebMethod
    public String createItemWithIntAudio(String corpusId, int samplesPerSecond, int channelCount, int[] data) {
        return null;
    }

    @WebMethod
    public String createItemWithLongAudio(String corpusId, int samplesPerSecond, int channelCount, long[] data) {
        return null;
    }

    @WebMethod
    public String createItemWithFloatAudio(String corpusId, int samplesPerSecond, int channelCount, float[] data) {
        return null;
    }

    @WebMethod
    public String createItemWithText(String corpusId, ArrayList<String> words) {
        return null;
    }

    @WebMethod
    public String createItemWithShortAudioAndText(String corpusId, int samplesPerSecond, int channelCount, short[] data, ArrayList<String> words) {
        return null;
    }

    @WebMethod
    public String createItemWithByteAudioAndText(String corpusId, int samplesPerSecond, int channelCount, byte[] data, ArrayList<String> words) {
        return null;
    }

    @WebMethod
    public String createItemWithIntAudioAndText(String corpusId, int samplesPerSecond, int channelCount, int[] data, ArrayList<String> words) {
        return null;
    }

    @WebMethod
    public String createItemWithLongAudioAndText(String corpusId, int samplesPerSecond, int channelCount, long[] data, ArrayList<String> words) {
        return null;
    }

    @WebMethod
    public String createItemWithFloatAudioAndText(String corpusId, int samplesPerSecond, int channelCount, float[] data, ArrayList<String> words) {
        return null;
    }

    @WebMethod
    public String createTextRegion(String itemID, int startIndex, int endIndex) {
        return null;
    }

    @WebMethod
    public String createAudioRegion(String itemID, int beginTime, int endTime) {
        return null;
    }

    @WebMethod
    public String createAudioRegionWithText(String itemID, int beginTime, int endTime, int startIndex, int endIndex) {
        return null;
    }

    @WebMethod
    public void associateAudioRegionWithText(String audioID, String textID) {

    }


    private Dictionary fetchDictionary(String dictionaryID) throws RemoteException {
        Dictionary d = em.find(Dictionary.class, dictionaryID);
        if (d == null)
            throw new RemoteException("no Dictionary with ID " + dictionaryID + " exists");
        else
            return d;
    }
}