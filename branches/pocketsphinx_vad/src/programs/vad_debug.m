% vad_debug
%   visualizes ps vad decisions from pocketsphinx_vad_debug
%
% Input arguments with typical values:   
%   wav_file = 'speech.wav'  % Your speech file in WAV format
%   vad_dir = './';    % Directory with vad_values files
%   frate = 100;       % amount of frames per second
%   threshold = 75;    % vad threshold

% Output:
%   Plots audio data and aligns it to vad decisions  

function vad_debug(wav_file, vad_dir, frate, threshold)
[audio,Fs] = wavread(wav_file);

figure;
subplot(2,1,1);
plot(0:1/Fs:(length(audio)-1)/Fs, audio);
grid on;
xlabel('time, sec');
axis([0 (length(audio)-1)/Fs -1 1]);
subplot(2,1,2);
hold on;
colors = ['r' 'b' 'y' 'g'];
colorIdx = 1;
areaShift = 0;
for i=1:length(dir(vad_dir))
	if length(strfind(dir(vad_dir)(i).name, 'vad_values')) > 0
		if strfind(dir(vad_dir)(i).name, 'vad_values')(1) == 1 %startWith
			vad = load(strcat(vad_dir, '/', dir(vad_dir)(i).name));
			vad_aligned = zeros(length(vad)*(Fs/frate),1);
			for i=0:(length(vad)-1)
				for j=1:Fs/frate
					vad_aligned(i*(Fs/frate) + j) = vad(i+1);
				end;
			end;
			area((areaShift + 0):1/Fs:(areaShift + (length(vad_aligned)-1)/Fs), vad_aligned, "FaceColor", colors(colorIdx));
			areaShift = areaShift + (length(vad_aligned)-1)/Fs;
			colorIdx = colorIdx + 1;
			if colorIdx > length(colors)
				colorIdx = 1;
			end;
		end;
	end;
end;

line('XData', [0 (length(audio)-1)/Fs], 'YData', [threshold threshold], 'LineStyle', '-.', 'Color', 'black');
grid on;
xlabel('time, sec');
axis([0 (length(audio)-1)/Fs -1 (threshold*3)]);
