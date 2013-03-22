package edu.cmu.sphinx.speaker_diarization;

public class Segment implements Comparable<Segment>{
	public final static int FEATURES_COUNT = 39;
	
	private int startTime, length;
	float[] features = new float[FEATURES_COUNT];
	
	public Segment (Segment ref){
		this.startTime = ref.startTime;
		this.length = ref.length;
		for(int i = 0; i < 39; i++)
			this.features[i] = ref.features[i];
	}
	
	public Segment(int startTime, int length){
		this.startTime = startTime;
		this.length = length;
	}
	
	public Segment (int startTime, int length, float[] features){
		this.startTime = startTime;
		this.length = length;
		this.features = features;
	}
	
	public Segment (){
		this.startTime = this.length = 0;
	}
	
	/*setters */
	public void setStartTime(int startTime){
		this.startTime = startTime;
	}
	
	public void setLength(int length){
		this.length = length;
	}
	
	public void setFeatures(float[] features){
		this.features = features;
	}
	
	/* getters */
	public int getStartTime(){
		return this.startTime;
	}
	
	public int getLength(){
		return this.length;
	}
	
	public float[] getFeatures(){
		return this.features;
	}
	
	public int equals(Segment ref){
		return (this.startTime == ref.startTime) ? 1 : 0;
	}
	
	@Override
	public String toString(){
		return this.startTime + " " + this.length + "\n";
	}
	public int compareTo(Segment ref){
		return (this.startTime - ref.startTime);
	}
}
