N   = 62;        % FIR filter order
Fp  = 2.5e3;       % 20 kHz passband-edge frequency
Fs  = 48e3;       % 96 kHz sampling frequency
Rp  = 0.00057565; % Corresponds to 0.01 dB peak-to-peak ripple
Rst = 1e-4;       % Corresponds to 80 dB stopband attenuation

scope  = dsp.SpectrumAnalyzer('SampleRate',Fs,'SpectralAverages',5);

lowpassFilt = dsp.LowpassFilter('DesignForMinimumOrder',false, ...
    'FilterOrder',N,'PassbandFrequency',Fp,'SampleRate',Fs,...
    'PassbandRipple',0.01, 'StopbandAttenuation',80);

eqnum = tf(lowpassFilt);
fvtool(eqnum,'Fs',Fs,'Color','White') % Visualize filter
display(eqnum);

tic
while toc < 10
    x = randn(256,1);
    y = lowpassFilt(x);
    scope(y);
end

for index = 1:63
    fprintf('%f,  ',eqnum(1,index));
end
