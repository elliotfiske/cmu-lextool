package edu.cmu.pocketsphinx.demo;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.ToggleButton;
import edu.cmu.pocketsphinx.Hypothesis;

public class WeatherForecastFragment extends ShowcaseFragment {

    private TextView resultText;
    private String resultPrefix;

    private ToggleButton toggleButton;

    @Override
    public View onCreateView(LayoutInflater inflater,
                             ViewGroup container,
                             Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.weather_forecast, container, false);
        toggleButton = (ToggleButton) v.findViewById(R.id.start_button);
        resultText = (TextView) v.findViewById(R.id.result_text);

        return v;
    }

    @Override
    public void onStart() {
        super.onStart();
        resultPrefix = "";
        toggleButton.setChecked(false);
        toggleButton.setOnCheckedChangeListener(this);
    }

    @Override
    public void onPartialResult(Hypothesis hypothesis) {
        super.onPartialResult(hypothesis);
        Log.i("WeatherForecastFragment", ">>> partial result from bank: " + hypothesis.getHypstr());
        if (hypothesis.getHypstr().equals(PocketSphinxActivity.KEYPHRASE))
            return;
        resultText.setText(resultPrefix + " " + hypothesis.getHypstr());
    }

    @Override
    public void onResult(Hypothesis hypothesis) {
        Log.i("WeatherForecastFragment", ">>> final result from bank: " + hypothesis.getHypstr());
        if (hypothesis.getHypstr().equals(PocketSphinxActivity.KEYPHRASE))
            return;
        resultPrefix += " " + hypothesis.getHypstr();
        resultText.setText(resultPrefix);
        //if recognition was stopped explicitly, reset vad state
    }
    
    @Override
    protected void setButtonPressed() {
        toggleButton.setChecked(true);
    }

    @Override
    public void onVadStateChanged(boolean state) {
	if (!state && toggleButton.isChecked()) {
	    //speech -> silence transition, 
	    //end old utterance and start new one
	    recognizer.stopListening();
	    recognizer.startListening();
	}
    }
}
