clear; clc;

% Input data
radius_um = [0	0.0100000000000000	0.0300000000000000	0.0500000000000000	0.0700000000000000	0.0800000000000000	0.0900000000000000	0.100000000000000	0.110000000000000	0.120000000000000	0.130000000000000	0.140000000000000	0.150000000000000];
weighted_escape_prob = [0.190945259219596	0.189907377997801	0.197758952327481	0.221055700233453	0.262826044266326	0.293550303102207	0.332990029273258	0.383762959821573	0.451885612782562	0.544405637556324	0.675854291794639	0.859258835149716	1.17859245516615];

% Translation parameter
z_translate = 0.15;

% Transform x-axis by subtracting z_translate
radius_translated = radius_um - z_translate;

% Define the exponential function with constant: y = c + a*exp(b*(x-z))
exp_func = @(params, x) params(1) + params(2) * exp(params(3) * x);

% Initial parameter guesses
c0 = min(weighted_escape_prob) * 0.1; % small baseline constant
a0 = max(weighted_escape_prob) - c0; % exponential amplitude
b0 = 10; % exponential coefficient

initial_guess = [c0, a0, b0];

% Set optimization options
options = optimoptions('lsqcurvefit', 'Display', 'off');

% Perform nonlinear fitting
params = lsqcurvefit(exp_func, initial_guess, radius_translated, weighted_escape_prob, [], [], options);

% Extract fitted parameters
c = params(1);
a = params(2);
b = params(3);

% Calculate R-squared
y_pred = exp_func(params, radius_translated);
SS_res = sum((weighted_escape_prob - y_pred).^2);
SS_tot = sum((weighted_escape_prob - mean(weighted_escape_prob)).^2);
R_squared = 1 - SS_res/SS_tot;

% Display results
fprintf('Exponential Fit with Constant: y = c + a*exp(b*(x-%.1f))\n', z_translate);
fprintf('Parameters:\n');
fprintf('c = %.6e\n', c);
fprintf('a = %.6e\n', a);
fprintf('b = %.6f\n', b);
fprintf('R-squared = %.6f\n', R_squared);
fprintf('\nFitted Expression:\n');
fprintf('WeightedEscapeProbability = %.6e + %.6e * exp(%.6f * (Radius_um - %.1f))\n', c, a, b, z_translate);

% Plot the fit with extended range
figure;
plot(radius_um, weighted_escape_prob, 'ko', 'MarkerSize', 8, 'MarkerFaceColor', 'k');
hold on;

% Extend the plotting range beyond the data
x_fine = linspace(min(radius_um) - 0.1, max(radius_um) + 0.1, 200);
plot(x_fine, c + a * exp(b * (x_fine - z_translate)), 'r-', 'LineWidth', 2);

xlabel('Radius (μm)');
ylabel('Weighted Escape Yield');
title(sprintf('Exponential Fit: y = %.2e + %.2e × exp(%.2f × (x-%.1f)), R² = %.4f', c, a, b, z_translate, R_squared));
legend('Data', 'Fit', 'Location', 'northwest');
grid on;
box on;

% Set axis limits to show the extended curve
xlim([min(radius_um) - 0.05, max(radius_um) + 0.05]);
ylim([0, max(weighted_escape_prob) * 1.2]);

% Additional analysis: Show what happens at x=z_translate (translated x=0)
fprintf('\nAt Radius = %.1f μm (translated x=0):\n', z_translate);
fprintf('Predicted WeightedEscapeProbability = %.6e\n', c + a);