clear;clc

function dGamma_dEe = tritium_beta_spectrum_normalized(Ee)
    % Masses (in MeV)
    me = 0.510998928; % electron mass
    Q_keV = 18.591; % Q-value in keV
    Q = Q_keV / 1000; % Convert to MeV
    
    % Electron momentum and kinetic energy
    kinetic_energy = Ee - me; % Kinetic energy in MeV
    
    % Step function
    valid = (Ee > me) & (kinetic_energy < Q) & (kinetic_energy > 0);
    
    % Main spectrum formula
    dGamma_dEe = zeros(size(Ee));
    
    for i = 1:length(Ee)
        if valid(i)
            T = kinetic_energy(i); % Kinetic energy
            remaining = Q - T; % Remaining energy
            % Beta decay spectrum formula
            dGamma_dEe(i) = sqrt(T) * (T + me) * remaining^2;
        end
    end
end

% Generate high-resolution spectrum for accurate integration
Q_keV = 18.591;
Ee_fine = linspace(0.511, 0.511 + Q_keV/1000, 5000); % High resolution
spectrum_fine = tritium_beta_spectrum_normalized(Ee_fine);

% Normalize so the total area under the curve = 1 (probability distribution)
energy_step_fine = (Ee_fine(2) - Ee_fine(1)) * 1000; % Convert to keV
total_area = sum(spectrum_fine) * energy_step_fine;
spectrum_normalized_fine = spectrum_fine / total_area;

% Convert to keV for easier handling
energy_keV_fine = (Ee_fine - 0.511) * 1000;

% Load your escape probability data
data = readtable('/Volumes/LaCie/MSc Nuclear Engineering/Thesis/DATAA/0.3um/output/averaged_escape_probabilities.csv');

% Get unique energies and radii
unique_energies = unique(data.Energy_keV);
unique_radii = unique(data.Radius_um);

fprintf('Energies in your data: ');
fprintf('%.0f ', unique_energies);
fprintf('keV\n');

%% CALCULATE INTEGRATED WEIGHTS
fprintf('\n=== CALCULATING INTEGRATED WEIGHTS ===\n');

% Define energy bin edges (halfway between your simulation energies)
energy_edges = zeros(length(unique_energies) + 1, 1);

% First and last edges
energy_edges(1) = unique_energies(1) - 0.5; % 0.5 keV below first energy
energy_edges(end) = unique_energies(end) + 0.5; % 0.5 keV above last energy

% Middle edges (halfway between adjacent energies)
for i = 2:length(unique_energies)
    energy_edges(i) = (unique_energies(i-1) + unique_energies(i)) / 2;
end

fprintf('Energy bin edges: ');
fprintf('%.1f ', energy_edges);
fprintf('keV\n');

% Calculate integrated weights for each energy bin
integrated_weights = zeros(length(unique_energies), 1);

for i = 1:length(unique_energies)
    E_low = energy_edges(i);
    E_high = energy_edges(i+1);
    
    % Find indices in fine spectrum corresponding to this bin
    bin_indices = (energy_keV_fine >= E_low) & (energy_keV_fine <= E_high);
    
    if sum(bin_indices) > 0
        % Integrate using trapezoidal rule
        E_bin = energy_keV_fine(bin_indices);
        spectrum_bin = spectrum_normalized_fine(bin_indices);
        integrated_weights(i) = trapz(E_bin, spectrum_bin);
    else
        % If no points in bin, use interpolation
        integrated_weights(i) = interp1(energy_keV_fine, spectrum_normalized_fine, unique_energies(i), 'linear', 0) * (E_high - E_low);
    end
    
    fprintf('Energy %2.0f keV: Bin [%.1f - %.1f] keV, Integrated weight: %.6f\n', ...
        unique_energies(i), E_low, E_high, integrated_weights(i));
end

% Normalize weights
integrated_weights_normalized = integrated_weights / sum(integrated_weights);

fprintf('\nNormalized integrated weights:\n');
for i = 1:length(unique_energies)
    fprintf('Energy: %2.0f keV, Weight: %.6f\n', unique_energies(i), integrated_weights_normalized(i));
end

%% APPLY WEIGHTS TO ESCAPE PROBABILITIES
fprintf('\n=== APPLYING TRITIUM WEIGHTS ===\n');

weighted_results = table();

for r_idx = 1:length(unique_radii)
    current_radius = unique_radii(r_idx);
    
    % Get data for this radius
    radius_data = data(data.Radius_um == current_radius, :);
    
    % Sort by energy to ensure correct order
    [~, sort_idx] = sort(radius_data.Energy_keV);
    radius_data = radius_data(sort_idx, :);
    
    % Apply integrated weights
    weighted_escape_prob = sum(radius_data.AvgEscapeProbability .* integrated_weights_normalized);
    weighted_num_escaped = sum(radius_data.AvgNumEscapedElectrons .* integrated_weights_normalized);
    
    % Store results
    temp_table = table(current_radius, weighted_escape_prob, weighted_num_escaped, ...
        'VariableNames', {'Radius_um', 'WeightedEscapeProbability', 'WeightedNumEscaped'});
    weighted_results = [weighted_results; temp_table];
