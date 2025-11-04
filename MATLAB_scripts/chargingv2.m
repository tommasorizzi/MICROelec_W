% Self-Charging Rate Analysis Using Exponential Models
% Integrates continuous exponential functions for multiple particle sizes

clear; clc;

%% Constants
lambda = 1.78e-9;        % Decay constant (s^-1)
e = 1.602e-19;          % Elementary charge (C)
kappa = 1.7e-3;         % Tritium ratio (NT/NW)
rho = 19.25;            % Density (g/cm^3)
NA = 6.022e23;          % Avogadro's number (mol^-1)
MW = 183.24;            % Tungsten atomic mass (g/mol)

% Calculate constant factor
constant_factor = lambda * e * kappa * (rho * NA / MW);

fprintf('=== SELF-CHARGING ANALYSIS USING EXPONENTIAL MODELS ===\n');
fprintf('Constant factor: %.6e C⋅cm⁻²⋅s⁻¹\n\n', constant_factor);

%% Define Particle Sizes and Exponential Model Parameters
% Format: [radius_um, a, b, c] where y = a + b*exp(c*(x-R))
% You'll need to fill in the actual parameters for each size

particle_data = [
    % Radius(μm)    a        b       c       (Fill in your actual values)
    0.1,           0.364,   0.840,  39.96;   % Example from your table
    0.15,          0.189,   0.971,  33.30;
    0.2,           0.0996,  1.03,   30.37;   % Example from your table  
    0.5,           0.0231,  1.07,   29.90;   % Example from your table
    1.0,           0.0197,  1.07,   31.00;   % Example from your table
    5.0,           0.0180,  1.06,   31.81;   % Example from your table
    10.0,          0.0179,  1.06,   31.93;   % Example from your table
    20.0,          0.0177,  1.06,   31.99;   % From your figure
];

%% Analysis for Each Particle Size
results = struct();
num_particles = size(particle_data, 1);

for i = 1:num_particles
    radius_um = particle_data(i, 1);
    a = particle_data(i, 2);
    b = particle_data(i, 3);
    c = particle_data(i, 4);
    
    fprintf('--- Particle %d: Radius = %.1f μm ---\n', i, radius_um);
    fprintf('Exponential model: y = %.4f + %.3f × exp(%.2f × (x-%.1f))\n', a, b, c, radius_um);
    
    %% Define Integration Limits
    radius_cm = radius_um * 1e-4;  % Convert to cm
    
    % Integration limits based on particle size and simulation coverage
    if radius_um <= 0.4
        % Small particles: integrate entire particle (simulated completely)
        r_min = 0;                 % Center of particle
        r_max = radius_cm;         % Surface
        fprintf('Small particle: integrating from center to surface\n');
    else
        % Large particles: integrate only outer 0.4 μm shell (simulation limit)
        r_min = (radius_um - 0.4) * 1e-4;  % 0.4 μm from surface
        r_max = radius_cm;                  % Surface
        fprintf('Large particle: integrating outer 0.4 μm shell\n');
    end
    
    fprintf('Integration limits: %.3f to %.3f μm\n', r_min*1e4, r_max*1e4);
    fprintf('Integration depth: %.3f μm\n', (r_max-r_min)*1e4);
    
    %% Define Exponential Model Function
    % μ(r) = a + b*exp(c*(r-R)) where r is in cm, R is radius in cm
    mu_model = @(r) a + b * exp(c * (r - radius_cm));
    
    %% Define Integrand V(r)*μ(r)
    integrand = @(r) 4*pi*r.^2 .* mu_model(r);
    
    %% Perform Integration
    try
        integral_result = integral(integrand, r_min, r_max, 'RelTol', 1e-8);
        charging_rate = constant_factor * integral_result;
        
        fprintf('Integral ∫V(r)μ(r)dr: %.6e cm²\n', integral_result);
        fprintf('Self-charging rate: %.6e C/s\n', charging_rate);
        fprintf('Self-charging rate: %.6e electrons/s\n', charging_rate/e);
        fprintf('Self-charging rate: %.6e μA\n', charging_rate * 1e6);
        
        % Store results
        results(i).radius_um = radius_um;
        results(i).model_params = [a, b, c];
        results(i).integral = integral_result;
        results(i).charging_rate_C_per_s = charging_rate;
        results(i).charging_rate_electrons_per_s = charging_rate/e;
        results(i).charging_rate_microA = charging_rate * 1e6;
        
    catch ME
        fprintf('ERROR: Integration failed - %s\n', ME.message);
        
        % Store error results
        results(i).radius_um = radius_um;
        results(i).model_params = [a, b, c];
        results(i).integral = NaN;
        results(i).charging_rate_C_per_s = NaN;
        results(i).charging_rate_electrons_per_s = NaN;
        results(i).charging_rate_microA = NaN;
    end
    
    fprintf('\n');
