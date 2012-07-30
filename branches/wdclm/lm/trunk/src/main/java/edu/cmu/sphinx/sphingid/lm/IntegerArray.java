package edu.cmu.sphinx.sphingid.lm;

import java.util.Arrays;
import java.util.Random;

public class IntegerArray {
	private int[] array;
	private int size;

	public IntegerArray(int size) {
		this.array = new int[size];
		this.size = size;
	}

	public int get(int position) {
		if (position < 0 || position >= size)
			throw new ArrayIndexOutOfBoundsException();
		else
			return this.array[position];
	}

	public void put(int position, int value) {
		if (position < 0 || position >= size)
			throw new ArrayIndexOutOfBoundsException();
		else
			this.array[position] = value;
	}

	public void sort() {
		Arrays.sort(this.array);
	}

	public void sort(int fromIndex, int toIndex) {
		Arrays.sort(this.array, fromIndex, toIndex);
	}

	/**
	 * Shuffles the array using Durstenfeld algorithm.
	 * 
	 */
	public void shuffle() {
		Random random = new Random();
		int rnd = 0;
		int temp;
		for (int i = size - 1; i >= 0; --i) {
			rnd = random.nextInt(i + 1);
			temp = this.array[i];
			this.array[i] = this.array[rnd];
			this.array[rnd] = temp;
		}
	}

	public int size() {
		return this.size;
	}

	public boolean equals(IntegerArray array) {
		if (this.size != array.size)
			return false;

		for (int i : this.array) {
			if (this.array[i] != array.get(i))
				return false;
		}

		return true;
	}
}
