% rainfall.m
%
% Use ERA5 mean daily rainfall to generate adjustments to the model beta parameter.
%
% Bounding of 0.4 to 1.0 is based on USAID Global Health Supply Chain program figures
% https://www.ghsupplychain.org/news/data-driven-redistribution-ensures-availability-malaria-supplies-rwanda
warning('off', 'MATLAB:MKDIR:DirectoryExists');

% Set the floor for the beta, this is based on the 2.5x difference in cases
minima = 0.4;

% Load the data, drop the Google Earth Engine column
raw = readmatrix('../data/uga_rainfall.csv');
raw = raw(:, 1:2);

% Shift the data to account for the Anopheles gambiae life cycle of about
% 10 - 11 days, skewing a bit longer for Uganda
daily = circshift(raw(:, 2), 11);

% Smooth the and normalize the data
daily = smoothdata(daily);
daily = (1 - minima) * (daily - min(daily)) / (max(daily) - min(daily)) + minima;

% Plot the ERA5 rainfall data
subplot(2, 1, 1);
plot(raw(:, 1) + 1, raw(:, 2));
format('Mean Rainfall, 10 Year Mean');

% Plot the beta adjustments
subplot(2, 1, 2);
plot(raw(:, 1) + 1, daily);
xlim([1 365]);
format('Daily Beta Adjustment');

% Write the data to disk
writematrix(daily, '../../GIS/uga_adjustment.csv');

function [] = format(label) 
    xlim([1 365]);
    xlabel('Day of Year');
    ylabel(label);
    graphic = gca;
    graphic.FontSize = 18;
end