end

%% Summary Results Table
fprintf('=== SUMMARY RESULTS TABLE ===\n');
fprintf('Radius   Model Parameters        Integral     Charging Rate    Charging Rate    Charging Rate\n');
fprintf('(μm)     [a, b, c]              (cm²)        (C/s)            (electrons/s)    (μA)\n');
fprintf('-------------------------------------------------------------------------------------------\n');

for i = 1:num_particles
    fprintf('%4.1f   [%6.4f,%5.3f,%5.2f]   %.3e   %.3e   %.3e   %.3e\n', ...
        results(i).radius_um, results(i).model_params(1), results(i).model_params(2), ...
        results(i).model_params(3), results(i).integral, results(i).charging_rate_C_per_s, ...
        results(i).charging_rate_electrons_per_s, results(i).charging_rate_microA);
end

%% Analysis and Visualization
% Extract valid results (no NaN values)
valid_idx = ~isnan([results.charging_rate_C_per_s]);
valid_results = results(valid_idx);

if sum(valid_idx) > 1
    
    radii = [valid_results.radius_um];
    charging_rates_C = [valid_results.charging_rate_C_per_s];
    charging_rates_electrons = [valid_results.charging_rate_electrons_per_s];
    
    %figure('Position', [100, 100, 1500, 1000]);
    
    %% Plot 1: Exponential Models
    % %subplot(2,3,1);
    % colors = lines(length(valid_results));
    % for i = 1:length(valid_results)
    %     radius_um = valid_results(i).radius_um;
    %     a = valid_results(i).model_params(1);
    %     b = valid_results(i).model_params(2);
    %     c = valid_results(i).model_params(3);
    % 
    %     % Plot model over reasonable range
    %     r_plot = linspace(0.9*radius_um, radius_um, 1000);
    %     mu_plot = a + b * exp(c * (r_plot*1e-4 - radius_um*1e-4));
    % 
    %     plot(r_plot, mu_plot, 'Color', colors(i,:), 'LineWidth', 2, ...
    %         'DisplayName', sprintf('R=%.1f μm', radius_um));
    %     hold on;
    % end
    % xlabel('Radius (μm)');
    % ylabel('Escape Yield');
    % title('Exponential Models');
    % legend('show', 'Location', 'best');
    % grid on;
    
    %% Plot 2: Charging Rate vs Particle Size (C/s)
    %subplot(2,3,2);
    figure;
    loglog(radii, charging_rates_C, 'o-', 'LineWidth', 2, 'MarkerSize', 8, 'MarkerFaceColor', 'blue');
    xlabel('Particle Radius (μm)');
    ylabel('Charging Rate (C/s)');
    title('Self-Charging Rate vs Particle Size');
    grid on;
    
    %% Plot 3: Charging Rate vs Particle Size (electrons/s)
    %subplot(2,3,3);
    figure;
    loglog(radii, charging_rates_electrons, 's-', 'LineWidth', 2, 'MarkerSize', 8, 'MarkerFaceColor', 'red');
    xlabel('Particle Radius (μm)');
    ylabel('Charging Rate (electrons/s)');
    title('Electron Escape Rate vs Particle Size');
    grid on;
    
    %% Plot 4: Scaling Analysis
    % figure;
    % % Fit power law: charging_rate = A * radius^n
    % if length(radii) >= 3
    %     log_radii = log10(radii);
    %     log_rates = log10(charging_rates_electrons);
    % 
    %     % Remove any infinite values
    %     finite_idx = isfinite(log_radii) & isfinite(log_rates);
    %     if sum(finite_idx) >= 2
    %         p = polyfit(log_radii(finite_idx), log_rates(finite_idx), 1);
    %         power_law_slope = p(1);
    %         power_law_intercept = p(2);
    % 
    %         % Plot data and fit
    %         loglog(radii, charging_rates_electrons, 'o', 'MarkerSize', 8, 'MarkerFaceColor', 'blue');
    %         hold on;
    %         radii_fit = logspace(log10(min(radii)), log10(max(radii)), 100);
    %         rates_fit = 10^power_law_intercept * radii_fit.^power_law_slope;
    %         loglog(radii_fit, rates_fit, 'r--', 'LineWidth', 2);
    % 
    %         xlabel('Particle Radius (μm)');
    %         ylabel('Charging Rate (C/s)');
    %         title(sprintf('Power Law Fit: Rate ∝ R^{%.2f}', power_law_slope));
    %         legend('Data', sprintf('Fit: R^{%.2f}', power_law_slope), 'Location', 'best');
    %         grid on;
    %     end
    % end
    
    %% Plot 5: Normalized Charging Rate
    %subplot(2,3,5);
    % Normalize by particle volume or surface area
    particle_volumes = (4/3) * pi * (radii*1e-4).^3;  % cm³
    normalized_rates = charging_rates_C ./ particle_volumes;  % C/s/cm³
    
    loglog(radii, normalized_rates, '^-', 'LineWidth', 2, 'MarkerSize', 8, 'MarkerFaceColor', 'green');
    xlabel('Particle Radius (μm)');
    ylabel('Charging Rate per Volume (C/s/cm³)');
    title('Volume-Normalized Charging Rate');
    grid on;
    
    %% Plot 6: Model Parameter Trends
    %subplot(2,3,6);
    model_params = vertcat(valid_results.model_params);
    a_values = model_params(:, 1);
    b_values = model_params(:, 2);
    c_values = model_params(:, 3);
    
    yyaxis left;
    semilogx(radii, a_values, 'o-', 'LineWidth', 2, 'DisplayName', 'Parameter a');
    hold on;
    semilogx(radii, b_values, 's-', 'LineWidth', 2, 'DisplayName', 'Parameter b');
    ylabel('Parameters a, b');
    
    yyaxis right;
    semilogx(radii, c_values, '^-', 'LineWidth', 2, 'DisplayName', 'Parameter c');
    ylabel('Parameter c');
    
    xlabel('Particle Radius (μm)');
    title('Model Parameter Trends');
    legend('show', 'Location', 'best');
    grid on;
    
    sgtitle('Self-Charging Analysis Using Exponential Models');
    