end

% Display results
fprintf('\n=== TRITIUM-WEIGHTED ESCAPE PROBABILITIES ===\n');
fprintf('Radius (μm) | Weighted Escape Prob | Weighted Num Escaped\n');
fprintf('------------|---------------------|---------------------\n');
for i = 1:height(weighted_results)
    fprintf('%8.1f    |      %.6f        |      %.6f\n', ...
        weighted_results.Radius_um(i), ...
        weighted_results.WeightedEscapeProbability(i), ...
        weighted_results.WeightedNumEscaped(i));
end

%% SET LATEX FORMATTING
set(0, 'DefaultTextInterpreter', 'latex');
set(0, 'DefaultAxesTickLabelInterpreter', 'latex');
set(0, 'DefaultLegendInterpreter', 'latex');

%% FIGURE 1: TRITIUM SPECTRUM WITH ENERGY BINS
figure(1);
set(gcf, 'Position', [100, 100, 800, 600]);

plot(energy_keV_fine, spectrum_normalized_fine, 'b-', 'LineWidth', 2.5);
hold on;

% Show simulation energies as stems
stem(unique_energies, integrated_weights_normalized * max(spectrum_normalized_fine), ...
     'ro', 'LineWidth', 2, 'MarkerSize', 8, 'MarkerFaceColor', 'r');

% Show energy bin edges
for i = 1:length(energy_edges)
    xline(energy_edges(i), 'k--', 'Alpha', 0.6, 'LineWidth', 1.5);
end

xlabel('Energy (keV)', 'FontSize', 14);
ylabel('Normalized Spectrum', 'FontSize', 14);
title('Tritium $\beta^-$ Decay Spectrum with Energy Bins', 'FontSize', 16);
legend('Tritium Spectrum', 'Simulation Energies', 'Bin Edges', ...
       'Location', 'northeast', 'FontSize', 12);
grid on;
set(gca, 'FontSize', 12);
xlim([0, 20]);

% Export figure
% print(gcf, 'tritium_spectrum_bins', '-depsc', '-r300');

%% FIGURE 2: TRITIUM-WEIGHTED ESCAPE PROBABILITIES
figure(2);
set(gcf, 'Position', [200, 200, 800, 600]);

plot(weighted_results.Radius_um, weighted_results.WeightedEscapeProbability, ...
     'ro-', 'LineWidth', 2.5, 'MarkerSize', 8, 'MarkerFaceColor', 'r');

xlabel('Radius ($\mu$m)', 'FontSize', 14);
ylabel('Tritium-Weighted Escape Probability', 'FontSize', 14);
title('Radial Dependence of Tritium-Weighted Electron Escape', 'FontSize', 16);
grid on;
set(gca, 'FontSize', 12);

% Add text box with statistics
avg_escape = mean(weighted_results.WeightedEscapeProbability);
max_escape = max(weighted_results.WeightedEscapeProbability);
min_escape = min(weighted_results.WeightedEscapeProbability);

text_str = sprintf('Average: %.6f\nMax: %.6f\nMin: %.6f', avg_escape, max_escape, min_escape);
text(0.7, 0.8, text_str, 'Units', 'normalized', 'FontSize', 10, ...
     'BackgroundColor', 'white', 'EdgeColor', 'black');

% Export figure
% print(gcf, 'weighted_escape_probabilities', '-depsc', '-r300');

%% FIGURE 3: ENERGY DEPENDENCE OF ESCAPE YIELDS (UNWEIGHTED)
figure(3);
set(gcf, 'Position', [300, 300, 800, 600]);

colors = [
    0.8, 0.2, 0.2;    % Medium red
    0.2, 0.7, 0.2;    % Medium green
    0.2, 0.2, 0.8;    % Medium blue
    0.8, 0.5, 0.0;    % Medium orange
    0.6, 0.2, 0.7;    % Medium purple
    0.0, 0.7, 0.7;    % Medium cyan
    0.7, 0.7, 0.0;    % Medium yellow
    0.5, 0.5, 0.5;    % Medium gray
    0.8, 0.2, 0.5;    % Medium magenta
    0.3, 0.6, 0.3;    % Medium forest green
    0.6, 0.3, 0.1;    % Medium brown
    0.3, 0.3, 0.7;    % Medium navy
    0.7, 0.1, 0.3     % Medium maroon
];

hold on;
% Plot escape probability vs energy for each radius
for r_idx = 1:length(unique_radii)
 current_radius = unique_radii(r_idx);
