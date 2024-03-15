%Individuales
% Define the filename
filename1 = 'SaturatedOUT.log';

% Read the data from the CSV file
dataK3 = csvread(filename1, 1, 0); % Skip the first row which contains axis names

% Separate x and y data
x3 = dataK3(:, 2);
y3 = dataK3(:, 3);

figure
% Plot the data
plot(x3, 'b'); % 'b.-' specifies blue color, solid line, and markers
hold on 
plot(y3, 'r'); % 'b.-' specifies blue color, solid line, and markers
xlabel('X Axis Label');
ylabel('Y Axis Label');
title('AntiWind OFF Ref 15 ');
grid on; % Show grid on the plot