end

%% Key Insights
fprintf('\n=== KEY INSIGHTS ===\n');
if sum(valid_idx) > 1
    [max_rate, max_idx] = max(charging_rates_C);
    [min_rate, min_idx] = min(charging_rates_C);
    
    fprintf('Highest charging rate: %.2e C/s (R = %.1f μm)\n', max_rate, radii(max_idx));
    fprintf('Lowest charging rate: %.2e C/s (R = %.1f μm)\n', min_rate, radii(min_idx));
    fprintf('Rate ratio (max/min): %.1f\n', max_rate/min_rate);
    
    % Charging rate scaling
    if exist('power_law_slope', 'var')
        fprintf('Power law scaling: Charging Rate ∝ R^%.2f\n', power_law_slope);
        if power_law_slope > 3
            fprintf('  → Volume-dominated scaling (R³ dependence)\n');
        elseif power_law_slope > 2
            fprintf('  → Between surface and volume scaling\n');
        else
            fprintf('  → Surface-dominated scaling (R² dependence)\n');
        end
    end
end

fprintf('\nAnalysis complete using continuous exponential models.\n');

x1 = [0.010297916
0.011223149
0.012231383
0.013330122
0.014527525
0.015832562
0.017254778
0.018804897
0.020494149
0.022335186
0.024341608
0.026528434
0.028911722
0.031508984
0.034393901
0.037425225
0.040787792
0.044452127
0.048447095
0.05289523
0.05635361
0.060838235
0.065725397
0.071633969
0.07706073
0.083084433
0.090936931
0.098045312
0.103465722
0.11267863
0.121046536
0.135605223
0.14699857
0.159101496
0.17340749
0.189560731
0.203074518
0.220339312
0.24087013
0.257225245
0.288369331
0.31563536
0.34298505
0.376259472
0.411869159
0.446945813
0.487122009
0.530910873
0.578635442
0.630650393
0.687339892
0.749130485
0.816471651
0.8898655
0.969859378
1.057047029
1.152062521
1.255631925
1.36850973
1.49153363
1.62561834
1.771747586
1.931032892
2.104634729
2.293813429
2.50002742
2.724763366
2.969701589
3.236686474
3.527643819
3.844761748
4.200216957
4.56708553
4.977638956
5.425110057
5.912807134
6.444363276
7.023668953
7.655070488
8.343234116
9.093281907
9.910707867
10.80150155
11.77239241
12.83048422
13.98369104
15.24046296
16.61033447
18.1033353
19.73056044
21.50406438
23.43721975
25.54411567
27.84104531
30.34358538
33.07141797
36.04428838
39.28481004
42.81632985
46.66584778
50.86062106
55.43280041
60.41590989
65.84700091
71.76652534
78.21772483
85.25025018
92.91355424];

