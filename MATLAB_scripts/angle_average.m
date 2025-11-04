clear;clc
%% Angular Averaging of Escape Probabilities
% Script to average escape probabilities over all theta and phi angles
% for each energy-radius combination

%% Load the original data
data = readtable('/Volumes/LaCie/MSc Nuclear Engineering/Thesis/DATAA/0.3um/output/combined_results/escape_probabilities.csv');

% Extract variables
energy = data.Energy_keV_;
radius = data.Radius_um_;
theta = data.Theta;
phi = data.Phi;
escape_prob = data.EscapeProbability;
num_escaped = data.NumEscapedElectrons;

%% Define the complete sampling grid that was actually used
% These are the energies and radii you actually sampled in your simulations
sampled_energies = [0.2, 0.5, 1, 3, 4, 5, 6, 7, 9, 11, 13, 15, 17];  % keV
%sampled_energies = 17;  % keV
sampled_radii = [0.15, 0.14, 0.13, 0.12, 0.11, 0.10, 0.09, 0.08, 0.07, 0.05, 0.03, 0.01, 0.00];  % μm
%sampled_radii = 4.99;  % μm

% Determine the full angular grid from the data
all_thetas = unique(theta);
all_phis = unique(phi);
expected_combinations = length(all_thetas) * length(all_phis);

fprintf('=== SAMPLING GRID ===\n');
fprintf('Sampled energies: %s keV\n', mat2str(sampled_energies));
fprintf('Sampled radii: %s μm\n', mat2str(sampled_radii));
fprintf('Expected angular combinations per energy-radius: %d (theta: %d, phi: %d)\n', ...
    expected_combinations, length(all_thetas), length(all_phis));
fprintf('Total expected energy-radius combinations: %d\n', ...
    length(sampled_energies) * length(sampled_radii));

% Pre-allocate arrays for ALL sampled combinations
total_combinations = length(sampled_energies) * length(sampled_radii);
averaged_energy = zeros(total_combinations, 1);
averaged_radius = zeros(total_combinations, 1);
averaged_escape_prob = zeros(total_combinations, 1);
averaged_num_escaped = zeros(total_combinations, 1);
num_angular_combinations = zeros(total_combinations, 1);
num_missing_combinations = zeros(total_combinations, 1);

%% Perform angular averaging for ALL sampled combinations
counter = 1;
for i = 1:length(sampled_energies)
    for j = 1:length(sampled_radii)
        E = sampled_energies(i);
        R = sampled_radii(j);
        
        % Find all data points for this energy-radius combination
        idx = (energy == E) & (radius == R);
        
        % Store the energy-radius pair regardless of whether data exists
        averaged_energy(counter) = E;
        averaged_radius(counter) = R;
        
        if sum(idx) > 0
            % Data exists - calculate averages including implicit zeros for missing combinations
            total_escape_prob = sum(escape_prob(idx));
            total_num_escaped = sum(num_escaped(idx));
            
            % Average over the FULL expected angular grid
            averaged_escape_prob(counter) = total_escape_prob / expected_combinations;
            averaged_num_escaped(counter) = total_num_escaped / expected_combinations;
            
            num_angular_combinations(counter) = sum(idx);
            num_missing_combinations(counter) = expected_combinations - sum(idx);
            
            % Display progress with missing combination info
            fprintf('E = %2d keV, R = %.2f μm: %2d present, %2d missing (%.1f%% coverage), avg escape prob = %.6f\n', ...
                E, R, sum(idx), num_missing_combinations(counter), ...
                100*sum(idx)/expected_combinations, averaged_escape_prob(counter));
        else
            % No data exists - this combination had zero escape probability
            averaged_escape_prob(counter) = 0;
            averaged_num_escaped(counter) = 0;
            num_angular_combinations(counter) = 0;
            num_missing_combinations(counter) = expected_combinations;
            
            fprintf('E = %2d keV, R = %.1f μm: %2d present, %2d missing (%.1f%% coverage), avg escape prob = %.6f [ALL ZERO]\n', ...
                E, R, 0, expected_combinations, 0.0, 0.0);
        end
        
        counter = counter + 1;
    end
end

%% Create results table
results = table(averaged_energy, averaged_radius, averaged_escape_prob, ...
                averaged_num_escaped, num_angular_combinations, num_missing_combinations, ...
                'VariableNames', {'Energy_keV', 'Radius_um', 'AvgEscapeProbability', ...
                'AvgNumEscapedElectrons', 'NumAngularCombinations', 'NumMissingCombinations'});

% Sort by energy then radius
results = sortrows(results, {'Energy_keV', 'Radius_um'});

%% Display results
fprintf('\n=== AVERAGED RESULTS (including zeros for missing angles) ===\n');
fprintf('Energy(keV)\tRadius(μm)\tAvg Escape Prob\t\tAvg Escaped Electrons\tPresent/Expected\tCoverage\n');
fprintf('--------------------------------------------------------------------------------------------------------\n');
for i = 1:height(results)
    coverage = 100 * results.NumAngularCombinations(i) / expected_combinations;
    fprintf('%8.0f\t%9.2f\t%15.6f\t%17.0f\t%12.0f/%3.0f\t%8.1f%%\n', ...
        results.Energy_keV(i), results.Radius_um(i), results.AvgEscapeProbability(i), ...
        results.AvgNumEscapedElectrons(i), results.NumAngularCombinations(i), ...
        expected_combinations, coverage);
end

%% Save results to CSV
writetable(results, 'averaged_escape_probabilities.csv');
fprintf('\nResults saved to: averaged_escape_probabilities.csv\n');

%% Summary statistics
fprintf('\n=== SUMMARY ===\n');
fprintf('Total energy-radius combinations: %d\n', height(results));
fprintf('Original data points: %d\n', height(data));
fprintf('Average angular combinations per energy-radius: %.1f\n', ...
    mean(results.NumAngularCombinations));
fprintf('Energy range: %.0f - %.0f keV\n', min(results.Energy_keV), max(results.Energy_keV));
fprintf('Radius range: %.1f - %.1f μm\n', min(results.Radius_um), max(results.Radius_um));
fprintf('Escape probability range: %.2e - %.4f\n', ...
    min(results.AvgEscapeProbability), max(results.AvgEscapeProbability));