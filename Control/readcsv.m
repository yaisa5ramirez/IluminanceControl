%% K CONSTANTE
% Define the filename
filenamek3 = 'K3.csv';
filenamek5 = 'K5.csv';
filenamek12 = 'K12.csv';

% Read the data from the CSV file
dataK3 = csvread(filenamek3, 1, 0); % Skip the first row which contains axis names
dataK5 = csvread(filenamek5, 1, 0); % Skip the first row which contains axis names
dataK12 = csvread(filenamek12, 1, 0); % Skip the first row which contains axis names

% Separate x and y data
x3 = dataK3(:, 2);
y3 = dataK3(:, 3);

% Separate x and y data
x5 = dataK5(:, 2);
y5 = dataK5(:, 3);

% Separate x and y data
x12 = dataK12(:, 2);
y12 = dataK12(:, 3);

figure
subplot(3, 1,1)
% Plot the data
plot(x3, 'b'); % 'b.-' specifies blue color, solid line, and markers
hold on 
plot(y3, 'r'); % 'b.-' specifies blue color, solid line, and markers
xlabel('X Axis Label');
ylabel('Y Axis Label');
title('K = 3 ');
grid on; % Show grid on the plot


subplot(3,1, 2)
% Plot the data
plot(x5, 'b'); % 'b.-' specifies blue color, solid line, and markers
hold on 
plot(y5, 'r'); % 'b.-' specifies blue color, solid line, and markers
xlabel('X Axis Label');
ylabel('Y Axis Label');
title('K = 5');
grid on; % Show grid on the plot

subplot(3,1,3)
% Plot the data
plot(x12, 'b'); % 'b.-' specifies blue color, solid line, and markers
hold on 
plot(y12, 'r'); % 'b.-' specifies blue color, solid line, and markers
xlabel('X Axis Label');
ylabel('Y Axis Label');
title('K =12 ');
grid on; % Show grid on the plot



%% Ti Variable
% Define the filename
filenamek3 = 'Ti1.csv'; %0.01
filenamek5 = 'Ti5.csv';%0.05
filenamek12 = 'Ti2.csv';%0.2

% Read the data from the CSV file
dataK3 = csvread(filenamek3, 1, 0); % Skip the first row which contains axis names
dataK5 = csvread(filenamek5, 1, 0); % Skip the first row which contains axis names
dataK12 = csvread(filenamek12, 1, 0); % Skip the first row which contains axis names

% Separate x and y data
x3 = dataK3(:, 2);
y3 = dataK3(:, 3);

% Separate x and y data
x5 = dataK5(:, 2);
y5 = dataK5(:, 3);

% Separate x and y data
x12 = dataK12(:, 2);
y12 = dataK12(:, 3);

figure
subplot(3, 1,1)
% Plot the data
plot(x3, 'b'); % 'b.-' specifies blue color, solid line, and markers
hold on 
plot(y3, 'r'); % 'b.-' specifies blue color, solid line, and markers
xlabel('X Axis Label');
ylabel('Y Axis Label');
title('Ti = 0.01 ');
grid on; % Show grid on the plot


subplot(3,1, 2)
% Plot the data
plot(x5, 'b'); % 'b.-' specifies blue color, solid line, and markers
hold on 
plot(y5, 'r'); % 'b.-' specifies blue color, solid line, and markers
xlabel('X Axis Label');
ylabel('Y Axis Label');
title('Ti = 0.05');
grid on; % Show grid on the plot

subplot(3,1,3)
% Plot the data
plot(x12, 'b'); % 'b.-' specifies blue color, solid line, and markers
hold on 
plot(y12, 'r'); % 'b.-' specifies blue color, solid line, and markers
xlabel('X Axis Label');
ylabel('Y Axis Label');
title('Ti = 0.2 ');
grid on; % Show grid on the plot

%% B Variable
% Define the filename
filenamek3 = 'B005.csv'; %0.05
filenamek5 = 'B09.csv';%0.9
filenamek12 = 'B3.csv';%0.2

% Read the data from the CSV file
dataK3 = csvread(filenamek3, 1, 0); % Skip the first row which contains axis names
dataK5 = csvread(filenamek5, 1, 0); % Skip the first row which contains axis names
dataK12 = csvread(filenamek12, 1, 0); % Skip the first row which contains axis names

% Separate x and y data
x3 = dataK3(:, 2);
y3 = dataK3(:, 3);

% Separate x and y data
x5 = dataK5(:, 2);
y5 = dataK5(:, 3);

% Separate x and y data
x12 = dataK12(:, 2);
y12 = dataK12(:, 3);

figure
subplot(3, 1,1)
% Plot the data
plot(x3, 'b'); % 'b.-' specifies blue color, solid line, and markers
hold on 
plot(y3, 'r'); % 'b.-' specifies blue color, solid line, and markers
xlabel('X Axis Label');
ylabel('Y Axis Label');
title('B = 0.05 ');
grid on; % Show grid on the plot


subplot(3,1, 2)
% Plot the data
plot(x5, 'b'); % 'b.-' specifies blue color, solid line, and markers
hold on 
plot(y5, 'r'); % 'b.-' specifies blue color, solid line, and markers
xlabel('X Axis Label');
ylabel('Y Axis Label');
title(' B = 0.9');
grid on; % Show grid on the plot

subplot(3,1,3)
% Plot the data
plot(x12, 'b'); % 'b.-' specifies blue color, solid line, and markers
hold on 
plot(y12, 'r'); % 'b.-' specifies blue color, solid line, and markers
xlabel('X Axis Label');
ylabel('Y Axis Label');
title('B = 3 ');
grid on; % Show grid on the plot

%% Tt Variable
% Define the filename
filenamek3 = 'Tt001Luz.csv'; %0.01
filenamek5 = 'Tt01Luz.csv';%0.1
filenamek12 = 'Tt1Luz.csv';%1

% Read the data from the CSV file
dataK3 = csvread(filenamek3, 1, 0); % Skip the first row which contains axis names
dataK5 = csvread(filenamek5, 1, 0); % Skip the first row which contains axis names
dataK12 = csvread(filenamek12, 1, 0); % Skip the first row which contains axis names

% Separate x and y data
x3 = dataK3(:, 2);
y3 = dataK3(:, 3);

% Separate x and y data
x5 = dataK5(:, 2);
y5 = dataK5(:, 3);

% Separate x and y data
x12 = dataK12(:, 2);
y12 = dataK12(:, 3);

figure
subplot(3, 1,1)
% Plot the data
plot(x3, 'b'); % 'b.-' specifies blue color, solid line, and markers
hold on 
plot(y3, 'r'); % 'b.-' specifies blue color, solid line, and markers
xlabel('X Axis Label');
ylabel('Y Axis Label');
title('Tt = 0.01 ');
grid on; % Show grid on the plot


subplot(3,1, 2)
% Plot the data
plot(x5, 'b'); % 'b.-' specifies blue color, solid line, and markers
hold on 
plot(y5, 'r'); % 'b.-' specifies blue color, solid line, and markers
xlabel('X Axis Label');
ylabel('Y Axis Label');
title(' Tt = 0.1');
grid on; % Show grid on the plot

subplot(3,1,3)
% Plot the data
plot(x12, 'b'); % 'b.-' specifies blue color, solid line, and markers
hold on 
plot(y12, 'r'); % 'b.-' specifies blue color, solid line, and markers
xlabel('X Axis Label');
ylabel('Y Axis Label');
title('Tt = 1 ');
grid on; % Show grid on the plot