% Get data for this radius
 radius_data = data(data.Radius_um == current_radius, :);
% Sort by energy
 [sorted_energies, sort_idx] = sort(radius_data.Energy_keV);
 sorted_escape_probs = radius_data.AvgEscapeProbability(sort_idx);
% Plot with different markers for each radius
 plot(sorted_energies, sorted_escape_probs, 'o-', ...
'Color', colors(r_idx,:), 'LineWidth', 2, 'MarkerSize', 6, ...
'MarkerFaceColor', colors(r_idx,:), ...
'DisplayName', sprintf('R = %.2f $\\mu$m', current_radius));
end
xlabel('Electron Energy (keV)', 'FontSize', 20);
ylabel('Escape Yield (-)', 'FontSize', 20);
%title('Energy Dependence of Electron Escape Yields', 'FontSize', 16);
legend('Location', 'best', 'FontSize', 18);
grid on;
box on;
set(gca, 'FontSize', 20);

% Export figure
% print(gcf, 'energy_dependence_escape_yields', '-depsc', '-r300');

%% FIGURE 4: THETA DEPENDENCE 
% USER PARAMETERS - Change these values as needed
selected_energies = [1, 5, 11, 15 , 17]; % keV 
%selected_radii = [4.9, 4.92, 4.94, 4.95, 4.96, 4.97, 4.98, 4.99 5]; % μm 
selected_radii = 0; % μm 

% Load raw angular data
raw_data = readtable('/Volumes/LaCie/MSc Nuclear Engineering/Thesis/DATAA/0.3um/output/combined_results/escape_probabilities.csv');


figure(4);
set(gcf, 'Position', [400, 400, 800, 600]);


colors = lines(length(selected_energies) * length(selected_radii));
line_styles = {'-', '--', '-.', ':'};
markers = {'o', 's', '^', 'd', 'v', '>', '<', 'p', 'h'};

hold on;
legend_entries = {};
color_idx = 1;

% Loop through all energy-radius combinations
for e_idx = 1:length(selected_energies)
    for r_idx = 1:length(selected_radii)
        current_energy = selected_energies(e_idx);
        current_radius = selected_radii(r_idx);
        
        % Filter data for selected energy and radius
        filtered_data = raw_data((raw_data.Energy_keV_ == current_energy) & ...
            (abs(raw_data.Radius_um_ - current_radius) < 0.001), :);
        
        if ~isempty(filtered_data)
            % Get unique theta values and calculate average escape probability for each theta
            unique_thetas = sort(unique(filtered_data.Theta));
            theta_avg = zeros(size(unique_thetas));
            
            for theta_idx = 1:length(unique_thetas)
                theta_data = filtered_data(filtered_data.Theta == unique_thetas(theta_idx), :);
                theta_avg(theta_idx) = mean(theta_data.EscapeProbability);
            end
            
            % Plot with unique styling for each combination
            line_style = line_styles{mod(e_idx-1, length(line_styles)) + 1};
            marker_style = markers{mod(r_idx-1, length(markers)) + 1};
            
            plot(unique_thetas, theta_avg, ...
                'Color', colors(color_idx,:), ...
                'LineStyle', line_style, ...
                'Marker', marker_style, ...
                'LineWidth', 2, ...
                'MarkerSize', 8, ...
                'MarkerFaceColor', colors(color_idx,:));
            
            % Create legend entry
            legend_entries{end+1} = sprintf('E = %d keV, R = %.2f μm', current_energy, current_radius);
            
            color_idx = color_idx + 1;
        else
            fprintf('Warning: No data found for Energy = %.0f keV and Radius = %.2f μm\n', ...
                current_energy, current_radius);
        end
    end
end

xlabel('Theta (degrees)', 'FontSize', 33);
ylabel('Escape Yield (-)', 'FontSize', 33);
%title('Theta Dependence of Electron Escape Probability', 'FontSize', 16);
legend(legend_entries, 'Location', 'best', 'FontSize', 30);
grid on;
box on;
set(gca, 'FontSize', 33);

% Export figure
% print(gcf, 'theta_dependence_comparison', '-depsc', '-r300');

%% SAVE RESULTS
%writetable(weighted_results, 'tritium_weighted_escape_probabilities.csv');

fprintf('\n=== SUMMARY ===\n');
% fprintf('Results saved to: tritium_weighted_escape_probabilities.csv\n');
% fprintf('Figures exported:\n');
% fprintf('  - tritium_spectrum_bins.eps\n');
% fprintf('  - weighted_escape_probabilities.eps\n');
% fprintf('  - energy_dependence_escape_yields.eps\n');
fprintf('Average weighted escape probability: %.6f\n', avg_escape);
fprintf('Range: %.6f to %.6f\n', min_escape, max_escape);