y1 = [1.4993E-06
1.88583E-06
2.40264E-06
3.08079E-06
3.96162E-06
5.06532E-06
6.50196E-06
8.26614E-06
1.05879E-05
1.35329E-05
1.7297E-05
2.19432E-05
2.78375E-05
3.55043E-05
4.45579E-05
5.6291E-05
7.07276E-05
8.9726E-05
0.000109788
0.000140211
0.000173076
0.000215887
0.000255953
0.000303988
0.000363604
0.00043598
0.00054702
0.000655906
0.000754244
0.000896553
0.001080582
0.001363327
0.00173982
0.002058345
0.002391631
0.00295956
0.003506752
0.004183666
0.005027394
0.006039338
0.007288224
0.00919708
0.010658591
0.01303576
0.015711236
0.018603861
0.022277066
0.026601469
0.031806103
0.038004646
0.045508405
0.054029605
0.064559182
0.077223343
0.092075937
0.109433595
0.131461508
0.15591017
0.185301513
0.22046915
0.262030802
0.313431703
0.37013605
0.438034719
0.526769745
0.624068576
0.744893536
0.88911123
1.049964364
1.25297839
1.492690106
1.79682815
2.116656219
2.524838544
3.004017606
3.574138157
4.238841428
5.059517011
6.019742615
7.160674231
8.494202672
10.13875221
12.25796879
14.77268664
17.91787554
21.70482729
26.47268222
31.9376725
38.53084914
46.40565944
55.84211298
66.36916634
79.04958011
91.57140193
110.1215085
130.7411585
156.2206936
184.2861439
219.4956347
257.823582
309.0593584
367.7145044
438.3131333
522.2055862
619.9857251
741.6043451
869.2412561
1039.310151];


figure;
loglog(radii, charging_rates_electrons, 's-', 'LineWidth', 2.5, 'MarkerSize', 10, 'MarkerFaceColor', 'red');
hold on
plot(x1,y1, 's-', 'LineWidth', 2.5, 'MarkerSize', 10, 'MarkerFaceColor', 'red');
xlabel('Particle Radius (μm)');
ylabel('Charging Rate (electrons/s)');
title('Electron Escape Rate vs Particle Size');
xlim = [0.1,20];
grid on;
box on;