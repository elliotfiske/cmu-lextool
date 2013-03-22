package edu.cmu.sphinx.speaker_diarization;

import java.util.TreeSet;
import java.util.ArrayList;
import java.util.Iterator;

public class Cluster {
	private TreeSet<Segment> segmentSet;
	protected String speakerGender;
	
	public Cluster(){
		this.segmentSet = new TreeSet<Segment>();
		this.speakerGender = new String();
	}
	
	public Cluster(Segment s){
		this.segmentSet = new TreeSet<Segment>();
		this.speakerGender = new String();
		addSegment(s);
	}
	
	public Cluster(Cluster c){
		this.segmentSet = new TreeSet<Segment>();
		this.speakerGender = c.speakerGender;
		Iterator<Segment> it = c.segmentSet.iterator();
		while(it.hasNext())
			this.addSegment(it.next());
	}
	
	public TreeSet<Segment> getSegments(){
		return this.segmentSet;
	}
	
	public ArrayList<Segment> getArrayOfSegments(){
		Iterator<Segment> it = segmentSet.iterator();
		ArrayList<Segment> ret = new ArrayList<Segment>();
		while(it.hasNext())
			ret.add(it.next());
		return ret;
	}
	
	public Boolean addSegment(Segment s){
		return this.segmentSet.add(s);
	}
	
	public Boolean removeSegment(Segment s){
		return this.segmentSet.remove(s);
	}
	
	/*
	 * returns a 2 * n length array where n is the numbers of intervals
	 * assigned to the speaker modeled by this cluster
	 * every pair of elements with indexes (2 * i, 2 * i + 1)
	 * represents the start time and the length for each interval
	 */
	/*
	 * We may need a delay parameter to this function
	 * because the segments may not be exactly consecutive
	 */
	public ArrayList<Integer> getSpeakerIntervals(){
		ArrayList<Integer> ret = new ArrayList<Integer>();
		Iterator<Segment> it = segmentSet.iterator();
		Segment curent, previous = it.next();
		int start = previous.getStartTime(), length = previous.getLength(), idx = 0;
		ret.add(start); ret.add(length);
		while(it.hasNext()){
			curent = it.next();
			start = ret.get(2 * idx);
			length = ret.get(2 * idx + 1);
			if((start + length) == curent.getStartTime())
				ret.set(2 * idx + 1, length + curent.getLength());
			else{
				idx++;
				ret.add(curent.getStartTime());
				ret.add(curent.getLength());
			}
			previous = curent;
		}
		return ret;
	}
	
	public void mergeWith(Cluster target) throws NullPointerException{
		if(target == null)
			throw new NullPointerException();
		Iterator<Segment> it = target.segmentSet.iterator();
		while(it.hasNext())
			if(!this.addSegment(it.next()))
				System.out.println("Something doesn't work in mergeWith method, Cluster class");
	}
}
